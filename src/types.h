#ifndef RUBBISH_TYPES_H_
#define RUBBISH_TYPES_H_

#include <cstddef> // std::size_t
#include <vector>

/**
 * Defines types for the rubbish library.
 */

namespace rubbish {

/**
 * Unsigned integer to used to represent object id's
 */
typedef std::size_t id_t;

/**
 * Unsigned integer type to represent size of desired memory blocks
 */
typedef std::size_t size_t;

/**
 * A vector of id's.
 */
typedef std::vector<id_t> id_vector_t;

};

#endif
