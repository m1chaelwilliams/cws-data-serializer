# CWSDATA (Coding with Sphere's Data)

Simple header-only node-tree data serialization tool.

### Status = WIP
I hope to add more flexible array logic in the future. I want to add support for encapsulating array elements in "[]" so the text file is infinitely long... The rest of the program's functionality will most likely stay the same.

### Dependencies
- C++ Standard Library

### Features
- Writing to a file
- Reading to a file

### How does it work?

The parser uses a custom language similar to `YAML`. Nodes are defined by a key and value, the key being on the left of the colon and value (optional) being on the right. In order to nest a node as a child of a parent, indent it with <tab> one more than the target parent. Duplicate, unecessary tabs **will** cause undesired behavior.

Please reference the [Example](example/) program.