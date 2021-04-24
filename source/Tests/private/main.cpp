#include <iostream>
#include <thread>

#include <MemEx.h>

//Global allocator implementation
namespace MemEx {
	ptr_t GAllocate(size_t BlockSize, size_t BlockAlignment) noexcept {
		return _aligned_malloc(BlockSize, BlockAlignment);
	}

	void GFree(ptr_t BlockPtr) noexcept {
		_aligned_free(BlockPtr);
	}
}

using namespace MemEx;

struct TypeA {
	int a{ 23 };
	double t{ 0.0 };

	TypeA() {
		std::cout << "TypeA()\n";
	}
	TypeA(double d) :t(d) {
		std::cout << "TypeA(d)\n";
	}
	~TypeA() {
		std::cout << "~TypeA()\n";
	}
};

struct TypeB: IResource<TypeB> {
	int a{ 23 };
	double t{ 0.0 };

	TypeB() {
		std::cout << "TypeB()\n";
	}
	TypeB(double d) :t(d) {
		std::cout << "TypeB(d)\n";
	}
	~TypeB() {
		std::cout << "~TypeB()\n";
	}
};

bool TestUniquePtr() {
	std::cout << "#TestUniquePtr():\n";
	{
		auto obj = MemoryManager::Alloc<TypeA>();

		if (obj->a != 23) {
			std::cout << "Alloc<TypeA> did not construct!";
			return false;
		}
		else {

			std::cout << "Obj.a = " << obj->a << "\n"; //

			obj->a = 55;

			std::cout << "Obj.a = " << obj->a << "\n"; //
		}
	}

	std::cout << "#TestUniquePtr():\n";

	return true;
}

bool TestSharedPtr(int argc) {
	std::cout << "#TestSharedPtr():\n";

	std::cout << "#TestSharedPtr():\n";

	auto obj22 = MemoryManager::AllocShared<TypeA>();

	int i = 1000;
	while (i--) {
		auto obj = MemoryManager::AllocShared<TypeA>();
	}

	return true;
}

bool TestThread(int argc) {

	std::thread t1, t2, t3;

	auto work = [&]() {

		int i = 1000;
		while (i--) {
			auto obj = MemoryManager::Alloc<TypeA>(23.3);
		}
	};

	t1 = std::thread(work);
	t2 = std::thread(work);
	t3 = std::thread(work);

	t1.join();
	t2.join();
	t3.join();

	return true;
}

bool TestBuffer() {

	auto Array = MemoryManager::AllocBuffer<TypeA>(64);

	auto Array2 = TypeB::NewArray(64);

	return true;
}

int main(int argc, const char** argv)
{
	if (MemoryManager::Initialize()) {
		std::cout << "MemoryManager::Initialize() -> Failed";
		std::cin.get();
		return 1;
	}

	if (!TestUniquePtr()) {
		std::cin.get();
		return 1;
	}

	if (!TestSharedPtr(argc)) {
		std::cin.get();
		return 1;
	}

	if (!TestThread(argc)) {
		std::cin.get();
		return 1;
	}

	MemoryManager::PrintStatistics();

	return 0;
}