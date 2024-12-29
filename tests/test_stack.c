#include <stdint.h>

#include "macros.h"
#include "stack.h"
#include "unity.h"

static stack s;

void setUp()
{
    init_stack(&s);
}

void tearDown()
{
    return;
}

void test_pop_validates_underflow()
{
    uint16_t address;
    TEST_ASSERT_FALSE(pop(&s, &address));
}

void test_push_pop_works_as_stack()
{
    uint16_t data[3] = {0x002, 0x020, 0x200};

    for (uint8_t i = 0; i < sizeof(data) / sizeof(uint16_t); i++) {
        TEST_ASSERT_TRUE(push(&s, data[i]));
    }

    for (uint8_t i = len(data) - 1; i < len(data) && i >= 0; i--) {
        uint16_t address;
        TEST_ASSERT_TRUE(pop(&s, &address));
        TEST_ASSERT_EQUAL_INT16(data[i], address);
    }
}

void test_peek_validates_underflow()
{
    uint16_t address;
    TEST_ASSERT_FALSE(peek(&s, &address));
}

void test_push_peek_works_as_stack()
{
    uint16_t data[3] = {0x002, 0x020, 0x200};

    for (uint8_t i = 0; i < sizeof(data) / sizeof(uint16_t); i++) {
        TEST_ASSERT_TRUE(push(&s, data[i]));
    }

    for (uint8_t i = len(data) - 1; i < len(data) && i >= 0; i--) {
        uint16_t address;
        TEST_ASSERT_TRUE(peek(&s, &address));
        // Keep the expected index static, as peek should not mutate the stack.
        TEST_ASSERT_EQUAL_INT16(data[2], address);
    }
}

void test_push_validates_overflow()
{
    for (uint8_t i = 0; i < STACK_SIZE; i++) {
        TEST_ASSERT_TRUE(push(&s, 0x200));
    }

    TEST_ASSERT_FALSE(push(&s, 0x200));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_pop_validates_underflow);
    RUN_TEST(test_push_pop_works_as_stack);
    RUN_TEST(test_peek_validates_underflow);
    RUN_TEST(test_push_peek_works_as_stack);
    RUN_TEST(test_push_validates_overflow);
    return UNITY_END();
}
