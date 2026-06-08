#include "memory.h"

/*   Configuração do heap   */

#define HEAP_START 0x80400000UL
#define HEAP_SIZE  (8 * 1024 * 1024)   // 8 MB

typedef struct block
{
    uint64_t size;
    int free;
    struct block *next;
} block_t;

static uint8_t *heap_base = (uint8_t*)HEAP_START;
static block_t *free_list;

static uint64_t align8(uint64_t size)
{
    return (size + 7) & ~7ULL;
}

static void coalesce_free_blocks(void)
{
    block_t *current = free_list;

    while (current != 0 && current->next != 0)
    {
        if (current->free && current->next->free)
        {
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
        }
        else
        {
            current = current->next;
        }
    }
}

/*   Inicialização da lista livre   */

void memory_init(void)
{
    free_list = (block_t*)heap_base;
    free_list->size = HEAP_SIZE - sizeof(block_t);
    free_list->free = 1;
    free_list->next = 0;
}

/*   Alocação com Free List e política First Fit   */

void *kmalloc(uint64_t size)
{
    if (size == 0)
        return 0;

    size = align8(size);

    block_t *current = free_list;

    while (current != 0)
    {
        if (current->free && current->size >= size)
        {

            if (current->size >= size + sizeof(block_t) + 8)
            {
                /* Divide o bloco quando há espaço suficiente para criar outro bloco livre. */
                block_t *new_block = (block_t*)((uint8_t*)current + sizeof(block_t) + size);

                new_block->size = current->size - size - sizeof(block_t);
                new_block->free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->free = 0;

            return (void*)((uint8_t*)current + sizeof(block_t));
        }

        current = current->next;
    }

    return 0;
}

/*   Liberação de memória com coalescência   */

void kfree(void *ptr)
{
    if (ptr == 0)
        return;

    block_t *block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    block->free = 1;

    coalesce_free_blocks();
}

/*   Estatísticas do heap baseadas na lista de blocos   */

uint64_t memory_used(void)
{
    uint64_t used = 0;
    block_t *current = free_list;

    while (current != 0)
    {
        if (!current->free)
            used += current->size + sizeof(block_t);

        current = current->next;
    }

    return used;
}

uint64_t memory_free(void)
{
    uint64_t free_bytes = 0;
    block_t *current = free_list;

    while (current != 0)
    {
        if (current->free)
            free_bytes += current->size + sizeof(block_t);

        current = current->next;
    }

    return free_bytes;
}

uint64_t memory_total(void)
{
    return HEAP_SIZE;
}