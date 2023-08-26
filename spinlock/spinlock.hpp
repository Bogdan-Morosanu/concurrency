#pragma once

#include <atomic>

namespace cnc {

  namespace internal {
    void pause()
    {
#ifdef __arm__
      asm volatile("yield");
#else
      __builtin_ia32_pause();
#endif
    } 
  }
  
  class SpinLock {
  public:

    SpinLock() : is_locked_(false) { }
    
    void lock() noexcept
    {
      while (true) {
	if (!is_locked_.exchange(true, std::memory_order_acquire)) {
	  return;
	}

	while (is_locked_.load(std::memory_order_relaxed)) {
	  internal::pause();
	}
      }
    }

    void unlock() noexcept
    {
      is_locked_.store(false, std::memory_order_release);
    }
    
  private:
    std::atomic<bool> is_locked_;
  };
}
