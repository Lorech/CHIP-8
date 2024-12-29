#include "unity.h"

void run_memory_tests();

int main()
{
    UNITY_BEGIN();

    run_memory_tests();

    return UNITY_END();
}
