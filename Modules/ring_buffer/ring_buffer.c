/******************************************************************************
 * @file      ring_buffer.c
 * @version   1.0
 * @date      Jul 13, 2022
 * @copyright
 *
 *****************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "utils/utils.h"
#include "ring_buffer/ring_buffer.h"


/* ---------------------------------------------------------------------------
 *
 * Put @ ring_buffer->tail : if RingBuffer->tail (ring_buffer->tail + 1) != ring_buffer->head (Full ring_buffer)
 * Get @ ring_buffer->head : if ring_buffer->head != ring_buffer->tail (Empty ring_buffer)
 *
 * ring_buffer invariable:
 * - tail : points to the location where a new item can be inserted
 * - head : points to the location of the oldest inserted item
 * - When reading, RingBuffer_IsEmpty : if RingBuffer->head== RingBuffer->tail
 * - When writing, RinBuffer_IsFull : if (RingBuffer->tail + 1) == RingBuffer->head
 *
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enInit(RingBuffer_t * ring_buffer, RingBuffer_Item_t const * const data, RingBuffer_Counter_t size)
{

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(data))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(size < 2)
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */

    ring_buffer->data = (RingBuffer_Item_t * const)data;
    ring_buffer->size = size;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enReset(RingBuffer_t * const ring_buffer)
{
#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    return RingBuffer_enInit(ring_buffer, ring_buffer->data, ring_buffer->size);
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enFree(RingBuffer_t * const ring_buffer)
{
#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    ring_buffer->data = NULL;

    return RING_BUFFER_ERROR_NONE;

}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enPutItem(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const item)
{
    RingBuffer_Counter_t RingBuffer_tail;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(item))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    /*  calculate new ring_buffer tail value  */

    RingBuffer_tail = ring_buffer->tail + 1;
    if(RingBuffer_tail == ring_buffer->size)
    {
        RingBuffer_tail = 0;
    }

    /*  check if ring_buffer is full  */
    if(RingBuffer_tail == ring_buffer->head)
    {
        return RING_BUFFER_ERROR_FULL;
    }

    /*  put item into ring_buffer  */
    memcpy(&ring_buffer->data[ring_buffer->tail], item, sizeof(RingBuffer_Item_t));

    /*  update ring_buffer tail pointer  */
    ring_buffer->tail = RingBuffer_tail;

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enPutItems(RingBuffer_t * const ring_buffer, RingBuffer_Item_t const * const items, RingBuffer_Counter_t len, RingBuffer_Counter_t * const item_count)
{
    RingBuffer_Counter_t tail;
    RingBuffer_Counter_t free_count;
    RingBuffer_Counter_t write_count;
    RingBuffer_Counter_t truncated_len;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(items) || IS_NULLPTR(item_count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(IS_ZERO(len))
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */

    /*  Get number of free items in ring buffer */
    RingBuffer_enFreeCount(ring_buffer, &free_count);

    /*  if ring buffer is full  */
    if(free_count == 0)
    {
        (*item_count) = 0;
        return RING_BUFFER_ERROR_FULL;
    }

    /*  maximum number of free items in the ring buffer  */
    truncated_len = MIN(len, free_count);

    /*  Get ring_buffer's current tail  */
    tail = ring_buffer->tail;

    /*
     * maximum number of items that can added to queue after tail pointer
     * either:
     *  - between tail & head (tail < head), or
     *  - between head & ring_buffer->size (tail > head)
     * */
    write_count = MIN((RingBuffer_Counter_t)(ring_buffer->size - tail), truncated_len);

    /*  copy items to ring buffer  */
    memcpy(
            &ring_buffer->data[tail],
            items,
            write_count
    );

    tail += write_count;
    truncated_len -= write_count;

    /*
     * check if there are still more items to write
     * in case (tail > head) && (head > 0)
     * there are some free items @ start of the ring buffer
     * */
    if(truncated_len)
    {
        memcpy(
                ring_buffer->data,
                &items[write_count],
                truncated_len
        );

        tail = truncated_len;
    }

    if(tail >= ring_buffer->size)
    {
        tail = 0;
    }

    /*  update ring_buffer's tail  */
    ring_buffer->tail = tail;

    (*item_count) = (truncated_len + write_count);

    if(free_count < len)
    {
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enGetItem(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const item)
{
    RingBuffer_Counter_t RingBuffer_head;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(item))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    RingBuffer_head = ring_buffer->head;

    /*  check if ring_buffer is empty  */
    if(RingBuffer_head == ring_buffer->tail)
    {
        return RING_BUFFER_ERROR_EMPTY;
    }

    /*  get item from ring_buffer  */
    memcpy(item, &ring_buffer->data[RingBuffer_head], sizeof(RingBuffer_Item_t));

    /*  calculate new ring_buffer head  */
    RingBuffer_head++;
    if(RingBuffer_head == ring_buffer->size)
    {
        RingBuffer_head = 0;
    }

    /*  update ring_buffer head pointer  */
    ring_buffer->head = RingBuffer_head;

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enGetItems(RingBuffer_t * const ring_buffer, RingBuffer_Item_t * const items, RingBuffer_Counter_t len, RingBuffer_Counter_t * const item_count)
{
    RingBuffer_Counter_t head;
    RingBuffer_Counter_t available_items;
    RingBuffer_Counter_t truncated_len;
    RingBuffer_Counter_t read_count;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(items) || IS_NULLPTR(item_count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(IS_ZERO(len))
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */


    /*  get number of available items in ring buffer  */
    RingBuffer_enItemCount(ring_buffer, &available_items);


    if(available_items == 0)
    {
        (*item_count) = 0;
        return RING_BUFFER_ERROR_EMPTY;
    }

    /*  maximum number of items available to read from ring buffer  */
    truncated_len = MIN(len, available_items);

    /*  Get ring_buffer head  */
    head = ring_buffer->head;

    /*
     * maximum number of items to read from ring buffer after head pointer
     * either:
     * - between head & tail (head < tail)
     * - between head & ring_buffer->size (head > tail)
     * */
    read_count = MIN((RingBuffer_Counter_t)(ring_buffer->size - head), truncated_len);

    /*  copy items from ring buffer  */
    memcpy(
            items,
            &ring_buffer->data[head],
            read_count
    );

    head += read_count;
    truncated_len -= read_count;

    /*
     * check if there are more items to read
     * (head > tail) && (tail >= 0)
     * */
    if(truncated_len)
    {
        memcpy(
                &items[read_count],
                ring_buffer->data,
                truncated_len
        );

        head = truncated_len;
    }

    if(head >= ring_buffer->size)
    {
        head = 0;
    }

    ring_buffer->head = head;

    (*item_count) = (truncated_len + read_count);

    if(available_items < len)
    {
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enPeekItems(RingBuffer_t * ring_buffer, RingBuffer_Item_t * items, RingBuffer_Counter_t len, RingBuffer_Counter_t offset, RingBuffer_Counter_t * item_count)
{
    RingBuffer_Counter_t head;
    RingBuffer_Counter_t tail;
    RingBuffer_Counter_t available_items;
    RingBuffer_Counter_t items_to_peek;
    RingBuffer_Counter_t read_count;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(items) || IS_NULLPTR(item_count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(IS_ZERO(len))
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */

    /*  Get copy of ring_buffer head & tail */
    head = ring_buffer->head;
    tail = ring_buffer->tail;

    /*  Check if ring_buffer is empty  */
    if(head == tail)
    {
        (*item_count) = 0;
        return RING_BUFFER_ERROR_EMPTY;
    }

    /**
     * 1 - available_items = number of available items in the ring buffer
     *
     * 2 - get total number of items to peek from ring buffer, according to offset :
     *
     *   - if offset < available_items, items_to_peek = available_items - offset. Else items_to_peek = 0 (return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS)
     *   -
     *
     * */

    /*  Get number of available items in the ring_buffer  */
    RingBuffer_enItemCount(ring_buffer, &available_items);

    /*  if offset >= number of items  */
    if(offset >= available_items)
    {
        (*item_count) = 0;
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    items_to_peek = MIN((RingBuffer_Counter_t)(available_items - offset), len);

    head += offset;
    if(head >= ring_buffer->size)
    {
        head = head - ring_buffer->size;
    }

    /*  Peek items from ring_buffer into data buffer  */
    read_count = MIN((RingBuffer_Counter_t)(ring_buffer->size - head), items_to_peek);

    memcpy(
            items,
            &ring_buffer->data[head],
            read_count * sizeof(RingBuffer_Item_t)
    );

    items_to_peek -= read_count;

    if(items_to_peek)
    {
        memcpy(
                &items[read_count],
                ring_buffer->data,
                items_to_peek * sizeof(RingBuffer_Item_t)
        );
    }

    (*item_count) = (items_to_peek + read_count);

    if((available_items - offset) < len)
    {
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enItemCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * item_count)
{
    RingBuffer_Counter_t tail;
    RingBuffer_Counter_t head;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(item_count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    tail = ring_buffer->tail;
    head = ring_buffer->head;

    if(tail > head)
    {
        (*item_count) = tail - head;
    }
    else if(tail < head)
    {
        (*item_count) = (RingBuffer_Counter_t)(tail + ring_buffer->size - head);
    }
    else
    {
        (*item_count) = 0;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enFreeCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * free_count)
{
    RingBuffer_Counter_t tail;
    RingBuffer_Counter_t head;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(free_count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    tail = ring_buffer->tail;
    head = ring_buffer->head;

    if(tail > head)
    {
        (*free_count) = (RingBuffer_Counter_t)(ring_buffer->size - (tail - head) - 1);
    }
    else if(head > tail)
    {
        (*free_count) = (RingBuffer_Counter_t)((head - tail) - 1);
    }
    else
    {
        (*free_count) = ring_buffer->size - 1;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enIsEmpty(RingBuffer_t * ring_buffer, uint8_t * is_empty)
{
#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(is_empty))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    (*is_empty) = (ring_buffer->head == ring_buffer->tail);

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enIsFull(RingBuffer_t * ring_buffer, uint8_t * is_full)
{
    RingBuffer_Counter_t count = 0;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(is_full))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    RingBuffer_enItemCount(ring_buffer, &count);

    (*is_full) = (count == (ring_buffer->size - 1));

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */
#ifdef DEBUG

const char * RingBuffer_pcError(RingBuffer_Error_t error)
{
    const char * error_string;

    switch(error)
    {
        case RING_BUFFER_ERROR_NONE:
        {
            error_string = "RING_BUFFER_ERROR_NONE";
        }
        break;

        case RING_BUFFER_ERROR_NULLPTR:
        {
            error_string = "RING_BUFFER_ERROR_NULLPTR";
        }
        break;

        case RING_BUFFER_ERROR_INVALID_PARAM:
        {
            error_string = "RING_BUFFER_ERROR_INVALID_PARAM";
        }
        break;

        case RING_BUFFER_ERROR_EMPTY:
        {
            error_string = "RING_BUFFER_ERROR_EMPTY";
        }
        break;

        case RING_BUFFER_ERROR_FULL:
        {
            error_string = "RING_BUFFER_ERROR_FULL";
        }
        break;

        case RING_BUFFER_ERROR_INSUFFICIENT_ITEMS:
        {
            error_string = "RING_BUFFER_ERROR_INSUFFICIENT_ITEMS";
        }
        break;

        default:
        {
            error_string = "UNKNOWN";
        }
    }

    return error_string;
}

#endif /*  DEBUG  */

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enSkipItems(RingBuffer_t * ring_buffer, RingBuffer_Counter_t skip_count, RingBuffer_Counter_t * skipped)
{
    RingBuffer_Counter_t head;
    RingBuffer_Counter_t item_count;
    RingBuffer_Counter_t skipped_items;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(skipped))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(IS_ZERO(skip_count))
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */

    RingBuffer_enItemCount(ring_buffer, &item_count);

    if(item_count == 0)
    {
        (*skipped) = 0;
        return RING_BUFFER_ERROR_EMPTY;
    }

    skipped_items = MIN(item_count, skip_count);

    head = ring_buffer->head;
    head += skipped_items;

    if(head >= ring_buffer->size)
    {
        head -= ring_buffer->size;
    }

    ring_buffer->head = head;
    (*skipped) = skipped_items;

    if(skipped_items != skip_count)
    {
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enAdvance(RingBuffer_t * ring_buffer, RingBuffer_Counter_t advance_count, RingBuffer_Counter_t * advanced)
{
    RingBuffer_Counter_t tail;
    RingBuffer_Counter_t free_count;
    RingBuffer_Counter_t advanced_items;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(advanced))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

    if(IS_ZERO(advance_count))
    {
        return RING_BUFFER_ERROR_INVALID_PARAM;
    }

#endif /*  DEBUG_RING_BUFFER  */

    RingBuffer_enFreeCount(ring_buffer, &free_count);

    if(free_count == 0)
    {
        (*advanced) = 0;
        return RING_BUFFER_ERROR_FULL;
    }

    advanced_items = MIN(free_count, advance_count);

    tail = ring_buffer->tail;
    tail += advance_count;

    if(tail >= ring_buffer->size)
    {
        tail -= ring_buffer->size;
    }

    ring_buffer->tail = tail;
    (*advanced) = advanced_items;

    if(advanced_items != advance_count)
    {
        return RING_BUFFER_ERROR_INSUFFICIENT_ITEMS;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enBlockReadAddress(RingBuffer_t * const ring_buffer, RingBuffer_Item_t ** const read_address)
{
#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(read_address))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    (*read_address) = &ring_buffer->data[ring_buffer->head];

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enBlockReadCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * count)
{
    RingBuffer_Counter_t head;
    RingBuffer_Counter_t tail;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    head = ring_buffer->head;
    tail = ring_buffer->tail;

    if(head < tail)
    {
        (*count) = tail - head;
    }
    else if(head > tail)
    {
        (*count) = ring_buffer->size - head;
    }
    else
    {
        (*count) = 0;
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enBlockWriteAddress(RingBuffer_t * ring_buffer, RingBuffer_Item_t ** write_address)
{
#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(write_address))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    (*write_address) = &ring_buffer->data[ring_buffer->tail];

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */

RingBuffer_Error_t RingBuffer_enBlockWriteCount(RingBuffer_t * ring_buffer, RingBuffer_Counter_t * count)
{
    RingBuffer_Counter_t head;
    RingBuffer_Counter_t tail;

#ifdef DEBUG_RING_BUFFER

    if(IS_NULLPTR(ring_buffer) || IS_NULLPTR(ring_buffer->data) || IS_NULLPTR(count))
    {
        return RING_BUFFER_ERROR_NULLPTR;
    }

#endif /*  DEBUG_RING_BUFFER  */

    head = ring_buffer->head;
    tail = ring_buffer->tail;

    if(head <= tail)
    {
        (*count) = ring_buffer->size - tail;

        if(head == 0)
        {
            (*count)--;
        }
    }
    else /* if(head > tail)  */
    {
        (*count) = (RingBuffer_Counter_t)(head - tail - 1);
    }

    return RING_BUFFER_ERROR_NONE;
}

/* ------------------------------------------------------------------------- */
