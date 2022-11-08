# Ring Buffer

A simple lock free, SPSC (single Producer, Single Consumer), FIFO (First In, Fist Out) [ring buffer](https://en.wikipedia.org/wiki/Circular_buffer) implementation in C. Ring buffer works as a wrapper around an array, providing item storage up to `(size) - 1` items. Heavily inspired by `lock free ring buffer` implementations by [QuantumLeap](https://github.com/QuantumLeaps/lock-free-ring-buffer) and `lwrb` implementation [MaJerle](https://github.com/MaJerle/lwrb).

## How it works

This implementation of ring buffer tries to be as simple as possible while providing an acceptable performance on small processors (micro-controlers for embedded systems). The implementation doesn't rely on locks or critical sections but relies on atomic operations (read/write) for ring buffer's read/write pointers.

To ensure correct operation, there are following conditions must be met:

- ring buffer is shared between only 2 threads or tasks.

- only 1 thread or task writes to the ring buffer, and only 1 thread or task reads from the ring buffer (single producer, single consumer). 

- ring buffer internal read/write pointers configured to match processor's general purpose registers size. For 8-bit processors, ring buffer's read/write pointers should be 8-bits wide. For a 32-bit processor, read/write pointers must be 32-bit wide <a href="#foot-note-1"><sup>1</sup></a>. 

- ring buffer's read/write ointers are aligned correctly

When the previous conditions are met, ring buffer will operate correctly even when the producer task and the consumer task interrupt each other.

## Usage

```C

/******************************************
 * file: main.c
 *****************************************/
#include "ring_buffer/ring_buffer.h"

#define RING_BUFFER_DATA_SIZE	256

int main(void)
{
    RingBuffer_t ring_buffer;   /* ring buffer instance */
    RingBuffer_Item_t ring_buffer_data [RING_BUFFER_DATA_SIZE] = {0};   /* ring buffer data (memory where ring buffer items will be stored) */
    RingBuffer_Error_t error;   /* ring buffer error code */

	RingBuffer_Item_t single_item;
	RingBuffer_Item_t items [RING_BUFFER_DATA_SIZE];
	RingBuffer_Counter_t count;
	uint8_t is_full;
	uint8_t is_empty;

	/* initialize ring buffer instance */
	error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, sizeof(ring_buffer_data) / sizeof(RingBuffer_Item_t));

	/* put an item into ring buffer */
	error = RingBuffer_enPutItem(&ring_buffer, &single_item);

	/* put multiple items into ring buffer */
	error = RingBuffer_enPutItems(&ring_buffer, items, 10, &count);

	/* peek multiple items (8) from ring buffer after an offset (3)  */
	error = RingBuffer_enPeekItems(&ring_buffer, items, 8, 3, &count);

	/* get number of free locations in the ring buffer */
	error = RingBuffer_enFreeCount(&ring_buffer, &count);

	/* get number of available items in the ring buffer */
	error = RingBuffer_enIemCount(&ring_buffer, &count);

	/* get an item from ring buffer */
	error = RingBuffer_enGetItem(&ring_buffer, &single_item);

	/* get multiple items from ring buffer */
	error = RingBuffer_enGetItems(&ring_buffer, items, 10, &count);

	/*  check if rig buffer is full  */
	error = RingBuffer_enIsFull(&ring_buffer, &is_full);

	/*  check if rig buffer is empty  */
	error = RingBuffer_enIsFull(&ring_buffer, &is_empty);
}

```

Check `main.c` for more usage examples, and documentation files

## Documentation

Check the header file `./Modules/ring_buffer/ring_buffer.h` for functions & their documentation. You can also build doxygen documentations, doxygen configuration file is in `./Docs/doxygen/Doxyfile`

## Configuring

Ring buffer can be configured using pre-processor definitions. These definition can be supplied during build (using -D option for GCC), or by editing it in `./Modules/ring_buffer/ring_buffer.h`. 

### RING_BUFFER_COUNTER_DATA_TYPE

This definition sets the data type for ring buffer's read/write pointers (`RingBuffer_Counter_t)`, which is used as the ring buffer's read/write pointer type. It's also used as the type for the ring buffer's `size`, which sets the maximum number of items the ring buffer can store. If not supplied, it defaults to `uint32_t`.

### RING_BUFFER_ITEM_DATA_TYPE

A pre-processor definition used to set ring buffer's data array type (`RingBuffer_Item_t`). Usually ring buffers array data type is `uint8_t`. This implementation of ring buffer uses `memcpy` to copy data to/from ring buffer data array, which allows storing user defined data. If not supplied, it defaults to `uint8_t`.

## Porability

- to include ring buffer in your project,  you only need 3 files, `./Modules/ring_buffer/ring_buffer.c`, `./Modules/ring_buffer/ring_buffer.h` and `./Modules/utils/utils.h`.

- written completely in C99, without using dynamic memory allocation (it's up to the caller to allocate ring buffer's memory, either on the heap or the stack)

## Build

A make file is supplied to build ring buffer as a static library for windows or STM32F1xx, using GCC. The makefile can be edited to build ring buffer using your toolchain,
or define your GCC path and/or prefix using `GCC_PATH=path/to/gcc/dir PREFIX=arm-none-eabi` when invoking make command.

Default target is `make all`, builds example code, tests and static library

### Build options

Build options that can be supplied when invoking make, multiple options can be used at the same time

```shell
make [option_1=vlaue [option_2=value ...]] [target]
```

- **GCC_PATH**: path to GCC directory, optional if GCC (compiler & build tools) is not in path

- **PREFIX**: GCC executable prefix (PREFIX-gcc.exe), default is `arm-none-eabi-` when building for `STM32F10xx` and empty when building for `Win`

- **platform**: accepted values \[Win STM32\]. Build for STM32F10xx or windows
  
  - *Win* : build for windows (default)

  - *STM32*: build for STM32F10xx

- **build**: build type, accepted values \[Debug, Release, RelMinSize\]
  
  - *Debug*: optimize for debugging and include deugging symbols in executable (default)

  - *Release*: compile with optimization using `-O2` optimization flag

  - *RelMinSize*: compile and optimize for size using `-Os` flag

### Build targets

All build targets (except for docs, clean_all and clean_docs) can be called with `platform` and `build` options

- **ringbuffer** : build ring buffer example code
	```shell
	make ringbuffer platform=Win build=Debug

	make ringbuffer platform=Win build=Relese

	make ringbuffer platform=Win build=RelMinSize

	make ringbuffer platform=STM32 build=Debug

	make ringbuffer platform=STM32 build=Relese

	make ringbuffer platform=STM32 build=RelMinSize
	```

- **libringbuffer** : build ring buffer as a static library
	```shell
	make libringbuffer platform=Win build=Debug

	make libringbuffer platform=Win build=Relese

	make libringbuffer platform=Win build=RelMinSize

	make libringbuffer platform=STM32 build=Debug

	make libringbuffer platform=STM32 build=Relese

	make libringbuffer platform=STM32 build=RelMinSize
	```
  
- **test** : build ring buffer test code, using unity framework for C unit testing
	```shell
	make test platform=Win build=Debug

	make test platform=Win build=Relese

	make test platform=STM32 build=Debug
	
	make test platform=STM32 build=Relese
	```

- **docs** : generate Doxygen documentation as HTML files
	```shell
	make docs
	```

- **clean**: clean build directory of current build
	```shell
	make clean platform=Win build=Debug
	```

- **clean_all**: cleans the root build directory

- **clean_docs**: cleans generated documenations


## Notes

<a id="foot-note-1"></a>

1. Ring buffer read/write pointer (`RingBuffer_Counter_t` type) can be configured as 8-bit or 16-bit for a 32-bit processor, if a the processor supports instructions to read/write 8-bit or 16-bit variables in a single cycle.

2. Almost all ring buffer functions include input parameters checking (check for null pointers, zero length data, etc). To enable input parameters checking, enable `DEBUG` pre-processor directive, either by including it in `main.h` or during vompile time `-DDEBUG`.
