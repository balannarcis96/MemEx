# MemEx: intrusive_memory_management
Intrusive, elegant memory management solution for c++17 high performance projects.

Usage Example:
  ```cpp
   class TypeA {
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
    ...
    auto Obj = MemoryManager::Alloc<TypeA>(23.3);
    
    Obj->a = 5;
   }
  ```

![image](https://user-images.githubusercontent.com/8436410/115960733-bf78d800-a51b-11eb-9e62-752e716d9bca.png)

