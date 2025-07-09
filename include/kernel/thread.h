#include <idt.h>
#include <stddef.h>

typedef struct {
  idt_stackFrame state;
  int requestAge; // Number of quantumns the thread has waited to execute
  int present; // useful for fixed size arrays. 1 if the thread exists
  int active;
} thread_t;

// changes output to the context of the thread that has been
// waiting the most quantumns to execute. If a thread was not chosen,
// output is unchanged, and return value < 0
// returns thread number of chosen thread
int threadChooser(idt_stackFrame *output, thread_t *threads,
                   size_t numThreads);

