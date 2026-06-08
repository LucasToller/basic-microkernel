#include "task.h"
#include "scheduler.h"
#include "memory.h"
#include "uart.h"

static void print_heap_stats(const char *label)
{
    uart_print(label);
    uart_print("\nHeap total: ");
    uart_print_uint(memory_total());
    uart_print(" bytes\nHeap usado: ");
    uart_print_uint(memory_used());
    uart_print(" bytes\nHeap livre: ");
    uart_print_uint(memory_free());
    uart_print(" bytes\n\n");
}

static void memory_allocator_test(void)
{
    uart_print("\n=== Teste do Free List Allocator ===\n");

    print_heap_stats("Estado inicial do heap");

    void *a = kmalloc(128);
    void *b = kmalloc(256);
    void *c = kmalloc(128);

    uart_print("Alocadas tres areas de memoria: A=128, B=256, C=128\n");
    print_heap_stats("Depois das alocacoes");

    kfree(b);

    uart_print("Bloco B liberado\n");
    print_heap_stats("Depois de liberar B");

    void *d = kmalloc(200);

    uart_print("Alocado bloco D=200 para testar reutilizacao do espaco de B\n");

    if (d == b)
        uart_print("Resultado: D reutilizou o bloco liberado por B\n");
    else
        uart_print("Resultado: D nao reutilizou exatamente o mesmo endereco de B\n");

    print_heap_stats("Depois de alocar D");

    kfree(a);
    kfree(d);
    kfree(c);

    uart_print("Blocos A, D e C liberados para testar coalescencia\n");
    print_heap_stats("Depois da coalescencia");
}

/*   Tasks   */

void task1()
{
    while (1)
    {
        uart_print("Task 1 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        yield();
    }
}

void task2()
{
    while (1)
    {
        uart_print("Task 2 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        yield();
    }
}

/*   Kernel   */

void kernel_main()
{
    memory_init();

    memory_allocator_test();

    uart_print("\n=== Kernel ===\n");

    xTaskCreate(task1, 2048, 1);
    xTaskCreate(task2, 2048, 1);

    scheduler_start();

    while (1);
}