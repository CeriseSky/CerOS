// lol

#include <stdio.h>

int main() {
  for(int i = 0; i < 256; i++)
    printf("extern void *idt_wrapper%i;\n", i);

  printf("\nvoid idt_init(idt_t *idt, uint16_t selector) {\n");
  for(int i = 0; i < 256; i++)
    printf("  idt_setEntry(idt, %i, &idt_wrapper%i, selector, \n"
           "             IDT_PRESENT | IDT_RING0 | %s);\n", i, i,
           i < 32 ? "IDT_TRAP" : "IDT_INTERRUPT");
  printf("}\n");

  return 0;
}

