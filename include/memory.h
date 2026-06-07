#pragma once
#include <stdint.h>

/* Inicializa o heap */
void memory_init(void);

/* Alocação dinâmica no heap */
void *kmalloc(uint64_t size);

/* Liberação de memória */
void kfree(void *ptr);

/* Estatísticas do heap */
uint64_t memory_used(void);
uint64_t memory_free(void);
uint64_t memory_total(void);