/******************************************************************************
 * @file      ring_buffer.h
 * @brief     A Lock free, SPSC (Single Producer Single Consumer),
 *            FIFO (First In, First Out) ring buffer.
 *
 * @details   The ring buffer implementation guarantees correct behavior
 *            under the following conditions:
 *              - Ring buffer counter type is chosen so that the processor can
 *              read/write (for atomic read/write)
 *              - Only 1 thread is writing to the ring_buffer
 *              - Only 1 thread is reading from the ring_buffer
 *
 *            The ring buffer implementation provides functions for single item,
 *            multiple items and block of items read/write.
 *            As well as ring buffer information functions is_empty, is_full,
 *            item_count and free_count.
 *
 * @version   1.0
 * @date      Jul 13, 2022
 * @copyright Licensed under The MIT License (MIT)
 *
 *            Copyright � 2022 tolba.mohammad.17@gmail.com
 *
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the �Software�), to deal in the Software without restriction,
 *            including without limitation the rights to use, copy, modify, merge,
 *            publish, distribute, sublicense, and/or sell copies of the Software,
 *            and to permit persons to whom the Software is furnished to do so,
 *            subject to the following conditions:
 *
 *              - The above copyright notice and this permission notice shall be
 *              included in all copies or substantial portions of the Software.
 *
 *            THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *            MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *            IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 *            ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *            TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *            THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifdef DEBUG
#define DEBUG_RING_BUFFER
#endif /*  DEBUG  */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RingBuffer Lock free SPSC (Single Producer, Single Consumer) ring buffer
 * @brief Lock free SPSC (Single Producer, Single Consumer) ring buffer
 * @details   The ring buffer implementation guarantees correct behavior
 *            under the following conditions:
 *              - Ring buffer counter type is chosen so that the processor can
 *              read/write (for atomic read/write)
 *              - Only 1 thread is writing to the ring_buffer
 *              - Only 1 thread is reading from the ring_buffer
 *
 *            The ring buffer implementation provides functions for single item,
 *            multiple items and block of items read/write.
 *            As well as ring buffer information functions is_empty, is_full,
 *            item_count and free_count.
 * @{
 * */

/* ------------------------------------------------------------------------- */
/* -------------------------- Configuration Macros ------------------------- */
/* ------------------------------------------------------------------------- */

/**
 * @brief Ring buffer item data type.
 *
 * @note Must be a basic data type so that items can be copied using assignment operator `=`
 *
 * */
#ifndef RING_BUFFER_ITEM_DATA_TYPE
#define RING_BUFFER_ITEM_DATA_TYPE      uint8_t
#endif /*  RING_BUFFER_ITEM_DATA_TYPE  */

/**
 * @brief Ring buffer counter data type.
 *
 * @note To guarantee correct behavior of ring buffer functions, without using locks,
 * counter data type must be a data type that the processor can read/write in a single cycle.
 *
 * */
#ifndef RING_BUFFER_COUNTER_DATA_TYPE
#define RING_BUFFER_COUNTER_DATA_TYPE   uint32_t
#endif /*  RING_BUFFER_COUNTER_DATA_TYPE  */

/* ------------------------------------------------------------------------- */
/* --------------------------- Type Definitions ---------------------------- */
/* ------------------------------------------------------------------------- */

/**
 * @brief Ring buffer data item type declaration
 * */
typedef RING_BUFFER_ITEM_DATA_TYPE RingBuffer_Item_t;

/**
 * @brief Ring buffer counter data type declaration
 * */
typedef RING_BUFFER_COUNTER_DATA_TYPE RingBuffer_Counter_t;

/**
 * @brief Ring buffer structure
 */
typedef struct RingBuffer_t {
    RingBuffer_Item_t * data;               /**<  pointer to ring buffer data  */
    RingBuffer_Counter_t size;              /**<  size of ring buffer, maximum number of items ring buffer can hold is `size - 1`  */
    volatile RingBuffer_Counter_t head;     /**<  ring buffer head pointer, used to read items from the buffer   */
    volatile RingBuffer_Counter_t tail;     /**<  ring buffer tail pointer, used to write items to the buffer  */
} RingBuffer_t;

/**
 * @brief Ring buffer error codes
 */
typedef enum RingBuffer_error_t {
    RING_BUFFER_ERROR_NONE,                 /**<  No error: function execution was successful  */
    RING_BUFFER_ERROR_NULLPTR,              /**<  Null pointer error: an unexpected null pointer  */
    RING_BUFFER_ERROR_INVALID_PARAM,        /**<  Invalid parameter error: a parameter with an invalid value was passed to the function. For example: zero for an array length  */
    RING_BUFFER_ERROR_EMPTY,                /**<  Execution failed because ring buffer is empty  */
    RING_BUFFER_ERROR_FULL,                 /**<  Execution failed because ring buffer is full  */
    RING_BUFFER_ERROR_INSUFFICIENT_ITEMS,   /**<  Requested operation was done on some of the requested data, because ring buffer has insufficient items  */
} RingBuffer_Error_t;

/* ------------------------------------------------------------------------- */
/* ------------------------- Function Declarations ------------------------- */
/* ------------------------------------------------------------------------- */


/** @brief Initialize ring buffer instance.
 *
 * @param [in] ring_buffer: pointer to ring buffer object
 * @param [in] data       : pointer to an array of bytes used for ring buffer's data storage
 * @param [in] size       : size of ring buffer data, must be > 1
 *
 * @note Ring buffer can hold at most `size - 1` items.
 *
 * @pre @p ring_buffer instance pointer is not null
 *
 * @pre @p data is not null
 *
 * @pre @p size > 1
 *
 * @post @p ring_buffer instance is initialized
 *
 * @return RingBuffer_Error_t :
 *         - #RING_BUFFER_ERROR_NONE            : no error
 *         - #RING_BUFFER_ERROR_NULLPTR         : @p ring_buffer or @p data pointers is NULL
 *         - #RING_BUFFER_ERROR_INVALID_PARAM   : @p size <= 1
 *
 */

RingBuffer_Error_t RingBuffer_enInit(RingBuffer_t * ring_buffer, RingBuffer_Item_t const * const data, RingBuffer_Counter_t size);


/** @brief Reset ring buffer instance
 *
 * @param [in] ring_buffer: pointer to ring buffer object
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p ring_buffer instance is reset (read, write pointers are reset to 0).
 *       the instance can still be used with ring buffer functions
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE    : no error
 *         - #RING_BUFFER_ERROR_NULLPTR : @p ring_buffer is NULL or was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enReset(RingBuffer_t * const ring_buffer);


/** @brief Free ring buffer data
 *
 * @param [in] ring_buffer: pointer to ring buffer object
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p ring_buffer data pointer is set to NULL (ring buffer doesn't use memory allocation).
 *
 * @note @p ring_buffer instance need to be initialized using RingBuffer_enInitialize() to be usable with other ring buffer functions.
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE    : no error
 *         - #RING_BUFFER_ERROR_NULLPTR : @p ring_buffer is NULL or was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enFree(RingBuffer_t * const ring_buffer);


/** @brief Put an item into ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [in] item         : pointer to ring buffer item to put into ring buffer
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not full
 *
 * @post @p item is added to @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE    : no error
 *         - #RING_BUFFER_ERROR_NULLPTR : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_FULL    : @p ring_buffer is full
 *
 */
RingBuffer_Error_t RingBuffer_enPutItem(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const item);


/** @brief Get an item from ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] item        : pointer to ring buffer item variable to store item taken from ring buffer
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not empty
 *
 * @post @p item pointer contains the oldest item in @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE    : no error
 *         - #RING_BUFFER_ERROR_NULLPTR : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_EMPTY         : @p ring_buffer is empty
 *
 */
RingBuffer_Error_t RingBuffer_enGetItem(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const item);


/** @brief Put multiple items into ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [in] items        : pointer to an array of ring buffer items
 * @param [in] len          : number of items to put into ring buffer
 * @param [out] item_count  : pointer to ring buffer counter variable to store number of items successfully put into the ring buffer.
 *
 *
 * @note If @p ring_buffer has enough free space for @p items, all the items will be put into @p ring_buffer
 *       and @p item_count will contain @p len.
 *
 * @note If @p ring_buffer has free space less than @p len, only some of the items will be put into @p ring_buffer,
 *       and @p item_count will contain then number of items put into @p ring_buffer.
 *
 * @note If @p ring_buffer is full, contents of @p item_count will be 0.
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not full
 *
 * @post @p items are added to @p ring_buffer
 *
 * @post @p item_count contains number of items added to @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_FULL                : @p ring_buffer is full
 *         - #RING_BUFFER_ERROR_INSUFFICIENT_ITEMS  : only some of the items were put into @p ring_buffer,
 *                                                    as @p ring_buffer didn't have enough free space to put all the items
 *
 */
RingBuffer_Error_t RingBuffer_enPutItems(RingBuffer_t * const ring_buffer, RingBuffer_Item_t const * const items, RingBuffer_Counter_t len, RingBuffer_Counter_t * const item_count);


/** @brief Get items from ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] items       : pointer to an array of ring buffer items
 * @param [in] len          : number of items to put into ring buffer
 * @param [out] item_count  : pointer to ring buffer counter variable to store number of items successfully taken from the ring buffer.
 *
 * @note If @p ring_buffer has enough items, all the items will be taken from @p ring_buffer
 *       and @p item_count will contain @p len.
 *
 * @note If @p ring_buffer has less items than @p len, only some of the items will be taken from @p ring_buffer,
 *       and @p item_count will contain then number of items taken from @p ring_buffer.
 *
 * @note If @p ring_buffer is empty, contents of @p item_count will be 0.
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not empty
 *
 * @post @p item_count items are taken from @p ring_buffer and copied into @p items
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : ring_buffer or item is NULL, or ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_EMPTY               : ring buffer is empty
 *         - #RING_BUFFER_ERROR_INSUFFICIENT_ITEMS  : only some of the items were taken from the ring buffer,
 *                                                    as ring buffer didn't have enough free space to take all the items
 *
 */
RingBuffer_Error_t RingBuffer_enGetItems(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const items, RingBuffer_Counter_t len, RingBuffer_Counter_t * const item_count);


/**
 * @brief Get ring buffer's read address (address where data can be read)
 *
 * @param [in] ring_buffer      : pointer to ring buffer object
 * @param [in] read_address     : pointer to pointer to RingBuffer_Item_t to store address of item to be read
 *                                (oldest item in the ring buffer)
 *
 * @pre @p ring_buffer is initialized
 *
 * @post @p read_address contains address of current item to be read from @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p read_address is NULL, or @p ring_buffer was not initialized
 *
 * */
RingBuffer_Error_t RingBuffer_enBlockReadAddress(RingBuffer_t * const ring_buffer, RingBuffer_Item_t ** const read_address);


/**
 * @brief Get ring buffer's number of items that can be read as a contiguous linear block in memory
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [in] item_count   : pointer to RingBuffer_Counter_t to store number of items that can be read
 *
 * @pre @p ring_buffer is initialized
 *
 * @post @p item_count contains number of items available to read as a contiguous block
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item_count is NULL, or @p ring_buffer was not initialized
 *
 * */
RingBuffer_Error_t RingBuffer_enBlockReadCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * item_count);


/**
 * @brief Get ring buffer's write address (address where data can be written)
 *
 * @param [in] ring_buffer      : pointer to ring buffer object
 * @param [in] write_address    : pointer to pointer to RingBuffer_Item_t to store address of item to be written
 *
 * @pre @p ring_buffer is initialized
 *
 * @post @p read_address contains address of current empty location where an item can be written into @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p write_address is NULL, or @p ring_buffer was not initialized
 *
 * */
RingBuffer_Error_t RingBuffer_enBlockWriteAddress(RingBuffer_t * ring_buffer, RingBuffer_Item_t ** write_address);


/**
 * @brief Get ring buffer's number of items that can be written as a contiguous linear block in memory
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [in] item_count   : pointer to RingBuffer_Counter_t to store number of items that can be written
 *
 * @pre @p ring_buffer is initialized
 *
 * @post @p item_count contains number of locations available to write as a contiguous block
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item_count is NULL, or @p ring_buffer was not initialized
 *
 * */
RingBuffer_Error_t RingBuffer_enBlockWriteCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * item_count);


/**
 * @brief Skip (ignore) oldest items in the ring buffer. The ring buffer head (read) pointer is moved ahead by @p skip_count.
 *         Has the same effect as RingBuffer_enGetItems() on the ring_buffer's read pointer.
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [in] skip_count   : number of items to skip in the ring buffer
 * @param [in] skipped      : pointer to RingBuffer_Counter_t variable to store number of skipped items
 *
 * @note If @p ring_buffer has enough items, requested @p skip_count items will be skipped (dropped) from @p ring_buffer.
 *       And @p skipped will be equal to @p skip_count
 *
 * @note If @p ring_buffer has less items than @p skip_count, all items in @p ring_buffer will be skipped (dropped).
 *       And @p skipped will contain number of skipped items
 *
 * @note If @p ring_buffer is empty, no items will be skipped and @p skipped will be 0
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not empty
 *
 * @post @p skipped contains number of items that were skipped in the ring buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_EMPTY               : @p ring_buffer is empty
 *         - #RING_BUFFER_ERROR_INSUFFICIENT_ITEMS  : only some of the items were skipped in @p ring buffer,
 *                                                    as ring buffer didn't have enough items
 *
 * */
RingBuffer_Error_t RingBuffer_enSkipItems(RingBuffer_t * ring_buffer, RingBuffer_Counter_t skip_count, RingBuffer_Counter_t * skipped);


/**
 * @brief Advance ring buffer's tail (write) pointer by @p advance_count.
 *         Has the same effect as RingBuffer_enPutItems() on the ring_buffer's write pointer.
 *
 * @param [in] ring_buffer      : pointer to ring buffer object
 * @param [in] advance_count    : number of items to advance through in the ring buffer
 * @param [in] advanced         : pointer to RingBuffer_Counter_t variable to store number advanced items
 *
 * @pre @p ring_buffer is initialized
 *
 * @pre @p ring_buffer is not full
 *
 * @post @p advanced contains number of locations that were added to @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_EMPTY               : @p ring_buffer is empty
 *         - #RING_BUFFER_ERROR_INSUFFICIENT_ITEMS  : only some of the items were advanced in @p ring_buffer,
 *                                                    as ring buffer didn't have enough items
 *
 * */
RingBuffer_Error_t RingBuffer_enAdvance(RingBuffer_t * ring_buffer, RingBuffer_Counter_t advance_count, RingBuffer_Counter_t * advanced);


/** @brief Read items from ring buffer without removing them from the ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] items       : pointer to an array of ring buffer items to read items from ring buffer into
 * @param [in] len          : number of items to peek from ring buffer
 * @param [in] offset       : number of items to skip before peeking (offset 0 : peek first item, offset 1: peek from second item)
 * @param [out] item_count  : pointer to ring buffer counter variable to store number of items successfully read from the ring buffer.
 *
 * @note If @p ring_buffer has enough items, all the items will be read from @p ring_buffer
 *       and @p item_count will contain @p len. If @p ring_buffer has less items than @p len, only
 *       some of the items will be read from @p ring_buffer, and @p item_count will contain then number
 *       of items taken from @p ring_buffer. If @p ring_buffer is empty, contents of @p item_count will be 0.
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @pre @p ring_buffer instance is not empty
 *
 * @post @p peek_count contains number of items copied from @p ring_buffer into @p items
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item is NULL, or @p ring_buffer was not initialized
 *         - #RING_BUFFER_ERROR_EMPTY               : @p ring_buffer is empty
 *         - #RING_BUFFER_ERROR_INSUFFICIENT_ITEMS  : only some of the @p items were read from @p ring_buffer,
 *                                                    as @p ring_buffer didn't have enough free space to read.
 *
 */
RingBuffer_Error_t RingBuffer_enPeekItems(RingBuffer_t * ring_buffer, RingBuffer_Item_t * items, RingBuffer_Counter_t len, RingBuffer_Counter_t offset, RingBuffer_Counter_t * item_count);


/** @brief Get number of items available in the ring buffer
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] is_empty    : pointer to RingBuffer_Counter_t variable
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p item_count contains number of available items in @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p item_count is NULL, or @p ring_buffer was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enItemCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * item_count);


/** @brief Get free space available in the ring buffer; i.e. number of items that can be inserted in the ring buffer to fill it
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] is_empty    : pointer to RingBuffer_Counter_t variable
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p free_count contains number of available write locations in @p ring_buffer
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p free_count is NULL, or @p ring_buffer was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enFreeCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * free_count);


/** @brief Check if ring buffer is empty
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] is_empty    : pointer to uint8_t variable. The variable content is 1 if the ring buffer is empty, 0 otherwise.
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p is_empty is #TRUE (1) if @p ring_buffer is empty. Otherwise, @p is_empty is #FALSE (0)
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p is_empty is NULL, or @p ring_buffer was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enIsEmpty(RingBuffer_t * ring_buffer, uint8_t * is_empty);


/** @brief Check if ring buffer is full
 *
 * @param [in] ring_buffer  : pointer to ring buffer object
 * @param [out] is_full     : a pointer to a uint8_t variable. The variable content is 1 if the ring buffer is full, 0 otherwise.
 *
 * @pre @p ring_buffer instance is initialized
 *
 * @post @p is_full is #TRUE (1) if @p ring_buffer is full (contains `size - 1` items). Otherwise, @p is_full is #FALSE (0)
 *
 * @return RingBuffer_Error_t
 *         - #RING_BUFFER_ERROR_NONE                : no error
 *         - #RING_BUFFER_ERROR_NULLPTR             : @p ring_buffer or @p is_full is NULL, or @p ring_buffer was not initialized
 *
 */
RingBuffer_Error_t RingBuffer_enIsFull(RingBuffer_t * ring_buffer, uint8_t * is_full);

#ifdef DEBUG

/** @brief Get a human readable ring buffer error
 *
 * @param [in] error : Ring buffer error code #RingBuffer_Error_t
 *
 * @return string representation of given ring buffer error code
 *
 */
const char * RingBuffer_pcError(RingBuffer_Error_t error);

#else

#define RingBuffer_pcError(error) "ring buffer error"

#endif /* DEBUG */

/* ------------------------------------------------------------------------- */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RING_BUFFER_H__ */
