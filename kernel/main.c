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
    uart_print("\n=== Validacao do Free List Allocator ===\n");

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

    uart_print("Alocacao D=200 apos liberar B\n");

    if (d == b)
        uart_print("Resultado: D reutilizou o bloco liberado por B\n");
    else
        uart_print("Resultado: D nao reutilizou exatamente o mesmo endereco de B\n");

    print_heap_stats("Depois de alocar D");

    kfree(a);
    kfree(d);
    kfree(c);

    uart_print("Blocos A, D e C liberados\n");
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

void task_temp()
{
    while (1)
    {
        yield();
    }
}

static void task_stack_test(void)
{
    uart_print("\n=== Validacao de stack dinamica de task ===\n");

    print_heap_stats("Antes de criar task temporaria");

    int temp_id = xTaskCreate(task_temp, 1024, 1);

    if (temp_id >= 0)
    {
        uart_print("Task temporaria criada com stack de 1024 bytes\n");
        print_heap_stats("Depois de criar task temporaria");

        xTaskDelete(temp_id);

        uart_print("Task temporaria removida e stack liberada com kfree\n");
        print_heap_stats("Depois de remover task temporaria");
    }
    else
    {
        uart_print("Falha ao criar task temporaria\n");
    }
}

/*   Kernel   */

void kernel_main()
{
    memory_init();

    /* Cenários de validação do allocator */

    memory_allocator_test();

    /* Validação da integração entre tasks e o novo allocator */

    task_stack_test();

    uart_print("\n=== Kernel ===\n");

    xTaskCreate(task1, 2048, 1);
    xTaskCreate(task2, 2048, 1);

    scheduler_start();

    while (1);
}