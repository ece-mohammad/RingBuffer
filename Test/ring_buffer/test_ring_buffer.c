#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ring_buffer.h"
#include "utils.h"
#include "unity.h"
#include "test_ring_buffer.h"


#define LOCAL_ARRAY_LEN(data)   (sizeof((data)) / sizeof(RingBuffer_Item_t))


/* ------------------------------------------------------------------------- */
/* ------------------------ Test RingBuffer_enInit() ----------------------- */
/* ------------------------------------------------------------------------- */
#ifdef DEBUG

static void test_RingBuffer_enInit_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10];
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(NULL, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enInit_NULL_data(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, NULL, ring_buffer.size);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enInit_Zero_len(void)
{
    RingBuffer_Item_t ring_buffer_data [4];
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, 0);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enInit(void)
{
    RingBuffer_Item_t ring_buffer_data [4];
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
}

/* ------------------------------------------------------------------------- */
/* ----------------------- Test RingBuffer_enReset() ----------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enReset_NULL_buffer(void)
{
    RingBuffer_Error_t error;

    error = RingBuffer_enReset(NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enReset_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enReset(&ring_buffer);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enReset(void)
{
    RingBuffer_Item_t ring_buffer_data [4];
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enReset(&ring_buffer);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
}

/* ------------------------------------------------------------------------- */
/* ----------------------- Test RingBuffer_enPutItem() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enPutItem_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [4];
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t item = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItem(NULL, &item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItem_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t item = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enPutItem(&ring_buffer, &item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItem_NULL_item(void)
{
    RingBuffer_Item_t ring_buffer_data [4];
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItem(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enPutItem_head_lt_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t items [10] = {0};
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize items
    for(uint32_t i = 0; i < LOCAL_ARRAY_LEN(items); i++)
    {
        items[i] = (RingBuffer_Item_t)(i + 1);
    }

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [1:9]  */
            ring_buffer.tail = i;

            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            uint32_t free_count = ring_buffer.size - ring_buffer.tail + ring_buffer.head - 1;

            for(uint32_t k = 0; k < free_count; k++)
            {
                error = RingBuffer_enPutItem(&ring_buffer, &items[i]);
                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(j, ring_buffer.head);
                TEST_ASSERT_EQUAL((i + k + 1) % ring_buffer.size, ring_buffer.tail);
            }
        }
    }
}

static void test_RingBuffer_enPutItem_head_gt_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t items [10] = {0};
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize items
    for(uint32_t i = 0; i < LOCAL_ARRAY_LEN(items); i++)
    {
        items[i] = (RingBuffer_Item_t)(i + 1);
    }

    for(uint32_t i = 0; i < ring_buffer.size - 2; i++)
    {
        for(uint32_t j = i + 2; j < ring_buffer.size; j++)
        {
            /*  head [2:9], tail [0:7]  */
            RingBuffer_enReset(&ring_buffer);
            ring_buffer.tail = i;
            ring_buffer.head = j;

            error = RingBuffer_enPutItem(&ring_buffer, &items[i]);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i + 1, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enPutItem_head_eq_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t items [10] = {0};
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize items
    for(uint32_t i = 0; i < LOCAL_ARRAY_LEN(items); i++)
    {
        items[i] = (RingBuffer_Item_t)(i + 1);
    }

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        for(uint32_t j = 0; j < ring_buffer.size - 1; j++)
        {
            error = RingBuffer_enPutItem(&ring_buffer, &items[i]);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL((i + j + 1) % ring_buffer.size, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enPutItem_full_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t items [10] = {0};
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize items
    for(uint32_t i = 0; i < LOCAL_ARRAY_LEN(items); i++)
    {
        items[i] = (RingBuffer_Item_t)(i + 1);
    }

    for(uint32_t i = 0; i < ring_buffer.size - 1; i++)
    {
        ring_buffer.head = i + 1;
        ring_buffer.tail = i;

        error = RingBuffer_enPutItem(&ring_buffer, &items[i]);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_FULL, error);
        TEST_ASSERT_EQUAL(i + 1, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
    }
}

/* ------------------------------------------------------------------------- */
/* ---------------------- Test RingBuffer_enPutItems() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enPutItems_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t items [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(NULL, items, sizeof(items) / sizeof(RingBuffer_Item_t), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItems_Uninit_buffer(void)
{
    RingBuffer_Item_t items [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error = 0;

    error = RingBuffer_enPutItems(&ring_buffer, items, LOCAL_ARRAY_LEN(items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItems_NULL_items(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t items [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, NULL, sizeof(items) / sizeof(RingBuffer_Item_t), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItems_NULL_item_count(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t items [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, items, sizeof(items) / sizeof(RingBuffer_Item_t), NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPutItems_zero_items(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t items [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, items, 0, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enPutItems_head_lt_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {1, 2};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;
    uint32_t i;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize put_items
    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    put_items[i] = 0;

    // put items in ring buffer (fill ring buffer)
    for(i = 1; i < ring_buffer.size; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            /*  tail [1:9]  */
            ring_buffer.tail = i;

            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            uint32_t free_count = ring_buffer.size - ring_buffer.tail + ring_buffer.head - 1;

            if(free_count)
            {
                error = RingBuffer_enPutItems(&ring_buffer, put_items, free_count, &put_count);
                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(j, ring_buffer.head);
                TEST_ASSERT_EQUAL(((j + ring_buffer.size) - 1) % ring_buffer.size, ring_buffer.tail);
                TEST_ASSERT_EQUAL(free_count, put_count);
            }
        }
    }
}

static void test_RingBuffer_enPutItems_head_gt_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {1, 2};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;
    uint32_t i;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize put_items
    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    put_items[i] = 0;

    for(i = 2; i < ring_buffer.size; i++)
    {
        /*  head [2:9]  */
        ring_buffer.head = i;

        for(uint32_t j = 0; j < i - 1; j++)
        {
            /*  tail [0:7]  */
            ring_buffer.tail = j;

            error = RingBuffer_enPutItems(&ring_buffer, put_items, i - j - 1, &put_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(i - 1, ring_buffer.tail); // j + (i - j - 1) = i - 1
            TEST_ASSERT_EQUAL(i - j - 1, put_count);
        }
    }
}

static void test_RingBuffer_enPutItems_head_eq_tail(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {1, 2};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;
    uint32_t i;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize put_items
    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    put_items[i] = 0;

    for(i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enPutItems(&ring_buffer, &put_items[i], ring_buffer.size - 1, &put_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL((ring_buffer.size - 1 + i) % ring_buffer.size, ring_buffer.tail);
        TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);
    }
}

static void test_RingBuffer_enPutItems_full_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {1, 2};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Error_t error;
    uint32_t i;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // initialize put_items
    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    put_items[i] = 0;

    // put items in ring buffer (fill ring buffer)
    for(i = 0; i < ring_buffer.size; i++)
    {
        RingBuffer_enReset(&ring_buffer);
        ring_buffer.tail = i;
        ring_buffer.head = (i + 1) % ring_buffer.size;

        error = RingBuffer_enPutItems(&ring_buffer, &put_items[i], ring_buffer.size - 1, &put_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_FULL, error);
        TEST_ASSERT_EQUAL((i + 1) % ring_buffer.size, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, put_count);
    }
}

/* ------------------------------------------------------------------------- */
/* ----------------------- Test RingBuffer_enGetItem() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enGetItem_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t put_items [4] = {0x01, 0x02, 0x03, 0x04};
    RingBuffer_Item_t get_items [4] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItem(&ring_buffer, &put_items[0]);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(NULL, &get_items[0]);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItem_Uninit_buffer(void)
{
    RingBuffer_Item_t get_items [4] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enGetItem(NULL, &get_items[0]);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItem_NULL_item(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t put_items [4] = {0x01, 0x02, 0x03, 0x04};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItem(&ring_buffer, &put_items[0]);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enGetItem_head_lt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_item;
    RingBuffer_Error_t error;
    RingBuffer_Counter_t put_count;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enGetItem(&ring_buffer, &get_item);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j + 1, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enGetItem_head_gt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_item;
    RingBuffer_Error_t error;
    RingBuffer_Counter_t put_count;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9], tail [0:8]  */
        ring_buffer.head = i;
        ring_buffer.tail = i - 1;

        for(uint32_t j = 0; j < ring_buffer.size - 1; j++)
        {
            error = RingBuffer_enGetItem(&ring_buffer, &get_item);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL((i + j + 1) % ring_buffer.size, ring_buffer.head);
            TEST_ASSERT_EQUAL(i -1, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enGetItem_head_eq_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_item;
    RingBuffer_Error_t error;
    RingBuffer_Counter_t put_count;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        for(uint32_t j = 0; j < ring_buffer.size - 1; j++)
        {
            error = RingBuffer_enGetItem(&ring_buffer, &get_item);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enGetItem_empty_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t get_item;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, &get_item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(0, ring_buffer.tail);
}

/* ------------------------------------------------------------------------- */
/* ---------------------- Test RingBuffer_enGetItems() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enGetItems_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {0};
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t get_count = 0;
    RingBuffer_Error_t error;

    RingBuffer_Counter_t i = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItems(NULL, get_items, LOCAL_ARRAY_LEN(get_items) - 1, &get_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItems_Uninit_buffer(void)
{
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t get_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enGetItems(&ring_buffer, get_items, LOCAL_ARRAY_LEN(get_items) - 1, &get_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItems_NULL_items(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {0};
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t get_count = 0;
    RingBuffer_Error_t error;

    RingBuffer_Counter_t i = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItems(&ring_buffer, NULL, LOCAL_ARRAY_LEN(get_items) - 1, &get_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItems_NULL_item_count(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {0};
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;

    RingBuffer_Counter_t i = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItems(&ring_buffer, get_items, LOCAL_ARRAY_LEN(get_items) - 1, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enGetItems_zero_items(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {0};
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t get_count = 0;
    RingBuffer_Error_t error;

    RingBuffer_Counter_t i = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItems(&ring_buffer, get_items, 0, &get_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enGetItems_head_lt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t get_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enGetItems(&ring_buffer, get_items, i - j, &get_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(i - j, get_count);
        }
    }
}

static void test_RingBuffer_enGetItems_head_gt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t get_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            /*  head [1:9], tail [0:head - 1]  */
            ring_buffer.head = i;
            ring_buffer.tail = j;

            error = RingBuffer_enGetItems(&ring_buffer, get_items, ring_buffer.size - ( i - j), &get_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(j, ring_buffer.tail);
            TEST_ASSERT_EQUAL(ring_buffer.size - (i - j), get_count);
        }
    }
}

static void test_RingBuffer_enGetItems_head_eq_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t get_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 0; i < ring_buffer.size - 1; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.tail = ring_buffer.head = i;

        error = RingBuffer_enGetItems(&ring_buffer, get_items, LOCAL_ARRAY_LEN(get_items) - 1, &get_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, get_count);
    }
}

static void test_RingBuffer_enGetItems_empty_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t get_items [10] = {0};
    RingBuffer_Counter_t get_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItems(&ring_buffer, get_items, LOCAL_ARRAY_LEN(get_items) - 1, &get_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(0, ring_buffer.tail);
    TEST_ASSERT_EQUAL(0, get_count);
}

/* ------------------------------------------------------------------------- */
/* ---------------------- Test RingBuffer_enPeekItems() -------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enPeekItems_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t put_items [3] = {1, 2, 3};
    RingBuffer_Item_t peek_items [3] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t peek_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, sizeof(put_items) / sizeof(RingBuffer_Item_t), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPeekItems(NULL, peek_items, sizeof(peek_items) / sizeof(RingBuffer_Item_t), 0, &peek_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPeekItems_Uninit_buffer(void)
{
    RingBuffer_Item_t peek_items [3] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t peek_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, sizeof(peek_items) / sizeof(RingBuffer_Item_t), 0, &peek_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPeekItems_NULL_items(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t put_items [3] = {1, 2, 3};
    RingBuffer_Item_t peek_items [3] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t peek_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, sizeof(put_items) / sizeof(RingBuffer_Item_t), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPeekItems(&ring_buffer, NULL, sizeof(peek_items) / sizeof(RingBuffer_Item_t), 0, &peek_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enPeekItems_Zero_items(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_Item_t put_items [3] = {1, 2, 3};
    RingBuffer_Item_t peek_items [3] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t peek_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, sizeof(put_items) / sizeof(RingBuffer_Item_t), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, 0, 0, &peek_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enPeekItems_head_lt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t peek_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t peek_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        /*  offset [0:8], len [9:1]  */
        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            /*  offset [0:i - j - 1], len [1:i - j]  */
            for(uint32_t offset = 0; offset < i - j; offset++)
            {
                for(uint32_t len = 1; len < i - j - offset + 1; len++)
                {
                    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, len, offset, &peek_count);
                    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                    TEST_ASSERT_EQUAL(j, ring_buffer.head);
                    TEST_ASSERT_EQUAL(i, ring_buffer.tail);
                    TEST_ASSERT_EQUAL(len, peek_count);
                }
            }
        }
    }
}

static void test_RingBuffer_enPeekItems_head_gt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t peek_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t peek_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            /*  head [1:9], tail [0:head - 1]  */
            ring_buffer.head = i;
            ring_buffer.tail = j;

            /*  offset [0:i - j], len [i - j:1]  */
            for(uint32_t offset = 0; offset < ring_buffer.size - i + j; offset++)
            {
                for(uint32_t len = 1; len < ring_buffer.size - i + j - offset + 1; len++)
                {
                    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, len, offset, &peek_count);
                    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                    TEST_ASSERT_EQUAL(i, ring_buffer.head);
                    TEST_ASSERT_EQUAL(j, ring_buffer.tail);
                    TEST_ASSERT_EQUAL(len, peek_count);
                }
            }
        }
    }
}

static void test_RingBuffer_enPeekItems_head_eq_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = "abcdefghi";
    RingBuffer_Item_t peek_items [10] = {0};
    RingBuffer_Counter_t put_count;
    RingBuffer_Counter_t peek_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, LOCAL_ARRAY_LEN(put_items) - 1, &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, put_count);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        ring_buffer.tail = ring_buffer.head = i;

        /*  offset [0:8], len [9:1]  */
        for(uint32_t j = 0; j < ring_buffer.size - 1; j++)
        {
            for (uint32_t offset = 0; offset < ring_buffer.size - 1; offset++)
            {
                for (uint32_t len = 1; len < ring_buffer.size - offset; len++)
                {
                    error = RingBuffer_enPeekItems(&ring_buffer, peek_items, len, offset, &peek_count);
                    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
                    TEST_ASSERT_EQUAL(i, ring_buffer.head);
                    TEST_ASSERT_EQUAL(i, ring_buffer.tail);
                    TEST_ASSERT_EQUAL(0, peek_count);
                }
            }
        }
    }
}

static void test_RingBuffer_enPeekItems_empty_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t peek_items [10] = {0};
    RingBuffer_Counter_t peek_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    /*  offset [0:8], len [9:1]  */
    for(uint32_t j = 0; j < ring_buffer.size - 1; j++)
    {
        error = RingBuffer_enPeekItems(&ring_buffer, peek_items, LOCAL_ARRAY_LEN(peek_items) - j - 1, j, &peek_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
        TEST_ASSERT_EQUAL(0, ring_buffer.head);
        TEST_ASSERT_EQUAL(0, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, peek_count);
    }
}

/* ------------------------------------------------------------------------- */
/* ------------------ Test RingBuffer_enBlockReadAddress() ----------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enBlockReadAddress_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t * read_address = NULL;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enBlockReadAddress(NULL, &read_address);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockReadAddress_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t * read_address = NULL;
    RingBuffer_Error_t error;

    error = RingBuffer_enBlockReadAddress(&ring_buffer, &read_address);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockReadAddress_NULL_address(void)
{
    RingBuffer_Item_t ring_buffer_data [4] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enBlockReadAddress(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enBlockReadAddress(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [10] = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t get_item = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Item_t * read_address = NULL;
    RingBuffer_Error_t error;

    RingBuffer_Counter_t i;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(i = 0; i < (LOCAL_ARRAY_LEN(put_items) - 1); i++)
    {
        put_items[i] = (RingBuffer_Item_t)(i + 1);
    }

    put_items[i] = 0;

    // put items in ring buffer
    error = RingBuffer_enPutItems(&ring_buffer, put_items, (LOCAL_ARRAY_LEN(put_items) - 1), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(0, ring_buffer.head);
    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
    TEST_ASSERT_EQUAL((LOCAL_ARRAY_LEN(put_items) - 1), put_count);

    for(i = 0; i < ring_buffer.size - 1; i++)
    {
        // get block read address from ring buffer
        error = RingBuffer_enBlockReadAddress(&ring_buffer, &read_address);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
        TEST_ASSERT_EQUAL((RingBuffer_Item_t *)(ring_buffer.data + ring_buffer.head), read_address);

        // get an item from ring buffer
        error = RingBuffer_enGetItem(&ring_buffer, &get_item);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    }
}

/* ------------------------------------------------------------------------- */
/* ------------------- Test RingBuffer_enBlockReadCount() ------------------ */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enBlockReadCount_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t block_read_count = 0;
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enBlockReadCount(NULL, &block_read_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockReadCount_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t block_read_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enBlockReadCount(&ring_buffer, &block_read_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockReadCount_NULL_count(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enBlockReadCount(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enBlockReadCount_head_lt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t read_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enBlockReadCount(&ring_buffer, &read_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i - j, read_count);
        }
    }
}

static void test_RingBuffer_enBlockReadCount_head_gt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t read_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size - 1; i++)
    {
        /*  tail [0:8]  */
        ring_buffer.tail = i;

        for(uint32_t j = i + 1; j < ring_buffer.size; j++)
        {
            /*  head [1:9]  */
            ring_buffer.head = j;

            error = RingBuffer_enBlockReadCount(&ring_buffer, &read_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(ring_buffer.size - j, read_count);
        }
    }
}

static void test_RingBuffer_enBlockReadCount_head_eq_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t read_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0,9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enBlockReadCount(&ring_buffer, &read_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, read_count);
    }
}

static void test_RingBuffer_enBlockReadCount_empty_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t read_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0,9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enBlockReadCount(&ring_buffer, &read_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, read_count);
    }
}

/* ------------------------------------------------------------------------- */
/* ----------------- Test RingBuffer_enBlockWriteAddress() ----------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enBlockWriteAddress_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Item_t get_item = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t * write_address = NULL;
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;


    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, &get_item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(put_items[0], get_item);

    error = RingBuffer_enBlockWriteAddress(NULL, &write_address);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockWriteAddress_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t * write_address = NULL;
    RingBuffer_Error_t error;

    error = RingBuffer_enBlockWriteAddress(&ring_buffer, &write_address);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockWriteAddress_NULL_address(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Item_t get_item = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, &get_item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(put_items[0], get_item);

    error = RingBuffer_enBlockWriteAddress(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enBlockWriteAddress_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t * write_address;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1: 9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enBlockWriteAddress(&ring_buffer, &write_address);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(&ring_buffer.data[ring_buffer.tail], write_address);
        }
    }

}

static void test_RingBuffer_enBlockWriteAddress_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t * write_address;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1: 9]  */
        ring_buffer.head = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [0: head - 1]  */
            ring_buffer.tail = j;

            error = RingBuffer_enBlockWriteAddress(&ring_buffer, &write_address);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(j, ring_buffer.tail);
            TEST_ASSERT_EQUAL(&ring_buffer.data[ring_buffer.tail], write_address);
        }
    }
}

static void test_RingBuffer_enBlockWriteAddress_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t * write_address;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [0: 9], tail [0:9]  */
        ring_buffer.head = i;
        ring_buffer.tail = i;

        error = RingBuffer_enBlockWriteAddress(&ring_buffer, &write_address);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(&ring_buffer.data[ring_buffer.tail], write_address);
    }
}

static void test_RingBuffer_enBlockWriteAddress_full_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t * write_address;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0: 9], tail = (head + ring_buffer.size - 1) % ring_buffer.size  */
        ring_buffer.head = i;
        ring_buffer.tail = (ring_buffer.size + i - 1) % ring_buffer.size;

        error = RingBuffer_enBlockWriteAddress(&ring_buffer, &write_address);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(((ring_buffer.size + i - 1) % ring_buffer.size), ring_buffer.tail);
        TEST_ASSERT_EQUAL(&ring_buffer.data[ring_buffer.tail], write_address);
    }
}

/* ------------------------------------------------------------------------- */
/* ------------------ Test RingBuffer_enBlockWriteCount() ------------------ */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enBlockWriteCount_NULL_buffer(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Item_t get_item = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t block_write_count = 0;
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, &get_item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(put_items[0], get_item);

    error = RingBuffer_enBlockWriteCount(NULL, &block_write_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockWriteCount_Uninit_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t block_write_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enBlockWriteCount(&ring_buffer, &block_write_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enBlockWriteCount_NULL_count(void)
{
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Item_t get_item = {0};
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enPutItems(&ring_buffer, put_items, strlen((char *)put_items), &put_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enGetItem(&ring_buffer, &get_item);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(put_items[0], get_item);

    error = RingBuffer_enBlockWriteCount(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enBlockWriteCount_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t write_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [1: 9]  */
            ring_buffer.tail = i;

            /*  head [0: tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enBlockWriteCount(&ring_buffer, &write_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

            if(j == 0)
            {
                TEST_ASSERT_EQUAL(ring_buffer.size - i - 1, write_count);
            }
            else
            {
                TEST_ASSERT_EQUAL(ring_buffer.size - i, write_count);
            }

            if(write_count)
            {
                error = RingBuffer_enPutItems(&ring_buffer, put_items, write_count, &put_count);
                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(write_count, put_count);
                TEST_ASSERT_EQUAL(j, ring_buffer.head);

                if(j == 0)
                {
                    TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
                }
                else
                {
                    TEST_ASSERT_EQUAL(0, ring_buffer.tail);
                }
            }
        }
    }
}

static void test_RingBuffer_enBlockWriteCount_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t write_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [1: 9]  */
            ring_buffer.head = i;

            /*  head [0: head - 1]  */
            ring_buffer.tail = j;

            error = RingBuffer_enBlockWriteCount(&ring_buffer, &write_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i - j - 1, write_count);

            if(write_count)
            {
                error = RingBuffer_enPutItems(&ring_buffer, put_items, write_count, &put_count);
                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(write_count, put_count);
                TEST_ASSERT_EQUAL(i, ring_buffer.head);
                TEST_ASSERT_EQUAL((i + ring_buffer.size - 1) % ring_buffer.size, ring_buffer.tail);
            }
        }
    }
}

static void test_RingBuffer_enBlockWriteCount_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t write_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0: 9], tail [0: 9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enBlockWriteCount(&ring_buffer, &write_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        if(i == 0)
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - 1, write_count);
        }
        else
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - ring_buffer.tail, write_count);
        }

        error = RingBuffer_enPutItems(&ring_buffer, put_items, write_count, &put_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(write_count, put_count);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        if(i == 0)
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
        }
        else
        {
            TEST_ASSERT_EQUAL(0, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enBlockWriteCount_buffer_empty(void)
{
RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Item_t put_items [] = "abcdefgh";
    RingBuffer_Counter_t put_count = 0;
    RingBuffer_Counter_t write_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0: 9], tail [0: 9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enBlockWriteCount(&ring_buffer, &write_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        if(i == 0)
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - 1, write_count);
        }
        else
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - ring_buffer.tail, write_count);
        }

        error = RingBuffer_enPutItems(&ring_buffer, put_items, write_count, &put_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(write_count, put_count);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        if(i == 0)
        {
            TEST_ASSERT_EQUAL(ring_buffer.size - 1, ring_buffer.tail);
        }
        else
        {
            TEST_ASSERT_EQUAL(0, ring_buffer.tail);
        }
    }
}

static void test_RingBuffer_enBlockWriteCount_buffer_full(void)
{
RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t write_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  tail [0: 9], head = (tail + 1)%ring_buffer.size  */
        ring_buffer.head = ring_buffer.tail = i;
        ring_buffer.head = (i + 1) % ring_buffer.size;

        error = RingBuffer_enBlockWriteCount(&ring_buffer, &write_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(0, write_count);
    }
}

/* ------------------------------------------------------------------------- */
/* --------------------- Test RingBuffer_enSkipItems() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enSkipItems_NULL_skipped(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enSkipItems(NULL, 1, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);

}

static void test_RingBuffer_enSkipItems_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    RingBuffer_Counter_t skip_count = 0;

    error = RingBuffer_enSkipItems(NULL, 1, &skip_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enSkipItems_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    RingBuffer_Counter_t skip_count = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enSkipItems(NULL, 1, &skip_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enSkipItems_Zero_Items(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enSkipItems(&ring_buffer, 0, &skip_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enSkipItems_head_lt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            for(uint32_t k = 1; k < i - j; k++)
            {
                /*  head [0:tail - 1]  */
                ring_buffer.head = j;

                error = RingBuffer_enSkipItems(&ring_buffer, k, &skip_count);

                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(i, ring_buffer.tail);
                TEST_ASSERT_EQUAL((j + k) % ring_buffer.size, ring_buffer.head);
                TEST_ASSERT_EQUAL(k, skip_count);
            }
        }
    }
}

static void test_RingBuffer_enSkipItems_head_gt_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            /*  tail [0:head - 1]  */
            ring_buffer.tail = j;

            uint32_t item_count = ring_buffer.size - i + j - 1;

            for(uint32_t k = 1; k < item_count; k++)
            {
                /*  head [1:9]  */
                ring_buffer.head = i;

                error = RingBuffer_enSkipItems(&ring_buffer, k, &skip_count);
                TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                TEST_ASSERT_EQUAL(j, ring_buffer.tail);
                TEST_ASSERT_EQUAL((i + k) % ring_buffer.size, ring_buffer.head);
                TEST_ASSERT_EQUAL(k, skip_count);
            }
        }
    }
}

static void test_RingBuffer_enSkipItems_head_eq_tail(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head = tail [0:9]  */
        ring_buffer.tail = ring_buffer.head = i;

        for(uint32_t k = 1; k < ring_buffer.size; k++)
        {
            error = RingBuffer_enSkipItems(&ring_buffer, k, &skip_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(0, skip_count);
        }
    }
}

static void test_RingBuffer_enSkipItems_empty_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head = tail [0:9]  */
        ring_buffer.tail = ring_buffer.head = i;

        for(uint32_t k = 1; k < ring_buffer.size; k++)
        {
            error = RingBuffer_enSkipItems(&ring_buffer, k, &skip_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_EMPTY, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(0, skip_count);
        }
    }
}

static void test_RingBuffer_enSkipItems_full_buffer(void)
{
    RingBuffer_t ring_buffer;
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t skip_count;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [0:9] */
        ring_buffer.tail = i;

        for(uint32_t k = 1; k < ring_buffer.size; k++)
        {
            /* head = (tail + 1) % ring_buffer.size  */
            ring_buffer.head = (i + 1) % ring_buffer.size;

            error = RingBuffer_enSkipItems(&ring_buffer, k, &skip_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL((i + 1 + k) % ring_buffer.size, ring_buffer.head);
            TEST_ASSERT_EQUAL(k, skip_count);
        }
    }
}

/* ------------------------------------------------------------------------- */
/* ---------------------- Test RingBuffer_enAdvance() ---------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enAdvance_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    RingBuffer_Counter_t advance_count = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enAdvance(NULL, 1, &advance_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enAdvance_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    RingBuffer_Counter_t advance_count = 0;

    error = RingBuffer_enAdvance(NULL, 1, &advance_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enAdvance_NULL_advanced(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enAdvance(&ring_buffer, 1, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enAdvance_Zero_Items(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    RingBuffer_Counter_t advance_count = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enAdvance(&ring_buffer, 0, &advance_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_INVALID_PARAM, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enAdvance_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t advance_count;
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // put items in ring buffer (fill ring buffer)
    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            uint32_t free_count = ring_buffer.size - i + j - 1;

            if(free_count)
            {
                for(uint32_t k = 1; k < free_count; k++)
                {
                    /*  tail [1:9]  */
                    ring_buffer.tail = i;

                    error = RingBuffer_enAdvance(&ring_buffer, k, &advance_count);
                    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                    TEST_ASSERT_EQUAL(j, ring_buffer.head);
                    TEST_ASSERT_EQUAL((i + k + ring_buffer.size) % ring_buffer.size, ring_buffer.tail);
                    TEST_ASSERT_EQUAL(k, advance_count);
                }
            }
        }
    }
}

static void test_RingBuffer_enAdvance_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t advance_count;
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // put items in ring buffer (fill ring buffer)
    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9]  */
        ring_buffer.head = i;

        for (uint32_t j = 0; j < i; j++)
        {
            uint32_t free_count = i - j - 1;

            if(free_count)
            {
                for(uint32_t k = 1; k < free_count; k++)
                {
                    /*  tail [0: head - 1]  */
                    ring_buffer.tail = j;

                    error = RingBuffer_enAdvance(&ring_buffer, k, &advance_count);
                    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
                    TEST_ASSERT_EQUAL(i, ring_buffer.head);
                    TEST_ASSERT_EQUAL((j + k + ring_buffer.size) % ring_buffer.size, ring_buffer.tail);
                    TEST_ASSERT_EQUAL(k, advance_count);
                }
            }
        }
    }
}

static void test_RingBuffer_enAdvance_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t advance_count;
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // put items in ring buffer (fill ring buffer)
    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9]  */
        ring_buffer.head = i;

        for(uint32_t k = 1; k < ring_buffer.size; k++)
        {
            /*  tail [0: 9]  */
            ring_buffer.tail = i;

            error = RingBuffer_enAdvance(&ring_buffer, k, &advance_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL((i + k + ring_buffer.size) % ring_buffer.size, ring_buffer.tail);
            TEST_ASSERT_EQUAL(k, advance_count);
        }
    }
}

static void test_RingBuffer_enAdvance_empty_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t advance_count;
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // put items in ring buffer (fill ring buffer)
    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9]  */
        ring_buffer.head = i;

        for(uint32_t k = 1; k < ring_buffer.size; k++)
        {
            /*  tail [0: 9]  */
            ring_buffer.tail = i;

            error = RingBuffer_enAdvance(&ring_buffer, k, &advance_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL((i + k + ring_buffer.size) % ring_buffer.size, ring_buffer.tail);
            TEST_ASSERT_EQUAL(k, advance_count);
        }
    }
}

static void test_RingBuffer_enAdvance_full_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t advance_count;
    RingBuffer_Error_t error;

    // initialize ring buffer
    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    // put items in ring buffer (fill ring buffer)
    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9]  */
        ring_buffer.head = i;
        /*  tail = ((head - 1) + ring_buffer.size) % ring_buffer.size  */
        ring_buffer.tail = (ring_buffer.size + i - 1) % ring_buffer.size;

        error = RingBuffer_enAdvance(&ring_buffer, 1, &advance_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_FULL, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL((ring_buffer.size + i - 1) % ring_buffer.size, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, advance_count);
    }
}

/** Test Queue Info APIs ------------------------------------------------ */

/* ------------------------------------------------------------------------- */
/* --------------------- Test RingBuffer_enItemCount() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enItemCount_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    RingBuffer_Counter_t item_count = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enItemCount(NULL, &item_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enItemCount_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    RingBuffer_Counter_t item_count = 0;

    error = RingBuffer_enItemCount(NULL, &item_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enItemCount_NULL_result(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enItemCount(NULL, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enItemCount_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t item_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0: tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enItemCount(&ring_buffer, &item_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i - j, item_count);
        }
    }
}

static void test_RingBuffer_enItemCount_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t item_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enItemCount(&ring_buffer, &item_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, item_count);
    }
}

static void test_RingBuffer_enItemCount_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t item_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enItemCount(&ring_buffer, &item_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, item_count);
    }
}

static void test_RingBuffer_enItemCount_buffer_empty(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    RingBuffer_Counter_t item_count = 0;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        ring_buffer.head = ring_buffer.tail = i;
        error = RingBuffer_enItemCount(&ring_buffer, &item_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(0, item_count);
    }
}

static void test_RingBuffer_enItemCount_buffer_full(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t item_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  tail [0:9]  */
        ring_buffer.tail = i;
        /*  head = r(ring_buffer.tail + 1)%ring_buffer.size  */
        ring_buffer.head = (i + 1) % ring_buffer.size;

        error = RingBuffer_enItemCount(&ring_buffer, &item_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL((i + 1) % ring_buffer.size, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(ring_buffer.size - 1, item_count);
    }
}

/* ------------------------------------------------------------------------- */
/* --------------------- Test RingBuffer_enFreeCount() --------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enFreeCount_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enFreeCount(NULL, &free_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enFreeCount_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    RingBuffer_Counter_t free_count = 0;

    error = RingBuffer_enFreeCount(NULL, &free_count);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enFreeCount_NULL_result(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enFreeCount(NULL, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enFreeCount_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(ring_buffer.size - i + j - 1, free_count);
        }
    }
}

static void test_RingBuffer_enFreeCount_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9]  */
        ring_buffer.head = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [0:head - 1]  */
            ring_buffer.tail = j;

            error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(j, ring_buffer.tail);
            TEST_ASSERT_EQUAL(i - j - 1, free_count);
        }
    }
}

static void test_RingBuffer_enFreeCount_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(ring_buffer.size - 1, free_count);
    }
}

static void test_RingBuffer_enFreeCount_buffer_empty(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [0:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(ring_buffer.size - 1, free_count);
    }
}

static void test_RingBuffer_enFreeCount_buffer_full(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Counter_t free_count = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  tail [0:9]  */
        ring_buffer.tail = i;

        /* head = (ring_buffer.tail + 1) % ring_buffer.size  */
        ring_buffer.head = (i + 1) % ring_buffer.size;

        error = RingBuffer_enFreeCount(&ring_buffer, &free_count);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL((i + 1) % ring_buffer.size, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(0, free_count);
    }
}

/* ------------------------------------------------------------------------- */
/* ---------------------- Test RingBuffer_enIsEmpty() ---------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enIsEmpty_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    uint8_t is_empty = FALSE;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enIsEmpty(NULL, &is_empty);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enIsEmpty_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    uint8_t is_empty = FALSE;

    error = RingBuffer_enIsEmpty(NULL, &is_empty);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enIsEmpty_NULL_result(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enIsEmpty(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enIsEmpty_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_empty = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;

            error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);
            TEST_ASSERT_EQUAL(FALSE, is_empty);
        }
    }
}

static void test_RingBuffer_enIsEmpty_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_empty = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9]  */
        ring_buffer.head = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [0:head - 1]  */
            ring_buffer.tail = j;

            error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(j, ring_buffer.tail);
            TEST_ASSERT_EQUAL(FALSE, is_empty);
        }
    }
}

static void test_RingBuffer_enIsEmpty_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_empty = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [1:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(TRUE, is_empty);
    }
}

static void test_RingBuffer_enIsEmpty_empty_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_empty = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [1:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(TRUE, is_empty);
    }
}

static void test_RingBuffer_enIsEmpty_full_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_empty = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  tail [0:9], head = (ring_buffer.tail + 1) % ring_buffer.size  */
        ring_buffer.tail = i;
        ring_buffer.head = (i + 1)%ring_buffer.size;

        error = RingBuffer_enIsEmpty(&ring_buffer, &is_empty);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL((i + 1)%ring_buffer.size, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(FALSE, is_empty);
    }
}

/* ------------------------------------------------------------------------- */
/* ----------------------- Test RingBuffer_enIsFull() ---------------------- */
/* ------------------------------------------------------------------------- */

#ifdef DEBUG

static void test_RingBuffer_enIsFull_NULL_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;
    uint8_t is_full = FALSE;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enIsFull(NULL, &is_full);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enIsFull_Uninit_buffer(void)
{
    RingBuffer_Error_t error;
    uint8_t is_full = FALSE;

    error = RingBuffer_enIsFull(NULL, &is_full);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

static void test_RingBuffer_enIsFull_NULL_result(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    error = RingBuffer_enIsFull(&ring_buffer, NULL);
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NULLPTR, error);
}

#endif /*  DEBUG  */

static void test_RingBuffer_enIsFull_head_lt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint32_t item_count = 0;
    uint8_t is_full = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  tail [1:9]  */
        ring_buffer.tail = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  head [0:tail - 1]  */
            ring_buffer.head = j;
            item_count = i - j;

            error = RingBuffer_enIsFull(&ring_buffer, &is_full);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(j, ring_buffer.head);
            TEST_ASSERT_EQUAL(i, ring_buffer.tail);

            if(item_count == (ring_buffer.size - 1))
            {
                TEST_ASSERT_EQUAL(TRUE, is_full);
            }
            else
            {
                TEST_ASSERT_EQUAL(FALSE, is_full);
            }
        }
    }
}

static void test_RingBuffer_enIsFull_head_gt_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint32_t item_count = 0;
    uint8_t is_full = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 1; i < ring_buffer.size; i++)
    {
        /*  head [1:9]  */
        ring_buffer.head = i;

        for(uint32_t j = 0; j < i; j++)
        {
            /*  tail [0:head - 1]  */
            ring_buffer.tail = j;
            item_count = ring_buffer.size - i + j;

            error = RingBuffer_enIsFull(&ring_buffer, &is_full);
            TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
            TEST_ASSERT_EQUAL(i, ring_buffer.head);
            TEST_ASSERT_EQUAL(j, ring_buffer.tail);

            if(item_count == (ring_buffer.size - 1))
            {
                TEST_ASSERT_EQUAL(TRUE, is_full);
            }
            else
            {
                TEST_ASSERT_EQUAL(FALSE, is_full);
            }
        }
    }
}

static void test_RingBuffer_enIsFull_head_eq_tail(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_full = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [1:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enIsFull(&ring_buffer, &is_full);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(FALSE, is_full);
    }
}

static void test_RingBuffer_enIsFull_empty_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_full = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  head [1:9], tail [0:9]  */
        ring_buffer.head = ring_buffer.tail = i;

        error = RingBuffer_enIsFull(&ring_buffer, &is_full);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL(i, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(FALSE, is_full);
    }
}

static void test_RingBuffer_enIsFull_full_buffer(void)
{
    RingBuffer_t ring_buffer = {0};
    RingBuffer_Item_t ring_buffer_data [10] = {0};
    uint8_t is_full = 0;
    RingBuffer_Error_t error;

    error = RingBuffer_enInit(&ring_buffer, ring_buffer_data, LOCAL_ARRAY_LEN(ring_buffer_data));
    TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);

    for(uint32_t i = 0; i < ring_buffer.size; i++)
    {
        /*  tail [0:9], head = (ring_buffer.tail + 1) % ring_buffer.size  */
        ring_buffer.tail = i;
        ring_buffer.head = (i + 1)%ring_buffer.size;

        error = RingBuffer_enIsFull(&ring_buffer, &is_full);
        TEST_ASSERT_EQUAL(RING_BUFFER_ERROR_NONE, error);
        TEST_ASSERT_EQUAL((i + 1)%ring_buffer.size, ring_buffer.head);
        TEST_ASSERT_EQUAL(i, ring_buffer.tail);
        TEST_ASSERT_EQUAL(TRUE, is_full);
    }
}

/* ------------------------------------------------------------------------- */
/* --------------------------- End of test cases --------------------------- */
/* ------------------------------------------------------------------------- */

void test_ring_buffer(void)
{

    /*  TEST_RING_BUFFER_INIT  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enInit_NULL_buffer);
    RUN_TEST(test_RingBuffer_enInit_NULL_data);
    RUN_TEST(test_RingBuffer_enInit_Zero_len);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enInit);

    /*  TEST_RING_BUFFER_RESET  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enReset_NULL_buffer);
    RUN_TEST(test_RingBuffer_enReset_Uninit_buffer);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enReset);

    /*  TEST_RING_BUFFER_PUT_ITEM  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enPutItem_NULL_buffer);
    RUN_TEST(test_RingBuffer_enPutItem_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enPutItem_NULL_item);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enPutItem_head_lt_tail);
    RUN_TEST(test_RingBuffer_enPutItem_head_gt_tail);
    RUN_TEST(test_RingBuffer_enPutItem_head_eq_tail);
    RUN_TEST(test_RingBuffer_enPutItem_full_buffer);

    /*  TEST_RING_BUFFER_PUT_ITEMS  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enPutItems_NULL_buffer);
    RUN_TEST(test_RingBuffer_enPutItems_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enPutItems_NULL_items);
    RUN_TEST(test_RingBuffer_enPutItems_NULL_item_count);
    RUN_TEST(test_RingBuffer_enPutItems_zero_items);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enPutItems_head_lt_tail);
    RUN_TEST(test_RingBuffer_enPutItems_head_gt_tail);
    RUN_TEST(test_RingBuffer_enPutItems_head_eq_tail);
    RUN_TEST(test_RingBuffer_enPutItems_full_buffer);

    /*  TEST_RING_BUFFER_GET_ITEM  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enGetItem_NULL_buffer);
    RUN_TEST(test_RingBuffer_enGetItem_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enGetItem_NULL_item);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enGetItem_head_lt_tail);
    RUN_TEST(test_RingBuffer_enGetItem_head_gt_tail);
    RUN_TEST(test_RingBuffer_enGetItem_head_eq_tail);
    RUN_TEST(test_RingBuffer_enGetItem_empty_buffer);

    /*  TEST_RING_BUFFER_GET_ITEMS  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enGetItems_NULL_buffer);
    RUN_TEST(test_RingBuffer_enGetItems_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enGetItems_NULL_items);
    RUN_TEST(test_RingBuffer_enGetItems_NULL_item_count);
    RUN_TEST(test_RingBuffer_enGetItems_zero_items);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enGetItems_head_lt_tail);
    RUN_TEST(test_RingBuffer_enGetItems_head_gt_tail);
    RUN_TEST(test_RingBuffer_enGetItems_head_eq_tail);
    RUN_TEST(test_RingBuffer_enGetItems_empty_buffer);

    /*  TEST_RING_BUFFER_PEEK_ITEMS  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enPeekItems_NULL_buffer);
    RUN_TEST(test_RingBuffer_enPeekItems_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enPeekItems_NULL_items);
    RUN_TEST(test_RingBuffer_enPeekItems_Zero_items);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enPeekItems_head_lt_tail);
    RUN_TEST(test_RingBuffer_enPeekItems_head_gt_tail);
    RUN_TEST(test_RingBuffer_enPeekItems_head_eq_tail);
    RUN_TEST(test_RingBuffer_enPeekItems_empty_buffer);

    /*  TEST_RING_BUFFER_BLOCK_READ_ADDRESS  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enBlockReadAddress_NULL_buffer);
    RUN_TEST(test_RingBuffer_enBlockReadAddress_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enBlockReadAddress_NULL_address);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enBlockReadAddress);

    /*  TEST_RING_BUFFER_BLOCK_READ_COUNT  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enBlockReadCount_NULL_buffer);
    RUN_TEST(test_RingBuffer_enBlockReadCount_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enBlockReadCount_NULL_count);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enBlockReadCount_head_lt_tail);
    RUN_TEST(test_RingBuffer_enBlockReadCount_head_gt_tail);
    RUN_TEST(test_RingBuffer_enBlockReadCount_head_eq_tail);
    RUN_TEST(test_RingBuffer_enBlockReadCount_empty_buffer);

    /*  TEST_RING_BUFFER_BLOCK_WRITE_ADDRESS  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_NULL_buffer);
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_NULL_address);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_head_lt_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_head_gt_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_head_eq_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteAddress_full_buffer);

    /*  TEST_RING_BUFFER_BLOCK_WRITE_COUNT  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enBlockWriteCount_NULL_buffer);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_NULL_count);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enBlockWriteCount_head_lt_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_head_gt_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_head_eq_tail);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_buffer_empty);
    RUN_TEST(test_RingBuffer_enBlockWriteCount_buffer_full);

    /*  TEST_RING_BUFFER_SKIP  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enSkipItems_NULL_buffer);
    RUN_TEST(test_RingBuffer_enSkipItems_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enSkipItems_NULL_skipped);
    RUN_TEST(test_RingBuffer_enSkipItems_Zero_Items);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enSkipItems_head_lt_tail);
    RUN_TEST(test_RingBuffer_enSkipItems_head_gt_tail);
    RUN_TEST(test_RingBuffer_enSkipItems_head_eq_tail);
    RUN_TEST(test_RingBuffer_enSkipItems_empty_buffer);
    RUN_TEST(test_RingBuffer_enSkipItems_full_buffer);

    /*  TEST_RING_BUFFER_ADVANCE  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enAdvance_NULL_buffer);
    RUN_TEST(test_RingBuffer_enAdvance_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enAdvance_NULL_advanced);
    RUN_TEST(test_RingBuffer_enAdvance_Zero_Items);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enAdvance_head_lt_tail);
    RUN_TEST(test_RingBuffer_enAdvance_head_gt_tail);
    RUN_TEST(test_RingBuffer_enAdvance_head_eq_tail);
    RUN_TEST(test_RingBuffer_enAdvance_empty_buffer);
    RUN_TEST(test_RingBuffer_enAdvance_full_buffer);

    /*  TEST_RING_BUFFER_ITEM_COUNT  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enItemCount_NULL_buffer);
    RUN_TEST(test_RingBuffer_enItemCount_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enItemCount_NULL_result);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enItemCount_head_lt_tail);
    RUN_TEST(test_RingBuffer_enItemCount_head_gt_tail);
    RUN_TEST(test_RingBuffer_enItemCount_head_eq_tail);
    RUN_TEST(test_RingBuffer_enItemCount_buffer_empty);
    RUN_TEST(test_RingBuffer_enItemCount_buffer_full);

    /*  TEST_RING_BUFFER_FREE_COUNT  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enFreeCount_NULL_buffer);
    RUN_TEST(test_RingBuffer_enFreeCount_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enFreeCount_NULL_result);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enFreeCount_head_lt_tail);
    RUN_TEST(test_RingBuffer_enFreeCount_head_gt_tail);
    RUN_TEST(test_RingBuffer_enFreeCount_head_eq_tail);
    RUN_TEST(test_RingBuffer_enFreeCount_buffer_empty);
    RUN_TEST(test_RingBuffer_enFreeCount_buffer_full);

    /*  TEST_RING_BUFFER_IS_EMPTY  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enIsEmpty_NULL_buffer);
    RUN_TEST(test_RingBuffer_enIsEmpty_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enIsEmpty_NULL_result);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enIsEmpty_head_lt_tail);
    RUN_TEST(test_RingBuffer_enIsEmpty_head_gt_tail);
    RUN_TEST(test_RingBuffer_enIsEmpty_head_eq_tail);
    RUN_TEST(test_RingBuffer_enIsEmpty_empty_buffer);
    RUN_TEST(test_RingBuffer_enIsEmpty_full_buffer);

    /*  TEST_RING_BUFFER_IS_FULL  */
#ifdef DEBUG
    RUN_TEST(test_RingBuffer_enIsFull_NULL_buffer);
    RUN_TEST(test_RingBuffer_enIsFull_Uninit_buffer);
    RUN_TEST(test_RingBuffer_enIsFull_NULL_result);
#endif /*  DEBUG  */
    RUN_TEST(test_RingBuffer_enIsFull_head_lt_tail);
    RUN_TEST(test_RingBuffer_enIsFull_head_gt_tail);
    RUN_TEST(test_RingBuffer_enIsFull_head_eq_tail);
    RUN_TEST(test_RingBuffer_enIsFull_empty_buffer);
    RUN_TEST(test_RingBuffer_enIsFull_full_buffer);
}
