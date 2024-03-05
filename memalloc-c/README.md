# Simple Memory Allocator in C

This simple memory allocator provides basic memory allocation functions similar to `malloc`, `free`, and `calloc`. It's designed for educational purposes and to demonstrate the basic concepts behind memory allocation.

## How to Use

1. **Include the Allocator**: Add the provided code to your project.

2. **Functionality**:
   - `malloc(size_t size)`: Allocates memory of size `size`.
   - `free(void *block)`: Frees memory allocated by `malloc`.
   - `calloc(size_t num, size_t nsize)`: Allocates memory for an array of elements.
   - `print_mem_list()`: Prints out information about the allocated memory blocks.

## Note

This allocator is not intended for production use and lacks many features found in professional memory allocators. It's a simple demonstration of memory management concepts in C.

Feel free to modify and experiment with the code to gain a better understanding of memory allocation in C.

Feel free to read the blog post i read to make this code. https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory
