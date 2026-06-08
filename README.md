# BasicMicrokernel - Gerência de Memória

Projeto da disciplina de Sistemas Operacionais com implementação de um alocador dinâmico de memória para o microkernel.

## Objetivo

Substituir o alocador linear original por um **Free List Allocator** usando a política **First Fit**.

## Implementação

O heap foi dividido em blocos encadeados. Cada bloco possui um cabeçalho com:

* `size`: tamanho útil do bloco;
* `free`: indica se o bloco está livre ou ocupado;
* `next`: ponteiro para o próximo bloco.

A função `kmalloc()` percorre a lista de blocos e aloca o primeiro bloco livre com tamanho suficiente. Quando há espaço sobrando, o bloco é dividido.

A função `kfree()` marca o bloco como livre e executa coalescência, unindo blocos livres adjacentes.

Também foram implementadas as funções:

* `memory_used()`
* `memory_free()`
* `memory_total()`

## Integração com tasks

As stacks das tasks são alocadas dinamicamente com `kmalloc()`.

A função `xTaskCreate()` cria a task e aloca sua stack.  
A função `xTaskDelete()` remove a task e libera sua stack com `kfree()`.

## Testes implementados

Foram adicionados testes para demonstrar:

* múltiplas alocações;
* liberação de blocos;
* reutilização de memória liberada;
* divisão de blocos;
* coalescência;
* estatísticas do heap;
* alocação e liberação de stack de task.

## Pré-requisitos

```bash
sudo apt update
sudo apt install -y make gcc-riscv64-unknown-elf qemu-system-misc
```

## Compilação

Dentro da pasta do projeto, execute:

```bash
rm -f *.o kernel.elf
make
```

## Execução no QEMU

```bash
qemu-system-riscv64 -machine virt -kernel kernel.elf -nographic
```

Para executar por tempo limitado e salvar a saída:

```bash
timeout 2s qemu-system-riscv64 -machine virt -kernel kernel.elf -nographic > qemu_output.txt 2>&1 || true
sed -n '1,130p' qemu_output.txt
```

## Resultado esperado

A saída deve mostrar os testes do allocator, exibindo memória total, memória usada e memória livre antes e depois das alocações e liberações.

Também deve mostrar a criação de uma task temporária com stack dinâmica e a liberação dessa stack com `kfree()`.

Após os testes, o kernel inicia normalmente as tasks principais.