#include "cwsdata.h"

/*
@brief traverses entire tree and prints all nodes

@param root Root of tree
@param depth Depth of node(s)
*/
void display_data(const cwsdata::Node& root, int depth = 0) {
	std::cout << std::string(depth, '\t') << root.name << ": " << root.value << std::endl;

	for (const auto& node : root.children) {
		display_data(*node, depth + 1);
	}
}

int main(int argc, char* argv[]) {
	// parsing file to a root node
	cwsdata::Node root = cwsdata::parse_file("test.cwsdata");

	// if root has no nodes, parsing failed (or file is empty)
	if (root.children.empty()) {
		std::cout << "Failed to open file" << std::endl;
		return -1;
	}

	// display the data
	display_data(root);

	// add arg1 and arg2 as a child node of `player`
	if (argc == 3) {
		std::cout << "Added new node to player!" << std::endl;
		root["player"].create_node(argv[1], argv[2]);
	}

	std::cout << "2nd element in array: " << root["player"]["list"][1].as_string() << std::endl;

	for (int i = 0; i < 10; i++) {
		root["player"]["list"].create_node(std::to_string(i));
	}

	// rewrite to original file
	cwsdata::write_file(root, "test.cwsdata");

	return 0;
}