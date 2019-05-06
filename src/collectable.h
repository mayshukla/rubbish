#ifndef RUBBISH_COLLECTABLE_H_
#define RUBBISH_COLLECTABLE_H_

#include "types.h"

namespace rubbish {

/**
 * An object which is collectable by rubbish
 * To use rubbish, your object must extend this class and implement all the
 * pure virtual methods.
 */
class Collectable {
public:
  /**
   * Should return an id_vector_t of id's of objects that this object references.
   */
  virtual id_vector_t references() = 0;
  virtual void destructor() = 0;
};

};

#endif
