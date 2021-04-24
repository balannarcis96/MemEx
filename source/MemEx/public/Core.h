#pragma once
/**
 * @file Core.h
 *
 * @brief MemEx core includes and abstractions
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

// Standard libs
#include <atomic>
#include <cstdint>
#include <type_traits>
#include <intrin.h>
#include <memory>

#define MEMEX_STATISTICS

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

	//Efficient implentation of a spin lock
	class SpinLock {
	public:
		FORCEINLINE void Lock() noexcept
		{
			for (;;) {
				if (!bLock.exchange(true, std::memory_order_acquire)) {
					break;
				}
				while (bLock.load(std::memory_order_relaxed)) {
					std::atomic_signal_fence(std::memory_order_seq_cst);
					_mm_pause();
				}
			}
		}

		FORCEINLINE void Unlock() noexcept
		{
			bLock.store(false, std::memory_order_release);
		}

	private:
		std::atomic<bool> bLock = { false };
	};

	//RAII-based spin lock guard (scope guard)
	class SpinLockScopeGuard {
	public:
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
