#include "task.h"
#include "memory.h"
#include <stdint.h>

TCB tasks[MAX_TASKS];
int task_count = 0;

static void task_copy(TCB *dest, TCB *src)
{
    for (int i = 0; i < 31; i++)
        dest->regs[i] = src->regs[i];

    dest->entry = src->entry;
    dest->priority = src->priority;
    dest->stack = src->stack;
}

static void task_clear(TCB *task)
{
    for (int i = 0; i < 31; i++)
        task->regs[i] = 0;

    task->entry = 0;
    task->priority = 0;
    task->stack = 0;
}

int xTaskCreate(void (*task)(void),
                uint32_t stack_size,
                int priority)
{
    if (task_count >= MAX_TASKS)
        return -1;

    if (stack_size == 0)
        stack_size = DEFAULT_STACK_SIZE;

    TCB *t = &tasks[task_count];

    t->stack = (uint8_t*)kmalloc(stack_size);

    if (t->stack == 0)
        return -1;

    for (int i = 0; i < 31; i++)
        t->regs[i] = 0;

    uint64_t *sp = (uint64_t*)(t->stack + stack_size);

    /* Configurar contexto inicial */
    t->regs[0] = (uint64_t)task; // ra
    t->regs[1] = (uint64_t)sp;   // sp

    t->entry = task;
    t->priority = priority;

    int task_id = task_count;
    task_count++;

    return task_id;
}

void xTaskDelete(int task_id)
{
    if (task_id < 0 || task_id >= task_count)
        return;

    if (tasks[task_id].stack != 0)
    {
        kfree(tasks[task_id].stack);
        tasks[task_id].stack = 0;
    }

    for (int i = task_id; i < task_count - 1; i++)
        task_copy(&tasks[i], &tasks[i + 1]);

    task_count--;

    task_clear(&tasks[task_count]);
}