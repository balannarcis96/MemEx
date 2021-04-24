#pragma once
/**
 * @file Memory.h
 *
 * @brief MemEx base abstractions
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

namespace MemEx {
	class MemoryResourceBase;

	//typedef void (*MemoryBlockDestroyCallback)(MemoryResourceBase*);
	using MemoryBlockDestroyCallback = Delegate<void, ptr_t, bool>;

	class MemoryResourceBase {
	protected:

		//We store the "controll block" inside the resource's memory space
		mutable uint32_t RefCount{ 1 };

		//Block flags
		union {

			struct {
				unsigned bDontDestruct : 1;
			};

			uint32_t MemoryResourceFlags{ 0 };
		};

		//Destroy callback (deleter)
		MemoryBlockDestroyCallback	Destroy{  };

		template<typename T>
		friend class MemoryResourcePtrBase;
		friend class MemoryManager;
	};

	template<bool bAtomicRef = true>
	class MemoryResource : public MemoryResourceBase {
		FORCEINLINE void AddReference() const noexcept { 
			if constexpr (bAtomicRef) {
				auto& VolatileRefCount = reinterpret_cast<volatile long&>(this->RefCount);

				long RefCount = __iso_volatile_load32(reinterpret_cast<volatile int*>(&VolatileRefCount));
				while (RefCount != 0) {
					const long OldValue = _InterlockedCompareExchange(&VolatileRefCount, RefCount + 1, RefCount);
					if (OldValue == RefCount) {
						return;
					}

					RefCount = OldValue;
				}
			}
			else {
				RefCount++;
			}
		}

		FORCEINLINE bool ReleaseReference() const noexcept {
			if constexpr (bAtomicRef) {
				if (_InterlockedDecrement(reinterpret_cast<volatile long*>(&this->RefCount)) == 0) {
					return true;
				}
			}
			else {
				RefCount--;

				if (RefCount == 0)
				{
					return true;
				}
			}

			return false;
		}

		template<typename T, typename Base>
		friend class _TSharedPtr;
		friend class MemoryManager;
	};

	class MemoryBlockBase : public MemoryResource<true> {
		using Base = MemoryResource<true>;

	public:
		ulong_t					const	BlockSize{ 0 };
		ulong_t					const	ElementSize{ 0 };
		ulong_t					const	ElementsCount{ 1 };
		uint8_t* PTR					Block{ nullptr };

		MemoryBlockBase(ulong_t BlockSize, uint8_t* Block, ulong_t ElementSize) noexcept
			:Base()
			, BlockSize(BlockSize)
			, ElementSize(ElementSize)
			, Block(Block)
		{}

		MemoryBlockBase(ulong_t BlockSize, uint8_t* Block, ulong_t ElementSize, ulong_t ElementsCount) noexcept
			:Base()
			, BlockSize(BlockSize)
			, ElementSize(ElementSize)
			, ElementsCount(ElementsCount)
			, Block(Block)
		{}

		//Cant copy
		MemoryBlockBase(const MemoryBlockBase&) = delete;
		MemoryBlockBase& operator=(const MemoryBlockBase&) = delete;

		//Cant move
		MemoryBlockBase(MemoryBlockBase&&) = delete;
		MemoryBlockBase& operator=(MemoryBlockBase&& Other) = delete;

		FORCEINLINE const uint8_t* CanFit(ulong_t Length, ulong_t StartOffset = 0) const noexcept {
			if (BlockSize < (Length + StartOffset)) {
				return nullptr;
			}

			return Block + StartOffset;
		}

		FORCEINLINE const uint8_t* GetBegin(ulong_t StartOffset = 0) const noexcept {
			return Block + StartOffset;
		}

		FORCEINLINE const uint8_t* GetEnd() const noexcept {
			return Block + BlockSize;
		}

		FORCEINLINE void ZeroBlockMemory() noexcept {
			memset(
				Block,
				0,
				BlockSize
			);
		}
	};

	using IMemoryBlock = MemoryBlockBase;

	template<ulong_t Size>
	class MemoryBlock : public IMemoryBlock {
		static_assert(Size% ALIGNMENT == 0, "Size of MemoryBlock<Size> must be a multiple of ALIGNMENT");

	public:
		uint8_t		FixedSizeBlock[Size];

		MemoryBlock(ulong_t ElementSize) noexcept
			: IMemoryBlock(Size, FixedSizeBlock, ElementSize)
		{}

		MemoryBlock(ulong_t ElementSize, ulong_t ElementsCount) noexcept
			: IMemoryBlock(Size, FixedSizeBlock, ElementSize, ElementsCount)
		{}
	};

	class CustomBlock : public IMemoryBlock {
	public:
		CustomBlock(ulong_t Size, ulong_t ElementSize) noexcept
			: IMemoryBlock(Size, nullptr, ElementSize)
		{
			Block = (uint8_t*)GAllocate(sizeof(uint8_t) * Size, ALIGNMENT);
		}

		CustomBlock(ulong_t Size, ulong_t ElementSize, ulong_t ElementsCount) noexcept
			: IMemoryBlock(Size, nullptr, ElementSize, ElementsCount)
		{
			Block = (uint8_t*)GAllocate(sizeof(uint8_t) * Size, ALIGNMENT);
		}
	};

	class CustomBlockHeader : public IMemoryBlock {
	public:
		CustomBlockHeader(ulong_t Size, ulong_t ElementSize)
			: IMemoryBlock(Size, nullptr, ElementSize)
		{
			Block = (reinterpret_cast<uint8_t*>(this) + sizeof(CustomBlockHeader));
		}

		CustomBlockHeader(ulong_t Size, ulong_t ElementSize, ulong_t ElementsCount)
			: IMemoryBlock(Size, nullptr, ElementSize, ElementsCount)
		{
			Block = (reinterpret_cast<uint8_t*>(this) + sizeof(CustomBlockHeader));
		}
	};

}

