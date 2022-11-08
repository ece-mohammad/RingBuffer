#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ring_buffer/ring_buffer.h"
#include "utils/utils.h"


#define RING_BUFFER_ASSERT(cond, on_fail, ...)   do{                                    \
                                                    if(!(cond))                         \
                                                    {                                   \
                                                        printf(on_fail, ##__VA_ARGS__); \
                                                        return 0;                       \
                                                    }                                   \
                                                }while(0);      

#define RING_BUFFER_DATA_SIZE        256


int main(void)
{
    RingBuffer_t ring_buffer;   /* ring buffer instance */
    RingBuffer_Item_t ring_buffer_data [RING_BUFFER_DATA_SIZE] = {0};   /* ring buffer data (memory where ring buffer items will be stored) */
    RingBuffer_Error_t error;   /* ring buffer error code */

    RingBuffer_Item_t put_items [RING_BUFFER_DATA_SIZE] = {0};
    RingBuffer_Item_t get_items [RING_BUFFER_DATA_SIZE] = {0};
    RingBuffer_Item_t peek_items [RING_BUFFER_DATA_SIZE] = {0};
    RingBuffer_Item_t single_item = 0;
    RingBuffer_Counter_t item_count = 0;
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t get_count = 0;
    RingBuffer_Counter_t peek_count = 0;
    RingBuffer_Counter_t block_write_size = 0;
    RingBuffer_Counter_t block_read_size = 0;
    RingBuffer_Counter_t advance_count = 0;
    RingBuffer_Counter_t skip_count = 0;
    RingBuffer_Item_t * block_read_address = 0;
    RingBuffer_Item_t * block_write_address = 0;
    uint8_t is_empty = 0;
    uint8_t is_full = 0;

    /**
     * initialize ring buffer instance
     */
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, sizeof(ring_buffer_data) / sizeof(RingBuffer_Item_t));
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));

    /**
     * check if buffer is empty
     */
    error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    if(is_empty)
    {
        puts("ring buffer is empty");
    }
    else
    {
        puts("ring buffer is not empty");
    }

    /** 
     * put an item into ring buffer
     */
    single_item = 'a';
    error = RingBuffer_enPutItem(&ring_buffer, &single_item);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));

    /**
     * get number of items available in ring buffer
     */
    error = RingBuffer_enItemCount(&ring_buffer, &item_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("available items: %d\n", item_count);

    /**
     * get number of free locations in ring buffer
     */
    error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("free locations: %d\n", free_count);

    /**
     * put multiple items into ring buffer
     */
    for(RingBuffer_Counter_t i = 0; i < sizeof(put_items); i++)
    {
        put_items[i] = i + 1;
    }

    error = RingBuffer_enPutItems(&ring_buffer, put_items, 10, &put_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    RING_BUFFER_ASSERT(put_count == 10, "put_count =/= 10\n");
    printf("put %d items into ring buffer\n", put_count);

    /**
     * put more items than available free locations in the ring buffer
     */
    error = RingBuffer_enPutItems(&ring_buffer, &put_items[10], sizeof(put_items) - 10, &put_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_INSUFFICIENT_ITEMS, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("put %d items into ring buffer\n", put_count);

    /**
     * check if buffer is full
     */
    error = RingBuffer_enIsFull(&ring_buffer, &is_full);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    if(is_full)
    {
        puts("ring buffer is full");
    }
    else
    {
        puts("ring buffer is not full");
    }

    /**
     * peek items from ring buffer
     */
    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, 10, 10, &peek_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    RING_BUFFER_ASSERT(peek_count == 10, "peek count =/= 10\n");

    /**
     * get number of items available in ring buffer
     */
    error = RingBuffer_enItemCount(&ring_buffer, &item_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("available items: %d\n", item_count);

    /**
     * get a item from ring buffer
     */
    error = RingBuffer_enGetItem(&ring_buffer, &single_item);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));

    /**
     * get multiple items from ring buffer
     */
    error = RingBuffer_enGetItems(&ring_buffer, get_items, 10, &get_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    RING_BUFFER_ASSERT(get_count == 10, "get count =/= 10\n");
    printf("got %d items from ring buffer\n", get_count);

    /**
     * get more items than available in the ring buffer
     */
    error = RingBuffer_enGetItems(&ring_buffer, &get_items[10], sizeof(get_items) - 10, &get_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_INSUFFICIENT_ITEMS, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("got %d items from ring buffer\n", get_count);

    /**
     * check if buffer is empty
     */
    error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    if(is_empty)
    {
        puts("ring buffer is empty");
    }
    else
    {
        puts("ring buffer is not empty");
    }

    /**
     * reset ring buffer
     */
    error = RingBuffer_enReset(&ring_buffer);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));

    /**
     * get block write size
     */
    error = RingBuffer_enBlockWriteCount(&ring_buffer, &block_write_size);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("write block size: %d\n", block_write_size);

    /**
     * get block write address & write items to ring buffer memory
     */
    error = RingBuffer_enBlockWriteAddress(&ring_buffer, &block_write_address);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    memcpy(block_write_address, put_items, sizeof(RingBuffer_Item_t) * block_write_size);

    /**
     * advance ring buffer by block_write_size (to register items copied to block_write_address)
     */
    error = RingBuffer_enAdvance(&ring_buffer, block_write_size, &advance_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("advanced %d items into ring buffer\n", advance_count);
    
    /**
     * get block read size
     */
    error = RingBuffer_enBlockReadCount(&ring_buffer, &block_read_size);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("read block size: %d\n", block_read_size);

    /**
     * get block read address & read items from ring buffer memory
     */
    error = RingBuffer_enBlockReadAddress(&ring_buffer, &block_read_address);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    memcpy(get_items, block_read_address, sizeof(RingBuffer_Item_t) * block_read_size);

    /**
     * advance ring buffer
     */
    error = RingBuffer_enSkipItems(&ring_buffer, block_read_size, &skip_count);
    RING_BUFFER_ASSERT(error == RING_BUFFER_ERROR_NONE, "Ring buffer error: %s\n", RingBuffer_pcError(error));
    printf("skipped %d items in ring buffer\n", skip_count);

    return 0;
}
