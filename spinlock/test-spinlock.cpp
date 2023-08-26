#include "spinlock.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

void print_help_and_exit(const char *program_name)
{
    std::fprintf(stderr, "usage %s [-m|-s]\n", program_name);
    std::fprintf(stderr, "\t-m : use std::mutex\n\t-s : use spinlock\n\tdefault : spinlock\n");
    std::exit(EXIT_FAILURE);
}

template <typename Lock>
void thread_work(int &target, unsigned times_increment, Lock &lock)
{
  while (times_increment-- > 0) {
    std::lock_guard<Lock> lk{lock};
    target++;
  }
}

template <typename Lock>
void launch_threads_and_wait(int &target, unsigned times_increment,
			     unsigned thread_num, Lock &lock)
{
  std::vector<std::thread> threads;
  threads.reserve(thread_num);

  for ( ;  thread_num > 0; thread_num--) {
    threads.emplace_back([&]()
			 {
			   thread_work(target, times_increment, lock);
			 });
  }

  for (auto &th: threads) {
    th.join();
  }
}

int main(int argc, char **argv)
{
  if (argc > 2) {
    print_help_and_exit(*argv);
  }

  bool use_spinlock = true;
  if (argc == 2 && (0 == std::strcmp(argv[1], "-m"))) {
    use_spinlock = false;
  } else if (argc == 2 && (0 != std::strcmp(argv[1], "-s"))) {
    print_help_and_exit(*argv);
  }

  int sum = 0;
  unsigned times_increment = 10000000;
  unsigned thread_num = 2;
  if (use_spinlock) {
    cnc::SpinLock lock;
    launch_threads_and_wait(sum, times_increment, thread_num, lock);

  } else {
    std::mutex lock;
    launch_threads_and_wait(sum, times_increment, thread_num, lock);
  }

  std::printf("final sum %d\n", sum);
  return 0;
}
