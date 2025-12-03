#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  struct procinfo info;
  int pid = getpid();
  
  if(getprocinfo(pid, &info) == 0) {
    printf("Current process info:\n");
    printf("PID: %d\n", info.pid);
    printf("Queue Level: %d\n", info.queue_level);
    printf("Ticks Used: %d\n", info.ticks_used);
    printf("Quantum: %d\n", info.quantum);
    printf("Name: %s\n", info.name);
  } else {
    printf("getprocinfo failed\n");
  }
  
  exit(0);
}