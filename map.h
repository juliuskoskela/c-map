///////////////////////////////////////////////////////////////////////////////
/// @file map.h
/// @author Julius Koskela
///
/// @brief A hash for the C programming language.
#ifndef MAP_H
#define MAP_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <immintrin.h>

// We resize the map when the load factor * len exceeds the capacity.
#define LOAD_FACTOR 0.75

// We grow the map by a factor of 2 when we resize.
#define GROW(x) (x  == 0 ? 2 : x << 1)

// We use linear probing to resolve collisions. We avoid the modulo
// operation by using a mask instead. This works because the capacity
// is always a power of 2.
#define PROBE(x, n) ((x + 1) & (n - 1))

// Function used to hash the keys.
typedef uint64_t (*hasher_t)(const void *, const size_t);

// Function that returns the size of a key.
typedef size_t (*keysize_t)(const void *);

// Function that frees a key or a value.
typedef void (*free_t)(void *);

///////////////////////////////////////////////////////////////////////////////
/// A hash map.
typedef struct map_s {

	// Conceals the private fields. 56 refers to the size of the internal
	// struct map_private_s.
	uint8_t priv[56];
}	map_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Default hash function.
uint64_t hash_function(const void *key, const size_t len);

///////////////////////////////////////////////////////////////////////////////
/// @brief Create a new map. The map will be empty, and no allocation will
/// be done until the first insertion.
///
/// @param keysize A function that returns the size of a key, This parameter
/// is REQUIRED if NULL is passed, program will EXIT.
/// @param hasher A function that returns a 64-bit hash of a key. This
/// parameter can be NULL, in which case the default hash function will be
/// used.
/// @param free_key A function that frees a key. This parameter can be NULL,
/// in which case the key will not be freed and it is assumed that the key
/// is either allocated on the stack or is freed by the user.
/// @param free_val A function that frees a value. This parameter can be
/// NULL, in which case the value will not be freed and it is assumed that
/// the value is either allocated on the stack or is freed by the user.
map_t map_new(
	const keysize_t keysize,
	const hasher_t hasher,
	const free_t free_key,
	const free_t free_val
);

///////////////////////////////////////////////////////////////////////////////
/// @brief Free the map. If a free function is defined for the keys or
/// values, it will be called for each key and value in the map.
///
/// @param map The map to free.
void map_free(map_t *map);

///////////////////////////////////////////////////////////////////////////////
/// @brief Return the number of elements in the map.
///
/// @param map The map to query.
size_t map_len(const map_t map);

///////////////////////////////////////////////////////////////////////////////
/// @brief Get a value from the map. If the key exists, the value is returned.
/// If the key does not exist, NULL is returned.
///
/// @param map The map to query.
/// @param key The key to search for.
void *map_get(const map_t map, const void *key);

///////////////////////////////////////////////////////////////////////////////
/// @brief Return true if the key exists in the map and false otherwise.
///
/// @param map The map to query.
/// @param key The key to search for.
bool map_contains(const map_t map, const void *key);

///////////////////////////////////////////////////////////////////////////////
/// @brief Insert a value into the map. If the key already exists, the value is
/// updated and the old value is returned. If the key does not exist, the value
/// is updated and NULL is returned.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_insert(map_t *map, const void *key, void *value);

///////////////////////////////////////////////////////////////////////////////
/// @brief Replace a value in the map. If the key exists, the value is updated
/// and the old value is returned. If the key does not exist, NULL is returned
/// and no value is inserted.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_replace(map_t *map, const void *key, void *value);

///////////////////////////////////////////////////////////////////////////////
/// @brief Emplace a value into the map. If the key already exists, the value
/// is not updated and the old value is returned, otherwise the value is
/// inserted and NULL is returned.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_emplace(map_t *map, const void *key, void *value);

///////////////////////////////////////////////////////////////////////////////
/// @brief Remove a value from the map. The value and key are freed using the
/// free functions provided to map_new. If a key does not exist, returns false,
/// otherwise returns true.
///
/// @param map The map to remove from.
/// @param key The key to remove.
bool map_remove(map_t *map, const void *key);

///////////////////////////////////////////////////////////////////////////////
/// @brief Take a value from the map. If the key exists, the value is returned
/// and the key and value are removed from the map. If the key does not exist,
/// NULL is returned. The key is freed using the free function provided to
/// map_new, but the value is not freed and must be freed by the caller.
///
/// @param map The map to take from.
/// @param key The key to take.
void *map_take(map_t *src, const void *key);

void map_print_cells(map_t *map);

#endif
