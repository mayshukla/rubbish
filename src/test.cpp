#include <iostream>

#include "collectable.h"
#include "types.h"
#include "gc.h"

/**
 * Testing using the library
 */

/**
 * A test collectable class which holds references to other objects.
 */
class MyBigClass : public rubbish::Collectable {
public:
  MyBigClass() {
    std::cout << "Big Constructor" << std::endl;
  }
  void setA(rubbish::id_t t_a) {
    a = t_a;
  }
  void setB(rubbish::id_t t_b) {
    b = t_b;
  }
  virtual ~MyBigClass() {
    std::cout << "Big Destructor" << std::endl;
  }
  virtual rubbish::id_vector_t references() { return {a, b}; }

private:
  // References to other rubbish-managed objects
  rubbish::id_t a;
  rubbish::id_t b;
};

/**
 * A test collectable class which holds no reference to other objects
 */
class MyLittleClass : public rubbish::Collectable {
public:
  MyLittleClass() {
    std::cout << "Little Constructor" << std::endl;
  }
  virtual ~MyLittleClass() {
    std::cout << "Little Destructor" << std::endl;
  }
  virtual rubbish::id_vector_t references() { return {}; }
};

int main() {
  rubbish::GC gc;

  rubbish::id_t idA;
  rubbish::id_t idB;
  rubbish::id_t idC;
  rubbish::id_t idD;

  gc.alloc<MyLittleClass>(idA);
  gc.alloc<MyLittleClass>(idB);
  gc.alloc<MyBigClass>(idC);
  gc.alloc<MyLittleClass>(idD);

  // Make object C reference A and B
  gc.reference<MyBigClass>(idC).setA(idA);
  gc.reference<MyBigClass>(idC).setB(idB);

  gc.printAllIds();

  // Delete all objects that are not in rootSet or referenced by rootSet
  rubbish::id_vector_t rootSet;
  rootSet.push_back(idC);
  gc.setRootSet(rootSet);
  gc.printReachableIds();
  gc.printAllIds();
  gc.collect();

  //gc.printAllIds();
  //gc.printReachableIds();

  // Free all managed memory
  //gc.collect();
  //gc.printAllIds();

  return 0;
}
