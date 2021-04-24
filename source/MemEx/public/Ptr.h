#pragma once
/**
 * @file Ptr.h
 *
 * @brief MemEx Smart Pointers
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

namespace MemEx {
#pragma region _TPtr [Base of all TPtr]

	template<typename T>
	class _TPtrBase {
	public:
		FORCEINLINE T& operator*() noexcept
		{
			return *Ptr;
		}
		FORCEINLINE const T& operator*() const noexcept
		{
			return *Ptr;
		}

		FORCEINLINE T* operator->() noexcept
		{
			return Ptr;
		}
		FORCEINLINE const T* operator->() const noexcept
		{
			return Ptr;
		}

		FORCEINLINE T& operator[](size_t Index) noexcept {
			return Ptr[Index];
		}
		FORCEINLINE const T& operator[](size_t Index) const noexcept {
			return Ptr[Index];
		}

		T* Get() noexcept
		{
			return Ptr;
		}
		const T* Get() const noexcept
		{
			return Ptr;
		}

		explicit operator bool() const noexcept
		{
			return Ptr != nullptr;
		}
		FORCEINLINE bool IsNull() const noexcept
		{
			return Ptr == nullptr;
		}

	protected:
		_TPtrBase() : Ptr(nullptr) {}
		_TPtrBase(T* Ptr) : Ptr(Ptr) {}

		mutable T* PTR	Ptr{ nullptr };
	};

	template<typename T, typename Base>
	class _TPtr : public Base {
		_TPtr() :Base() {}
		_TPtr(T* Ptr) :Base(Ptr) {}

		//Move
		_TPtr(_TPtr&& Other) noexcept : Base() {
			Reset(Other.Release());
		};
		_TPtr& operator=(_TPtr&& Other) noexcept {
			if (this == &Other) {
				return *this;
			}

			Reset(Other.Release());

			return *this;
		};

		//Cant copy
		_TPtr(_TPtr&) = delete;
		_TPtr& operator=(_TPtr&) = delete;

		//Cast
		template<typename TargetType>
		FORCEINLINE _TPtr<TargetType, Base> Cast() {
			return _TPtr<TargetType, Base>((TargetType*)Release());
		}

		~_TPtr() {
			this->DestroyResource();
		}

		FORCEINLINE void Reset(T* Resource = nullptr) {
			this->DestroyResource();
			this->Ptr = Resource;
		}
		FORCEINLINE T* Release() {
			T* Temp = this->Ptr;
			this->Ptr = nullptr;
			return Temp;
		}

		friend struct MemoryManager;
		template<typename T, typename BasePtr>
		friend struct _MPtr;
	};

	template<typename T, typename Base>
	class _TSharedPtr : public Base {
	public:
		_TSharedPtr() :Base() {}
		_TSharedPtr(T* Ptr) : Base(Ptr) {}

		//Move
		_TSharedPtr(_TSharedPtr&& Other) noexcept {
			this->Ptr = Other.Ptr;
			Other.Ptr = nullptr;
		};
		_TSharedPtr& operator=(_TSharedPtr&& Other)noexcept {
			if (this == &Other) {
				return *this;
			}

			this->Ptr = Other.Ptr;
			Other.Ptr = nullptr;

			return *this;
		};

		//Copy
		_TSharedPtr(const _TSharedPtr& Other) {
			Other.IncRef();
			this->Ptr = Other.Ptr;
		};
		_TSharedPtr& operator=(const _TSharedPtr& Other) {
			if (this == &Other) {
				return *this;
			}

			Other.AddReference();
			this->Ptr = Other.Ptr;

			return *this;
		};

		~_TSharedPtr() {
			ReleaseReference();
		}

		void Release() noexcept {
			ReleaseReference();
		}

	private:
		FORCEINLINE bool AddReference() const noexcept { // increment ref count if not zero, return true if successful
			if (this->IsNull()) { return false; }

			return this->Ptr->AddReference();
		}
		FORCEINLINE void ReleaseReference() const noexcept {
			if (this->IsNull()) { return; }

			if (this->Ptr->ReleaseReference()) {
				this->DestroyResource();
			}
		}

		template<typename K, size_t PoolSize>
		friend struct TGlobalMemoryStore;
		template<typename T, typename BasePtr>
		friend struct _MPtr;
		friend struct MemoryManager;
	};

#pragma endregion

#pragma region _MPtr [Base of all MPtr]

	template<typename T>
	class MemoryResourcePtrBase : public _TPtrBase<T> {
	public:
		MemoryResourcePtrBase() noexcept : _TPtrBase<T>(nullptr) {}
		MemoryResourcePtrBase(T* Ptr) noexcept : _TPtrBase<T>(Ptr) {}

		FORCEINLINE void DestroyResource() const noexcept
		{
			if (this->Ptr)
			{
				this->Ptr->Destroy(this->Ptr, true);
				this->Ptr = nullptr;
			}
		}
	};

	using TIMemoryBlockPtrBase = MemoryResourcePtrBase<IMemoryBlock>;

	//Memory Block pointer abstraction
	//It consits of 2 pointers, one to the MemoryBlock, and the other is an aligned pointer into the MemoryBlock, of type T
	template<typename T, typename MyBlockPtr>
	class _MPtr : public _TPtrBase<T> {
		static_assert(
			std::is_same_v<_TPtr<IMemoryBlock, TIMemoryBlockPtrBase>, MyBlockPtr> ||
			std::is_same_v<_TSharedPtr<IMemoryBlock, TIMemoryBlockPtrBase>, MyBlockPtr>,
			"See _MPtr<T, MyBlockPtr>");
	public:
		using MyType = _MPtr<T, MyBlockPtr>;

		_MPtr() {}
		_MPtr(IMemoryBlock* BlockObject, T* Ptr) :_TPtrBase<T>(Ptr), BlockObject(BlockObject) {}
		_MPtr(MyBlockPtr&& BlockObject, T* Ptr) :_TPtrBase<T>(Ptr), BlockObject(std::move(BlockObject)) {}

		//Cant copy
		_MPtr(const _MPtr&) = delete;
		_MPtr& operator=(const _MPtr&) = delete;

		//Move
		FORCEINLINE _MPtr(_MPtr&& Other) noexcept : BlockObject(std::move(Other.BlockObject)) {
			auto Temp = Other.Ptr;
			Other.Ptr = nullptr;
			this->Ptr = Temp;
		}
		FORCEINLINE _MPtr& operator=(_MPtr&& Other) noexcept {
			if (this == &Other) {
				return *this;
			}

			BlockObject = std::move(Other.BlockObject);

			auto Temp = Other.Ptr;
			Other.Ptr = nullptr;
			this->Ptr = Temp;

			return *this;
		}

		FORCEINLINE size_t GetCapacity() const noexcept {
			if (this->IsNull() || BlockObject.IsNull()) { return 0; }

			const auto Ptr = reinterpret_cast<uint8_t*>(this->Ptr);

			return static_cast<size_t>(BlockObject->GetEnd() - BlockObject->GetBegin(static_cast<ulong_t>(Ptr - BlockObject->Block)));
		}

		FORCEINLINE void Reset() noexcept {
			this->Ptr = nullptr;
			BlockObject.Reset();
		}

		FORCEINLINE IMemoryBlock* GetMemoryBlock() noexcept {
			return this->BlockObject.Ptr;
		}

		FORCEINLINE const IMemoryBlock* GetMemoryBlock() const noexcept {
			return this->BlockObject.Ptr;
		}

	protected:
		MyBlockPtr BlockObject{ nullptr };

		friend struct MemoryManager;
		template <typename T>
		friend struct TStructureBase;
	};

	using IMemoryBlockPtr = _TPtr<IMemoryBlock, TIMemoryBlockPtrBase>;
	using IMemoryBlockSharedPtr = _TSharedPtr<IMemoryBlock, TIMemoryBlockPtrBase>;

	//MemoryBlock unique pointer
	template<typename T>
	using MPtr = _MPtr<T, IMemoryBlockPtr>;

	//MemoryBlock shared pointer
	template<typename T>
	using MSharedPtr = _MPtr<T, IMemoryBlockSharedPtr>;

#pragma endregion
}