#pragma once

// Standard libs
#include <atomic>
#include <cstdint>
#include <type_traits>
#include <intrin.h>

#define MEMEX_STATISTICS 1

#ifndef ALIGNMENT
#define ALIGNMENT alignof(size_t)
#endif

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

//Empty macro used to keep Visual Studio from indenting pointer members
#define PTR

namespace MemEx {
	// Void * pointer type.
	using ptr_t = void*;

	using ulong_t = unsigned long;

	using long_t = long;

	// Global allocate block of memory
	extern ptr_t GAllocate(size_t BlockSize, size_t BlockAlignment) noexcept;

	// Global deallocate block of memory
	extern void GFree(ptr_t BlockPtr) noexcept;

	class SpinLock {
	public:
		void Lock() {
#if __has_builtin(__builtin_ia32_pause)
			for (;;) {
				if (!bLock.exchange(true, std::memory_order_acquire)) {
					break;
				}
				while (bLock.load(std::memory_order_relaxed)) {
					__builtin_ia32_pause();
				}
			}
#else
			for (;;) {
				if (!bLock.exchange(true, std::memory_order_acquire)) {
					break;
				}
				while (bLock.load(std::memory_order_relaxed)) {
					std::atomic_signal_fence(std::memory_order_seq_cst);
					_mm_pause();
				}
			}
#endif
		}

		void Unlock() { bLock.store(false, std::memory_order_release); }
	private:
		std::atomic<bool> bLock = { false };
	};

	class SpinLockScopeGuard {

		FORCEINLINE explicit SpinLockScopeGuard(SpinLock* Lock) noexcept
			:Lock(Lock)
		{
			Lock->Lock();
		}

		FORCEINLINE ~SpinLockScopeGuard() noexcept 
		{
			Lock->Unlock();
		}

	private:
		SpinLock* Lock;
	};

}
