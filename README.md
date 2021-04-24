# MemEx: intrusive_memory_management
Intrusive, elegant memory management solution for c++17 high performance projects.

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
   ...
   {
    //some scope
    auto Obj = MemoryManager::Alloc<TypeA>(23.3); //Allocate object
    
    Obj->a = 5; //modify object members etc
   }
   ...
   {
    //some other scope
    auto Obj = MemoryManager::AllocShared<TypeA>(23.3); //Allocate object
    
    PassPointerToOtherThread(Obj);
    
    Obj->a = 5; //modify object members etc
   }
   ...
   {
    //some other scope allocate an array of TypeA
    auto Array = MemoryManager::AllocBuffer<TypeA>(64);
    
    for(size_t i =0; i < 64; i++)
    {
      Array[i].a = i + 1;
      ...
     }
   }
  ```

![image](https://user-images.githubusercontent.com/8436410/115960733-bf78d800-a51b-11eb-9e62-752e716d9bca.png)

