#include <thread.h>
#include <memory.h>

#include <vgatext.h>

int threadChooser(idt_stackFrame *output, thread_t *threads,
                   size_t numThreads) {
  if(numThreads == 0)
    return -1;

  int targetThreadIndex = -1;
  thread_t *targetThread;
  for(size_t i = 1; i < numThreads; i++)
    if(threads[i].present && threads[i].sleepLeft <= 0) {
      targetThreadIndex = i;
      targetThread = &threads[i];
      break;
    }

  if(targetThreadIndex < 0)
    return -1;

  for(size_t i = targetThreadIndex; i < numThreads; i++)
    if(threads[i].requestAge > targetThread->requestAge &&
       threads[i].present && !threads[i].active &&
       threads[i].sleepLeft <= 0) {
      targetThread = &threads[i];
      targetThreadIndex = i;
    }

  for(size_t i = 1; i < numThreads; i++) {
    if(threads[i].present)
      threads[i].requestAge++;
    threads[i].active = 0;
  }
  threads[targetThreadIndex].requestAge = 0;
  threads[targetThreadIndex].active = 1;

  memcpy(output, targetThread, sizeof(idt_stackFrame));
  return targetThreadIndex;
}

