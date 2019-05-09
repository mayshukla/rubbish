#ifndef RUBBISH_GC_H_
#define RUBBISH_GC_H_

#include <unordered_map>
#include <iostream>

#include "collectable.h"
#include "types.h"

namespace rubbish {

/**
 * Stores pointer to an object, that objects size, and reachability
 */
class GCRecord {
public:
  // Constructor with no args required by std::unordered_map
  // IMPORTANT: ensure that ptr and size are set before using a GCRecord
  // instance
  GCRecord(){};

  GCRecord(Collectable *t_ptr, size_t t_size) : ptr(t_ptr), size(t_size){};

  Collectable *getPtr() { return ptr; }

  void setPtr(Collectable *t_ptr) { ptr = t_ptr; }

  size_t getSize() { return size; }

  size_t setSize(size_t t_size) { size = t_size; }

  bool isReachable() { return reachable; }

  void setReachable(bool isReachable) { reachable = isReachable; }

private:
  Collectable *ptr;
  size_t size;
  bool reachable;
};

/**
 * Handles memory allocation and garbage collection.
 */
class GC {
public:
  GC() {
    // Initialize nextId
    nextId = 0;
  }

  /**
   * Allocates memory on the heap and returns an id to reference that object.
   */
  template <class T>
  bool alloc(id_t &id) {
    // Heap allocate new object
    T* t = new T;
    // Store that object's id both internally and externally
    id = nextId;
    idMap[nextId] = {t, sizeof(T)};
    // Ensure next object has a unique id
    nextId++;
    return true;
  }

  /**
   * Returns a reference to the object with the given id
   */
  template <class T>
  T &reference(id_t &id) {
    return *static_cast<T *>(idMap[id].getPtr());
  }

  /**
   * Frees all unused blocks based on what is reachable.
   */
  void collect(id_vector_t &rootSet) {
    for (auto &id : rootSet) {
      if (idMap.find(id) != idMap.end()) {
        freeById(id);
      }
    }
  }

  /**
   * Frees ALL allocated memory
   */
  void collect() {
    for (auto &pair : idMap) {
      delete pair.second.getPtr();
    }
    idMap.clear();
  }

  /**
   * Returns list of all ids known by the GC instance
   */
  id_vector_t getAllIds() {
    id_vector_t ids;
    for (auto &pair : idMap) {
      ids.push_back(pair.first);
    }
    return ids;
  }

  /**
   * Prints a space-separated list of all ids to std out.
   */
  void printAllIds() {
    std::cout << "Known ids: ";
    for (auto &id : getAllIds()) {
      std::cout << id << " ";
    }
    std::cout << std::endl;
  }

private:
  // Maps id's to memory location and size in bytes
  std::unordered_map<id_t, GCRecord> idMap;
  // Next unique Id
  id_t nextId;

  /**
   * Changes nextId to a new unique id
   * TODO make this better
   */
  void incrementId() {
    nextId++;
  }

  /**
   * Deletes an object with given id. Also removes it from idMap
   */
  void freeById(id_t &id) {
    delete idMap[id].getPtr();
    idMap.erase(id);
  }
};

};

#endif
