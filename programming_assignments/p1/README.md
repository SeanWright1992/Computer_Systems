This project is simulating a file system on a simulated disk. The construction 
of this project is a 4 layered project, with each layer containing the layer below it.
The layers from top to bottom are: shell, file system, basic file system, disk. 

The shell runs on putty and provides the user the ability to make/edit directories, make/edit files, clear disk, etc.

The file system layer was the section we focused mostly on, building the logic that correlates the shell commands
to the functions the basic file system provides, such as get_free_block(), reclaim_block(block#), write_block(...) etc.

The basic file system creates creates the different structs of blocks, the linked list behavior of the file system, etc.

The disk is the bottom layer that completes the mount & unmount operations, reading etc.
