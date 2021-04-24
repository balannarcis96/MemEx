#pragma once
/**
 * @file TObjectPool.h
 *
 * @brief TObjectPool: Ring based thread safe object pool
			bUseSpinLock:
				[true] : SpinLock is used for synchronization [default]
				[false]: Atomic operations are used for synchronization
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

namespace MemEx {
	template<typename T, size_t PoolSize, bool bUseSpinLock = true>
	class TObjectPool {
	public:
		struct PoolTraits {
			static const size_t MyPoolSize = PoolSize;
			static const size_t MyPoolMask = PoolSize - 1;

			using MyPoolType = T;
			using MyType = TObjectPool<T, PoolSize>;

			static_assert((MyPoolSize& MyPoolMask) == 0, "TObjectPool size must be a power of 2");

#ifdef MEMEX_STATISTICS
			static inline std::atomic<size_t> TotalAllocations{ 0 };
			static inline std::atomic<size_t> TotalDeallocations{ 0 };

			static inline std::atomic<size_t> TotalOSAllocations{ 0 };
			static inline std::atomic<size_t> TotalOSDeallocations{ 0 };
#endif
		};

		//Preallocate and fill the whole Pool with [PoolSize] elements
		static bool Preallocate() noexcept {
			// ! Hopefully GAllocate will allocate in a continuous fashion.

			for (size_t i = 0; i < PoolSize; i++)
			{
				Pool[i] = GAllocate(sizeof(T), ALIGNMENT);
				if (Pool[i] == nullptr) {
					return false;
				}
			}

			return true;
		}

		//Allocate raw ptr T
		template<typename ...Types>
		static T* NewRaw(Types... Args) noexcept {
			return Allocate(std::forward<Types...>(Args)...);
		}

		//Deallocate T
		static void Deallocate(T* Obj) noexcept {

			if constexpr (std::is_destructible_v<T>) {
				//Call destructor manually
				Obj->~T();
			}

			ptr_t PrevVal{ nullptr };

			if constexpr (bUseSpinLock) {
				{ //Critical section
					SpinLockScopeGuard Guard(&SpinLock);

					const uint64_t InsPos = TailPosition++;

					PrevVal = Pool[InsPos & PoolTraits::MyPoolMask];
					Pool[InsPos & PoolTraits::MyPoolMask] = (ptr_t)Obj;
				}
			}
			else {
				uint64_t InsPos = (uint64_t)InterlockedIncrement64((volatile long long*)(&TailPosition));
				InsPos--;

				PrevVal = InterlockedExchangePointer(
					reinterpret_cast<volatile ptr_t*>(&Pool[InsPos & PoolTraits::MyPoolMask]),
					reinterpret_cast<ptr_t>(Obj)
				);
			}

			if (PrevVal)
			{
				GFree(PrevVal);

#ifdef MEMEX_STATISTICS
				PoolTraits::TotalOSDeallocations++;
#endif

				return;
			}

#ifdef MEMEX_STATISTICS
			PoolTraits::TotalDeallocations++;
#endif
		}

		//Get GUID of this Pool instance
		static size_t GetPoolId() {
			return (size_t)(&PoolTraits::MyType::Preallocate);
		}

#ifdef MEMEX_STATISTICS
		static size_t GetTotalOSDeallocations() {
			return PoolTraits::TotalOSDeallocations;
		}

		static size_t GetTotalOSAllocations() {
			return PoolTraits::TotalOSAllocations;
		}

		static size_t GetTotalDeallocations() {
			return PoolTraits::TotalDeallocations;
		}

		static size_t GetTotalAllocations() {
			return PoolTraits::TotalAllocations;
		}
#endif

	private:
		template<typename ...Types>
		static T* Allocate(Types... Args) noexcept {
			T* Allocated{ nullptr };

			if constexpr (bUseSpinLock) {
				{ //Critical section
					SpinLockScopeGuard Guard(&SpinLock);

					const uint64_t PopPos = HeadPosition++;

					Allocated = reinterpret_cast<T*>(Pool[PopPos & PoolTraits::MyPoolMask]);
					Pool[PopPos & PoolTraits::MyPoolMask] = nullptr;
				}
			}
			else {
				uint64_t PopPos = (uint64_t)InterlockedIncrement64(reinterpret_cast<volatile long long*>(&HeadPosition));
				PopPos--;

				Allocated = reinterpret_cast<T*>(InterlockedExchangePointer(
					reinterpret_cast<volatile ptr_t*>(&Pool[PopPos & PoolTraits::MyPoolMask]),
					nullptr
				));
			}

			if (!Allocated) {
				Allocated = (T*)GAllocate(sizeof(T), ALIGNMENT);
				if (!Allocated) {
					return nullptr;
				}

#ifdef MEMEX_STATISTICS
				PoolTraits::TotalOSAllocations++;
#endif
			}

			if constexpr (sizeof...(Types) == 0) {
				if constexpr (std::is_default_constructible_v<T>) {
					//Call default constructor manually
					new (Allocated) T();
				}
			}
			else {
				//Call constructor manually
				new (Allocated) T(std::forward<Types...>(Args)...);
			}

#ifdef MEMEX_STATISTICS
			PoolTraits::TotalAllocations++;
#endif

			return Allocated;
		}

		static	inline ptr_t 				Pool[PoolSize]{ 0 };
		static	inline uint64_t  			HeadPosition{ 0 };
		static	inline uint64_t  			TailPosition{ 0 };
		static	inline SpinLock				SpinLock{ };
	};
}