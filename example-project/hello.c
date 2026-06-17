#include <csr.h>
#include <uart.h>
#define printf uart_printf

int a = 3;

int main() {
  printf("Hello, world!\n");
  printf("Hello %d\n", a);
  a = 12 * a;
  printf("Hello %d\n", a);
  printf("Cycles %d\n", read_cycle());
  return 0;
}
