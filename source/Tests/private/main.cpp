#include <iostream>

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
	int a{ 0 };
};

struct TypeB{
	int a{ 0 };
};

int main(int argc, const char** argv) {

	MPtr<TypeA> a;

	return 0;
}