#ifndef RUBBISH_GC_H_
#define RUBBISH_GC_H_

#include <unordered_map>

#include "collectable.h"
#include "types.h"

namespace rubbish {

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
    return *static_cast<T *>(idMap[id].first);
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
      delete pair.second.first;
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

private:
  // Maps id's to memory location and size in bytes
  std::unordered_map<id_t, std::pair<Collectable *, size_t>> idMap;
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
    delete idMap[id].first;
    idMap.erase(id);
  }
};

};

#endif
