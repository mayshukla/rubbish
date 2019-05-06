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
  MyBigClass(rubbish::id_t a, rubbish::id_t b) : referenceA(a), referenceB(b){
    std::cout << "Big Constructor" << std::endl;
  }
  virtual void destructor() {
    std::cout << "Big Destructor" << std::endl;
  }
  virtual rubbish::id_vector_t references() { return {referenceA, referenceB}; }

private:
  rubbish::id_t referenceA;
  rubbish::id_t referenceB;
};

/**
 * A test collectable class which holds no reference to other objects
 */
class MyLittleClass : public rubbish::Collectable {
public:
  MyLittleClass() {
    std::cout << "Little Constructor" << std::endl;
  }
  virtual void destructor() {
    std::cout << "Little Destructor" << std::endl;
  }
  virtual rubbish::id_vector_t references() { return {}; }
};

int main() {
  rubbish::GC gc;

  rubbish::id_t idA;
  rubbish::id_t idB;
  rubbish::id_t idC;

  gc.alloc<MyLittleClass>(idA);
  gc.alloc<MyLittleClass>(idB);
  gc.alloc<MyLittleClass>(idC);

  // print ids known by GC instance
  for (auto &id : gc.getAllIds()) {
    std::cout << id << " ";
  }
  std::cout << std::endl;

  rubbish::id_vector_t rootSet;
  rootSet.push_back(idC);
  gc.collect(rootSet);

  // print ids known by GC instance
  for (auto &id : gc.getAllIds()) {
    std::cout << id << " ";
  }
  std::cout << std::endl;

  gc.collect();

  // print ids known by GC instance
  for (auto &id : gc.getAllIds()) {
    std::cout << id << " ";
  }

  return 0;
}
