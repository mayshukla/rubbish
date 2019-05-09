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

  void setSize(size_t t_size) { size = t_size; }

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
    incrementId();
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
  void collect() {
    freeMarkedUnreachable();
  }

  /**
   * Frees ALL allocated memory
   */
  void collectAll() {
    for (auto pair : idMap) {
      delete pair.second.getPtr();
    }
    idMap.clear();
  }

  /**
   * Returns list of all ids known by the GC instance
   */
  id_vector_t getAllIds() {
    id_vector_t ids;
    for (auto pair : idMap) {
      ids.push_back(pair.first);
    }
    return ids;
  }

  /**
   * Prints a space-separated list of all ids to std out.
   */
  void printAllIds() {
    std::cout << "Known ids: ";
    for (auto id : getAllIds()) {
      std::cout << id << " ";
    }
    std::cout << std::endl;
  }

  /**
   * Updates the root set. Should be called before collect().
   * Also does the marking.
   */
  void setRootSet(id_vector_t t_rootSet) {
    rootSet = t_rootSet;
    mark();
  }

  /**
   * Prints a space-separated list of all reachable ids to std out.
   */
  void printReachableIds() {
    std::cout << "Reachable ids: ";
    for (auto &pair : idMap) {
      GCRecord &record = pair.second;
      std::cout << "id/reachable: " << pair.first << "/" << record.isReachable() << std::endl;
      if (record.isReachable()) {
        std::cout << pair.first << " ";
      }
    }
    std::cout << std::endl;
  }

private:
  // Maps id's to memory location and size in bytes
  std::unordered_map<id_t, GCRecord> idMap;
  // Next unique id
  id_t nextId;
  // Root set of id's
  id_vector_t rootSet;

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
  void freeById(const id_t &id) {
    std::cout << "Freeing: " << id << std::endl;
    delete idMap[id].getPtr();
    idMap.erase(id);
  }

  /**
   * Mark objects as reachable/unreachable given a root set.
   * This is the "mark" phase of mark-and-sweep.
   */
  void mark() {
    // First mark all objects as unreachable
    for (auto &pair : idMap) {
      GCRecord &record = pair.second;
      record.setReachable(false);
    }

    for (auto &id : rootSet) {
      markIdRecursive(id);
    }
  }

  /**
   * Mark an object with id "id" as reachable and also recursively mark all the
   * objects it references as reachable.
   */
  void markIdRecursive(const id_t &id) {
    std::cout << "Marking id: "  << id << std::endl;
    GCRecord &record = idMap[id];
    // Mark current object reachable
    record.setReachable(true);
    // Mark all objects it references as reachable
    for (auto reachableId : record.getPtr()->references()) {
      std::cout << "Current id: " << id;
      std::cout << " references: " << reachableId << std::endl;
      markIdRecursive(reachableId);
    }
  }

  /**
   * Deletes all objects that have been marked unreachable.
   * This is the "sweep" phase of mark-and-sweep.
   */
  void freeMarkedUnreachable() {
    for (auto &pair : idMap) {
      GCRecord &record = pair.second;
      //std::cout << "id/reachable: " << pair.first << "/" << record.isReachable() << std::endl;
      if (!(record.isReachable())) {
        freeById(pair.first);
      }
    }
    std::cout << "Done sweeping" << std::endl;
  }
};

};

#endif
