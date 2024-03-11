# Image Compression

This project involves image compression using a quaternary tree data structure. The code is divided into two main parts: the header file `image-comp.h` and the implementation file `image-comp.c`.

## image-comp.h

- **Structures**: Defines structures for the pixel matrix, the quaternary tree, and two queue structures.
- **Queue Functions**: Handles queue operations such as creating a node, checking if a queue is empty, enqueue, dequeue, and freeing memory.
- **Divisions Function**: Creates the quaternary tree based on the pixel matrix.
- **HeightTree Function**: Calculates the maximum number of arcs from the root node to the farthest leaf.
- **Leaf Counting**: Calculates the number of blocks in the image for which the similarity score is less than or equal to the provided factor.
- **Binary File Writing**: Writes the quaternary tree to a binary file.
- **Create_tree Function**: Reads the data from the binary file and creates the quaternary tree.
- **Decompression Function**: Assigns the data from the quaternary tree to the pixel matrix.
- **PPM Function**: Writes the information from the matrix to the *.ppm file.
- **Memory Management**: Frees the memory for the tree.

## image-comp.c

- **Argument Checking**: Checks if the number of arguments from the command line is correct.
- **File Reading**: Opens the *.ppm file and reads the header and pixel matrix.
- **Tree Creation**: Initializes the quaternary tree and calls the divisions function.
- **Information Writing**: Writes the number of levels, the number of leaves, and the size of the largest undivided area to a text file.
- **Binary File Writing**: Writes the quaternary tree to a binary file.
- **Memory Management**: Frees the memory for the matrix and the tree.

When the command line has 4 arguments, the program reads from a binary file, creates the tree, decompresses the image, and writes the decompressed image to a *.ppm file.
