#include <pthread.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

using namespace std;

const unsigned kKb100 = 100 * 1024;
const unsigned kKb50 = 50 * 1024;

void *MainOne(void *data) {
  for (unsigned i = 0; i < 100; ++i) {
    void *buf = malloc(kKb50);
    free(buf);
  }
  return NULL;
}

void *MainTwo(void *data) {
  for (unsigned i = 0; i < 100; ++i) {
    void *buf = malloc(kKb100);
    free(buf);
  }
  return NULL;
}

int main() {
  printf("Main thread, allocating %d kB\n", kKb100 / 1024);
  void *buf = malloc(kKb100);

  pthread_t thread1;
  pthread_t thread2;
  int retval;

  printf("Starting thread 1\n");
  retval = pthread_create(&thread1, NULL, MainOne, NULL);
  assert(retval == 0);
  printf("Starting thread 2\n");
  retval = pthread_create(&thread2, NULL, MainTwo, NULL);
  assert(retval == 0);

  pthread_join(thread1, NULL);
  printf("Thread 1 stopped\n");
  pthread_join(thread2, NULL);
  printf("Thread 2 stopped\n");

  free(buf);
  return 0;
}
