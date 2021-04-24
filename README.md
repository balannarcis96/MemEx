# MemEx: intrusive_memory_management
Intrusive, elegant memory management solution for c++17 high performance projects.

```
Install guid (Windows & Visual Studio):
  1.Download and install CMake 3.8 or newer
  2.Clone the repo
  3.Create 'build' folder at the same level with the 'source' folder
  4.Start cmd inside the build folder and run 'cmake ../source'
  5.Build and run MemEx_Tests
```

Usage Example:
  ```cpp
   class TypeA { //Example Type
   public:
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
   struct TypeB: IResource<TypeB> { //Intrusive example (simple static API added to the type)
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
   ...
   {
    //some scope
    auto Obj = MemoryManager::Alloc<TypeA>(23.3); //Allocate object
    
    Obj->a = 5; //modify object members etc
    
    auto ObjB = TypeB::New(23.3);
   }
   ...
   {
    //some other scope
    auto Obj = MemoryManager::AllocShared<TypeA>(23.3); //Allocate object
    
    PassPointerToOtherThread(Obj);
    
    Obj->a = 5; //modify object members etc
    
    auto Objb = TypeB::NewShared(23.3);
   }
   ...
   {
    //some other scope allocate an array of size 64 of TypeA
    auto Array = MemoryManager::AllocBuffer<TypeA>(64);
    
    for(size_t i =0; i < 64; i++)
    {
      Array[i].a = i + 1;
      ...
     }
     
     auto Array2 = TypeB::NewArray(64);
   }
  ```

```cpp
MemoryManager::PrintStatistics();
```
![image](https://user-images.githubusercontent.com/8436410/115960733-bf78d800-a51b-11eb-9e62-752e716d9bca.png)

