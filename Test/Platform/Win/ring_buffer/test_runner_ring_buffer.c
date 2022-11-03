#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "ring_buffer.h"
#include "utils.h"
#include "unity.h"
#include "test_ring_buffer.h"


void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();

    test_ring_buffer();

    return UNITY_END();
}
