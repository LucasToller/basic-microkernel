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
static uint8_t *heap_end  = (uint8_t*)(HEAP_START + HEAP_SIZE);
static uint8_t *heap_ptr;

static block_t *free_list;

/*   Inicialização   */

void memory_init(void)
{
    heap_ptr = heap_base;

    free_list = (block_t*)heap_base;
    free_list->size = HEAP_SIZE - sizeof(block_t);
    free_list->free = 1;
    free_list->next = 0;
}

/*   Alocador bump   */

void *kmalloc(uint64_t size)
{
    if (size == 0)
        return 0;

    /* Alinhamento para 8 bytes */
    size = (size + 7) & ~7ULL;

    if (heap_ptr + size > heap_end)
        return 0;   // out of memory

    void *ptr = heap_ptr;
    heap_ptr += size;

    return ptr;
}

/*   Free mínimo   */

void kfree(void *ptr)
{
    /* Implementação mínima: não faz nada */
    (void)ptr;
}

/*   Estatísticas   */

uint64_t memory_used(void)
{
    return (uint64_t)(heap_ptr - heap_base);
}

uint64_t memory_free(void)
{
    return (uint64_t)(heap_end - heap_ptr);
}

uint64_t memory_total(void)
{
    return HEAP_SIZE;
}