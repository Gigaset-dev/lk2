/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define UTSTACK_VERSION (2.3.0)
#define STACK_MAX_SIZE 100

struct Stack {
    uint32_t array[STACK_MAX_SIZE];
    uint32_t index;
};


/*
 * This file contains macros to manipulate a singly-linked list as a stack.
 *
 * To use utstack, your structure must have a "next" pointer.
 *
 * ----------------.EXAMPLE -------------------------
 * struct item {
 *      int id;
 *      struct item *next;
 * };
 *
 * struct item *stack = NULL;
 *
 * int main() {
 *      int count;
 *      struct item *tmp;
 *      struct item *item = malloc(sizeof *item);
 *      item->id = 42;
 *      STACK_COUNT(stack, tmp, count); assert(count == 0);
 *      STACK_PUSH(stack, item);
 *      STACK_COUNT(stack, tmp, count); assert(count == 1);
 *      STACK_POP(stack, item);
 *      free(item);
 *      STACK_COUNT(stack, tmp, count); assert(count == 0);
 * }
 * --------------------------------------------------
 */
#define STACK_INIT() struct Stack stack = {.index = -1}

#define STACK_EMPTY(st) ((st).index == -1)

#define STACK_TOP(st) (((st).index != -1) ? (st).array[(st).index] : NULL)


#define STACK_PUSH(st, value)               \
do {                                        \
    (st).index++;                           \
    (st).array[(st).index] = (value);       \
} while (0)

#define STACK_POP(st, result)               \
do {                                        \
  (result) = (st).array[(st).index];        \
  (st).index--;                             \
} while (0)

#define STACK_COUNT(st, el, counter)       ((counter) = (((st).index) + 1))
