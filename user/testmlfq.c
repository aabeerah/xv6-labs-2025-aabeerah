#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// CPU bound process
void cpu_bound() {
  int i, j, k;
  volatile int x = 0;

  printf("CPU bound process [PID %d] starting\n", getpid());
  
  for(i = 0; i < 5; i++) {
    for(j = 0; j < 10000; j++) {
      for(k = 0; k < 10000; k++) {
        x = x + 1;
      }
    }
    
    struct procinfo info;
    if(getprocinfo(getpid(), &info) == 0) {
      printf("CPU [PID %d]: queue=%d, ticks=%d, quantum=%d\n", 
             info.pid, info.queue_level, info.ticks_used, info.quantum);
    }
  }
  
  printf("CPU bound process [PID %d] finished\n", getpid());
}

// I/O bound process
void io_bound() {
  int i;

  printf("I/O bound process [PID %d] starting\n", getpid());

  for(i = 0; i < 20; i++) {
    pause(2);
    
    // Check queue level
    struct procinfo info;
    if(getprocinfo(getpid(), &info) == 0) {
      printf("I/O [PID %d]: queue=%d, ticks=%d, quantum=%d\n", 
             info.pid, info.queue_level, info.ticks_used, info.quantum);
    }
  }
  
  printf("I/O-bound process [PID %d] finished\n", getpid());
}

int main(int argc, char *argv[]) {
  printf("=== MLFQ Scheduler Test ===\n");
  printf("Testing with CPU-bound and I/O-bound processes\n\n");
  
  int cpu_pid = fork();
  if(cpu_pid == 0) {
    // Child: CPU-bound
    cpu_bound();
    exit(0);
  }
  
  int io_pid = fork();
  if(io_pid == 0) {
    // Child: I/O-bound
    io_bound();
    exit(0);
  }
  
  // Parent waits for both
  wait(0);
  wait(0);

  printf("\nTest Complete\n");
  exit(0);
}
