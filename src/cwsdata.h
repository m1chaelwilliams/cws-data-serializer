/*
@file nodetxt.h
@brief simple data parsing tool for data serialization.
*/

#ifndef CWSDATA_H
#define CWSDATA_H

#include <iostream>
#include <deque>
#include <fstream>
#include <stack>
#include <memory>

/*
@brief Coding with Sphere Data
*/
namespace cwsdata {

    /*
    @class Node
    @brief Tree is comprised on these nodes
    */
    class Node {
        public:
            /*
            @param name Name ID of node.
            @param value Value of node.
            @param depth Depth of node.
            */
            Node(const std::string& name, const std::string& value, int depth) {
                this->name = name;
                this->value = value;
                this->depth = depth;
            }

            /*
            @brief Default constructor for `Node`
            
            Sets `depth` to `-1`
            */
            Node() : depth(-1) {}

            ~Node() {
            }

            Node(const Node& other) : name(other.name), value(other.value), depth(other.depth) {
                for (const auto& child : other.children) {
                    children.push_back(std::make_unique<Node>(*child));
                }
            }

            Node& operator=(const Node& other) {
                if (this != &other) {
                    name = other.name;
                    value = other.value;
                    depth = other.depth;
                    children.clear();
                    for (const auto& child : other.children) {
                        children.push_back(std::make_unique<Node>(*child));
                    }
                }
                return *this;
            }

            void create_node(const std::string& name, const std::string& value) {
                children.push_back(std::make_unique<Node>(name, value, depth+1));
            }

            void append_node(const std::string& value) {
                children.push_back(std::make_unique<Node>(std::to_string(children.size()), value, depth+1));
            }

            void prepend_node(const std::string& value) {
                children.push_front(std::make_unique<Node>("0", value, depth+1));
            }

            std::string name;
            std::string value;
            bool list;
            int depth;
            std::deque<std::unique_ptr<Node>> children;

            Node& operator[](const std::string& child_name) {
                auto& childNode = get_child(child_name);
                if (!childNode) {
                    childNode = std::make_unique<Node>();
                }
                return *childNode;
            }

            Node& operator[](int index) {
                return *children[index];
            }

            std::unique_ptr<Node>& get_child(const std::string& key) {
                for (auto& child : children) {
                    if (child->name == key) {
                        return child;
                    }
                }

                // If the child is not found, you might want to handle this case accordingly.
                // For now, let's return a reference to a static node.
                static std::unique_ptr<Node> dummyNode = std::make_unique<Node>();
                return dummyNode;
            }

            // parsing

            int as_int() {
                try {
                    return std::stoi(value);
                } catch(const std::invalid_argument& e) {
                    std::cerr << "Error converting arg to int" << e.what() << std::endl;
                    return 0;
                }
            }

            int as_float() {
                try {
                    return std::stof(value);
                } catch(const std::invalid_argument& e) {
                    std::cerr << "Error converting arg to float" << e.what() << std::endl;
                    return 0.0f;
                }
            }

            const char* as_c_str() {
                try {
                    return value.c_str();
                } catch(const std::invalid_argument& e) {
                    std::cerr << "Error converting arg to c str" << e.what() << std::endl;
                    return "";
                }
            }

            const std::string& as_string() {
                return value;
            }
    };

    /*
    @brief Opens file and parses it to treenode data structure

    @param filename Filepath + filename
    @return Root node of the tree
    */
    inline Node parse_file(const char* filename) {
        std::ifstream stream(filename);

        if (!stream.is_open()) {
            throw std::runtime_error("Unable to open file: " + std::string(filename));
        }

        Node root("root", "", -1);

        std::stack<Node*> node_stack;
        node_stack.push(&root);

        std::string line;
        while (std::getline(stream, line)) {
            if (line.empty()) continue;
            if (line[0] == '#') continue;

            // get the depth of the new node
            int line_depth = 0;
            while (line_depth < line.size() && (line[line_depth] == ' ' || line[line_depth] == '\t')) {
                line_depth++;
            }

            // find the encapsulating parent node
            while (line_depth <= node_stack.top()->depth && node_stack.top()->depth >= 0) {
                node_stack.pop();
            }

            // if the line has  `[`, it is the start of a list
            if (line.find('[') != std::string::npos) {
                node_stack.top()->list = true;
                std::cout << node_stack.top()->name << " is a list!" << std::endl;
                continue;
            }

            // find the end name colon
            size_t end_name_colon = line.find(':');

            // if there is a colon
            if (end_name_colon != std::string::npos) {
                std::string name = line.substr(line_depth, end_name_colon - line_depth);

                size_t begin_value_pos = end_name_colon + 1;
                while (begin_value_pos < line.size() && (line[begin_value_pos] == ' ' || line[begin_value_pos] == '\t')) {
                    begin_value_pos++;
                }

                size_t end_pos = line.size();

                size_t possible_comment_pos = line.find('#');
                if (possible_comment_pos != std::string::npos) {
                    end_pos = possible_comment_pos;
                }

                std::string value = line.substr(begin_value_pos, end_pos - begin_value_pos);
            
                auto newNode = std::make_unique<Node>(name, value, line_depth);

                Node* rawNodePtr = newNode.get();  // Retrieve the raw pointer before moving it
                node_stack.top()->children.push_back(std::move(newNode));
                node_stack.push(rawNodePtr);  // Push the raw pointer
            }
        }
        return root;
    }

    /*
    @brief DO NOT USE THIS (please)
    */
    inline void _write_nodes(const Node& root, std::ofstream& stream, int depth) {
        stream << std::string(depth, '\t') << root.name << ": " << root.value << std::endl;


        for (const auto& node : root.children) {
            _write_nodes(*node, stream, depth+1);
        }
    }

    /*
    @brief Opens and writes node tree to a file.

    @param root Root node
    @param filename Filepath + filename

    @return `true` if success, `false` if failure
    */
    inline bool write_file(const Node& root, const char* filename) {
        std::ofstream stream(filename);

        if (!stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(filename));
            return false;
        }

        for (const auto& node : root.children)
            _write_nodes(*node, stream, 0);

        return true;
    }
};

#endif