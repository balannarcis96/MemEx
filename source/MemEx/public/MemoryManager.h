#pragma once
/**
 * @file MemoryManager.h
 *
 * @brief MemEx MemoryManager implementation
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

namespace MemEx {
	/*------------------------------------------------------------
		Memory Manager
	  ------------------------------------------------------------*/
	struct MemoryManager {
		template<typename T, size_t PoolSize>
		using TObjectStore = TObjectPool<T, PoolSize>;

		struct SmallBlock : MemoryBlock<SmallMemBlockSize>, TObjectStore<SmallBlock, SmallMemBlockCount> {
			SmallBlock(ulong_t ElementSize) noexcept
				: MemoryBlock(ElementSize)
			{}

			SmallBlock(ulong_t ElementSize, ulong_t ElementsCount) noexcept
				: MemoryBlock(ElementSize, ElementsCount)
			{}
		};
		struct MediumBlock : MemoryBlock<MediumMemBlockSize>, TObjectStore<MediumBlock, MediumMemBlockCount> {
			MediumBlock(ulong_t ElementSize) noexcept
				: MemoryBlock(ElementSize)
			{}

			MediumBlock(ulong_t ElementSize, ulong_t ElementsCount) noexcept
				: MemoryBlock(ElementSize, ElementsCount)
			{}
		};
		struct LargeBlock : MemoryBlock<LargeMemBlockSize>, TObjectStore<LargeBlock, LargeMemBlockCount> {
			LargeBlock(ulong_t ElementSize) noexcept
				: MemoryBlock(ElementSize)
			{}

			LargeBlock(ulong_t ElementSize, ulong_t ElementsCount) noexcept
				: MemoryBlock(ElementSize, ElementsCount)
			{}
		};
		struct ExtraLargeBlock : MemoryBlock<ExtraLargeMemBlockSize>, TObjectStore<ExtraLargeBlock, ExtraLargeMemBlockCount> {
			ExtraLargeBlock(ulong_t ElementSize) noexcept
				: MemoryBlock(ElementSize)
			{}

			ExtraLargeBlock(ulong_t ElementSize, ulong_t ElementsCount) noexcept
				: MemoryBlock(ElementSize, ElementsCount)
			{}
		};

		static int Initialize() noexcept {
			if (!SmallBlock::Preallocate()) {
				return 1;
			}
			if (!MediumBlock::Preallocate()) {
				return 2;
			}
			if (!LargeBlock::Preallocate()) {
				return 3;
			}
			if (!ExtraLargeBlock::Preallocate()) {
				return 4;
			}

			return 0;
		}
		static bool Shutdown() noexcept {
			return true;
		}

#ifdef MEMEX_STATISTICS
		static inline std::atomic<size_t> CustomSizeAllocations{ 0 };
		static inline std::atomic<size_t> CustomSizeDeallocations{ 0 };

		static void PrintStatistics() {
			printf("MemoryManager ###############################################################\n");
			printf("\n\tSmallBlock:\n\t\tAllocations:%lld\n\t\tDeallocations:%lld\n\t\tOSAllocations:%lld\n\t\tOSDeallocations:%lld",
				SmallBlock::GetTotalAllocations(),
				SmallBlock::GetTotalDeallocations(),
				SmallBlock::GetTotalOSAllocations(),
				SmallBlock::GetTotalOSDeallocations()
			);
			printf("\n\tMediumBlock:\n\t\tAllocations:%lld\n\t\tDeallocations:%lld\n\t\tOSAllocations:%lld\n\t\tOSDeallocations:%lld",
				MediumBlock::GetTotalAllocations(),
				MediumBlock::GetTotalDeallocations(),
				MediumBlock::GetTotalOSAllocations(),
				MediumBlock::GetTotalOSDeallocations()
			);
			printf("\n\tLargeBlock:\n\t\tAllocations:%lld\n\t\tDeallocations:%lld\n\t\tOSAllocations:%lld\n\t\tOSDeallocations:%lld",
				LargeBlock::GetTotalAllocations(),
				LargeBlock::GetTotalDeallocations(),
				LargeBlock::GetTotalOSAllocations(),
				LargeBlock::GetTotalOSDeallocations()
			);
			printf("\n\tExtraLargeBlock:\n\t\tAllocations:%lld\n\t\tDeallocations:%lld\n\t\tOSAllocations:%lld\n\t\tOSDeallocations:%lld",
				ExtraLargeBlock::GetTotalAllocations(),
				ExtraLargeBlock::GetTotalDeallocations(),
				ExtraLargeBlock::GetTotalOSAllocations(),
				ExtraLargeBlock::GetTotalOSDeallocations()
			);
			printf("\n\tCustomSize(OS Blocks):\n\t\tAllocations:%lld\n\t\tDeallocations:%lld",
				CustomSizeAllocations.load(),
				CustomSizeDeallocations.load()
			);
			printf("\n\tTotal Allocation:%lld\n\tTotal Deallocations:%lld\n\tTotal OSAllocations:%lld\n\tTotal OSDeallocations:%lld",
				SmallBlock::GetTotalAllocations() + MediumBlock::GetTotalAllocations() + LargeBlock::GetTotalAllocations() + ExtraLargeBlock::GetTotalAllocations() + CustomSizeAllocations.load(),
				SmallBlock::GetTotalDeallocations() + MediumBlock::GetTotalDeallocations() + LargeBlock::GetTotalDeallocations() + ExtraLargeBlock::GetTotalDeallocations() + CustomSizeDeallocations.load(),
				SmallBlock::GetTotalOSAllocations() + MediumBlock::GetTotalOSAllocations() + LargeBlock::GetTotalOSAllocations() + ExtraLargeBlock::GetTotalOSAllocations(),
				SmallBlock::GetTotalOSDeallocations() + MediumBlock::GetTotalOSDeallocations() + LargeBlock::GetTotalOSDeallocations() + ExtraLargeBlock::GetTotalOSDeallocations()
			);
			printf("\nMemoryManager ###############################################################\n");
		}
#endif

#undef MEMORY_MANAGER_CALL_DESTRUCTOR
#define MEMORY_MANAGER_CALL_DESTRUCTOR																		\
			IMemoryBlock* NewBlockObject = (IMemoryBlock*)Object; 											\
			if constexpr (std::is_destructible_v<T>) {														\
				if(bCallDestructor && !NewBlockObject->bDontDestruct)	{									\
					auto Ptr = reinterpret_cast<ptr_t>(NewBlockObject->Block);								\
																											\
					size_t Space = sizeof(T) + alignof(T);													\
					std::align(alignof(T), sizeof(T), Ptr, Space);											\
																											\
					/*call destructor*/																		\
					reinterpret_cast<T*>(Ptr)->~T();														\
				}																							\
			}

#undef MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER
#define MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER																\
			IMemoryBlock* NewBlockObject = (IMemoryBlock*)Object; 											\
			if constexpr (std::is_destructible_v<T>) {														\
				if(bCallDestructor && !NewBlockObject->bDontDestruct)	{									\
					auto Ptr = reinterpret_cast<ptr_t>(NewBlockObject->Block);								\
																											\
					size_t Space = NewBlockObject->BlockSize;												\
					std::align(alignof(T), sizeof(T), Ptr, Space);											\
																											\
					for(size_t i = 0; i < NewBlockObject->ElementsCount; i ++) {							\
						/*call destructor*/																	\
						T* IPtr = reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(Ptr) + (sizeof(T) * i));	\
						/*call destructor*/																	\
						IPtr->~T();																			\
					}																						\
				}																							\
			}

#pragma region Compiletime

		template<typename T, typename ...Types>
		inline static MSharedPtr<T> AllocShared(Types... Args) noexcept {
			if constexpr (std::is_reference_v<T>) {
				static_assert(false, "Alloc<T> Cant allocate T reference!");
			}

			if constexpr (std::is_array_v<T>) {
				static_assert(false, "Use AllocBuffer(Count) to allocate arrays!");
			}

			constexpr size_t Size = sizeof(T) + alignof(T);

			MemoryBlockBase* NewBlockObject = MemoryManager::AllocBlock<T>();
			if (!NewBlockObject) {
				return { nullptr , nullptr };
			}

			ptr_t Ptr = NewBlockObject->Block;

			//Align pointer
			size_t Space = Size;
			if (!std::align(alignof(T), sizeof(T), Ptr, Space)) {
				NewBlockObject->Destroy((ptr_t)NewBlockObject, false);
				//LogFatal("MemoryManager::Alloc(...) Failed to std::align({}, {}, ptr, {})!", alignof(T), sizeof(T), Size);
				return { nullptr , nullptr };
			}

			if constexpr (sizeof...(Types) == 0) {
				if constexpr (std::is_default_constructible_v<std::decay<T>::type>) {

					//Call default constructor manually
					new (Ptr) T();
				}
			}
			else {
				//Call constructor manually
				new (Ptr) T(std::forward<Types...>(Args)...);
			}

			return { NewBlockObject, reinterpret_cast<T*>(Ptr) };
		}

		template<typename T, typename ...Types>
		inline static MPtr<T> Alloc(Types... Args) noexcept {
			if constexpr (std::is_reference_v<T>) {
				static_assert(false, "Alloc<T> Cant allocate T reference!");
			}

			if constexpr (std::is_array_v<T>) {
				static_assert(false, "Use AllocBuffer(Count) to allocate arrays!");
			}

			constexpr size_t Size = sizeof(T) + alignof(T);

			MemoryBlockBase* NewBlockObject = AllocBlock<T>();
			if (!NewBlockObject) {
				return { nullptr , nullptr };
			}

			ptr_t Ptr = NewBlockObject->Block;

			//Align pointer
			size_t Space = Size;
			if (!std::align(alignof(T), sizeof(T), Ptr, Space)) {
				NewBlockObject->Destroy((ptr_t)NewBlockObject, false);
				//LogFatal("MemoryManager::Alloc(...) Failed to std::align({}, {}, ptr, {})!", alignof(T), sizeof(T), Size);
				return { nullptr , nullptr };
			}

			if constexpr (sizeof...(Types) == 0) {
				if constexpr (std::is_default_constructible_v<std::decay<T>::type>) {

					//Call default constructor manually
					new (Ptr) T();
				}
			}
			else {
				//Call constructor manually
				new (Ptr) T(std::forward<Types...>(Args)...);
			}

			return { NewBlockObject, reinterpret_cast<T*>(Ptr) };
		}

		template<typename T>
		static IMemoryBlock* AllocBlock() noexcept {
			constexpr size_t Size = sizeof(T) + alignof(T);

			IMemoryBlock* NewBlockObject = nullptr;

			if constexpr (Size <= SmallMemBlockSize)
			{
				NewBlockObject = SmallBlock::NewRaw((ulong_t)Size);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() SmallBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true)  -> void {
					MEMORY_MANAGER_CALL_DESTRUCTOR;
					SmallBlock::Deallocate(reinterpret_cast<SmallBlock*>(NewBlockObject));
				};
			}
			else if constexpr (Size <= MediumMemBlockSize)
			{
				NewBlockObject = MediumBlock::NewRaw((ulong_t)Size);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() MediumBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR;
					MediumBlock::Deallocate(reinterpret_cast<MediumBlock*>(NewBlockObject));
				};
			}
			else if constexpr (Size <= LargeMemBlockSize)
			{
				NewBlockObject = LargeBlock::NewRaw((ulong_t)Size);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() LargeBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR;
					LargeBlock::Deallocate(reinterpret_cast<LargeBlock*>(NewBlockObject));
				};
			}
			else if constexpr (Size <= ExtraLargeMemBlockSize)
			{
				NewBlockObject = ExtraLargeBlock::NewRaw((ulong_t)Size);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() ExtraLargeBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR;
					ExtraLargeBlock::Deallocate(reinterpret_cast<ExtraLargeBlock*>(NewBlockObject));
				};
			}
			else {
				NewBlockObject = (IMemoryBlock*)GAllocate(sizeof(CustomBlockHeader) + Size, ALIGNMENT);
				if (NewBlockObject)
				{
					//Construct the CustomBlockHeader at the begining of the block
					new (reinterpret_cast<CustomBlockHeader*>(NewBlockObject)) CustomBlockHeader(Size, Size);

					//Set the destruction handler
					NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
						MEMORY_MANAGER_CALL_DESTRUCTOR;

#ifdef MEMORY_STATISTICS
						CustomSizeDeallocations++;
#endif
						GFree(NewBlockObject);
					};

#ifdef MEMORY_STATISTICS
					CustomSizeAllocations++;
#endif
				}
				else {
					//LogFatal("MemoryManager::Alloc() Failed to get memory from OS!");
					return nullptr;
				}
			}

			return NewBlockObject;
		}
#pragma endregion

#pragma region Runtime
		template<typename T>
		static IMemoryBlock* AllocBlock(size_t Count) noexcept {
			const size_t Size = (sizeof(T) * Count) + alignof(T);

			IMemoryBlock* NewBlockObject = nullptr;

			if (Size <= SmallMemBlockSize)
			{
				NewBlockObject = SmallBlock::NewRaw((ulong_t)sizeof(T), (ulong_t)Count);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() SmallBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER;
					SmallBlock::Deallocate(reinterpret_cast<SmallBlock*>(NewBlockObject));
				};
			}
			else if (Size <= MediumMemBlockSize)
			{
				NewBlockObject = MediumBlock::NewRaw((ulong_t)sizeof(T), (ulong_t)Count);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() MediumBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER;
					MediumBlock::Deallocate(reinterpret_cast<MediumBlock*>(NewBlockObject));
				};
			}
			else if (Size <= LargeMemBlockSize)
			{
				NewBlockObject = LargeBlock::NewRaw((ulong_t)sizeof(T), (ulong_t)Count);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() LargeBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER;
					LargeBlock::Deallocate(reinterpret_cast<LargeBlock*>(NewBlockObject));
				};
			}
			else if (Size <= ExtraLargeMemBlockSize)
			{
				NewBlockObject = ExtraLargeBlock::NewRaw((ulong_t)sizeof(T), (ulong_t)Count);
				if (!NewBlockObject) {
					//LogFatal("MemoryManager::Alloc() ExtraLargeBlock::NewRaw() Failed!");
					return nullptr;
				}

				NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
					MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER;
					ExtraLargeBlock::Deallocate(reinterpret_cast<ExtraLargeBlock*>(NewBlockObject));
				};
			}
			else {
				NewBlockObject = (IMemoryBlock*)GAllocate(sizeof(CustomBlockHeader) + Size, ALIGNMENT);
				if (NewBlockObject)
				{
					//Construct the CustomBlockHeader at the begining of the block
					new (reinterpret_cast<CustomBlockHeader*>(NewBlockObject)) CustomBlockHeader((ulong_t)Size, (ulong_t)sizeof(T), (ulong_t)Count);

					NewBlockObject->Destroy = [](ptr_t Object, bool bCallDestructor = true) {
						MEMORY_MANAGER_CALL_DESTRUCTOR_BUFFER;

#ifdef MEMORY_STATISTICS
						CustomSizeDeallocations++;
#endif
						GFree(NewBlockObject);
					};

#ifdef MEMORY_STATISTICS
					CustomSizeAllocations++;
#endif
				}
				else {
					//LogFatal("MemoryManager::Alloc() Failed to get memory from OS!");
					return nullptr;
				}
			}

			return NewBlockObject;
		}

		// Allocate T[Size] buffer
		// bDontConstructElements - if true the call to T default constructor (for each element) wont be made
		template<typename T, bool bDontConstructElements = false>
		static MPtr<T> AllocBuffer(const size_t Count) noexcept {
			if constexpr (std::is_array_v<T>) {
				static_assert(false, "Dont use AllocBuffer<T[]>(size) but use AllocBuffer<T>(size)!");
			}

			IMemoryBlock* NewBlockObject = AllocBlock<T>(Count);
			if (!NewBlockObject) {
				return { nullptr , nullptr };
			}

			//if we dont construct, we dont destruct 
			if constexpr (bDontConstructElements && std::is_destructible_v<T>) {
				NewBlockObject->bDontDestruct = true;
			}

			//Change to size in bytes
			const size_t Size = (sizeof(T) * Count) + alignof(T);

			ptr_t Ptr = reinterpret_cast<ptr_t>(NewBlockObject->Block);

			//Align pointer
			size_t Space = Size;
			if (!std::align(alignof(T), sizeof(T), Ptr, Space)) {
				if constexpr (!bDontConstructElements && std::is_destructible_v<T>) {
					NewBlockObject->Destroy((ptr_t)NewBlockObject, false);
				}

				//LogFatal("MemoryManager::AllocBuffer({}) Failed to std::align({}, {}, ptr, {})!", Count, alignof(T), sizeof(T), Size);
				return { nullptr , nullptr };
			}

			if constexpr (std::is_default_constructible_v<T> && !bDontConstructElements) {
				//Call default constructor manually for each object of the array
				for (size_t i = 0; i < Count; i++)
				{
					new (reinterpret_cast<uint8_t*>(Ptr) + (sizeof(T) * i)) T();
				}
			}

			return { NewBlockObject, reinterpret_cast<T*>(Ptr) };
		}

		template<typename T>
		static MSharedPtr<T> AllocSharedBuffer(size_t Count) noexcept {
			MPtr<T> Unique = AllocBuffer<T>(Count);
			if (Unique.IsNull()) {
				return { nullptr, nullptr };
			}

			T* Ptr = Unique.Get();
			return { Unique.BlockObject.Release(), Ptr };
		}
#pragma endregion
	};

	template<typename TUpper>
	struct IResource {
		template<typename ...Types>
		FORCEINLINE static MPtr<TUpper> New(Types... Args) noexcept {
			return MemoryManager::Alloc<TUpper>(std::forward<Types...>(Args)...);
		}

		template<typename ...Types>
		FORCEINLINE static MSharedPtr<TUpper> NewShared(Types... Args) noexcept {
			return MemoryManager::AllocShared<TUpper>(std::forward<Types...>(Args)...);
		}

		FORCEINLINE static MPtr<TUpper> NewArray(size_t Count) noexcept {
			return MemoryManager::AllocBuffer<TUpper>(Count);
		}

		FORCEINLINE static MSharedPtr<TUpper> NewSharedArray(size_t Count) noexcept {
			return MemoryManager::AllocSharedBuffer<TUpper>(Count);
		}
	};
}