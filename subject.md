# Subject: Map

In this subject we learn how to implement a generic hash map in C. The subject will provide function prototypes, instructions and a testing suite to help you implement your own hash map library.

## Rules

1. The implementation needs to pass all the provided tests.

2. Functions defined in the assignments need to be prototyped exactly as instructed and should perform the functionality described in the documentation comments or in the assignment description.

3. Proper usage of the library should never produce leaks or undefined behavior.

4. You are allowed to include the libraries from the standard library described below or any library that you are the author of, as long as they don't link to external libraries not on the list.
    - stdio.h
    - stdint.h
    - stdlib.h
    - string.h
    - stdbool.h
	- errno.h

5. You are allowed to use the following functions from the standard libraries:
    - malloc
    - calloc
    - realloc
    - free
    - memset
    - memcpy
    - memmove
    - memcmp
    - fprintf
	- exit

6. You will provide a header filed called `map.h` at the root of the repository which will include prototypes for all of the functions in the library and definitions for related structures.

7. The program will be compiled with the `make` command. You will provide a `Makefile` at the root of the repository. Your Makefile will compile both a static and shared versions of the library. The compiled binaries will be called `libmap.a` and `libmap.so` respectively. All produced object files and binaries will be put in a folder named `target`.

8. You will provide at minimum the following rules in your Makefile:
    - all: Compile binaries
    - clean: Delete all but the target binaries from the target folder
    - fclean: Delete target folder
    - re: Fclean and recompile

9. Your will compile your program using either the `gcc` or `clang` compiler. When compiling you must use the following error-flags:
	- Wall

## Implementation

### Type-definitions

Your header will include the following type-definitions:

```c
typedef struct map_s {
    /* struct fields omitted */
}   map_t;

// Function used to hash the keys.
typedef uint64_t (*hasher_t)(const void *, const size_t);

// Function that returns the size of a key.
typedef size_t (*keysize_t)(const void *);

// Function that frees a key or a value.
typedef void (*free_t)(const void *);
```

*You are allowed to put additional macros and type-definitions in your header-file.*

### Ex0: Hasher

The purpose of a hash function is to turn our key into a 64-bit long unsigned integer, which we will use to index into our map. A hash function could be anything, but a quality hash function will allow our map to operate faster. Depending on the key-type, other hash functions can perform better than others, thus we want to be able to choose our hash function for our map while providing a good default.

Create a function `hasher` with the following prototype:

```c
uint64_t hasher(const void *key, const size_t len);
```

### Ex1: New

Create a function `map_new` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Create a new map. The map will be empty, and no allocation will
/// be done until the first insertion.
///
/// @param keysize A function that returns the size of a key, This parameter
/// is REQUIRED if NULL is passed, program will EXIT with the exit code EINVAL
/// and print and error on stderr.
/// @param hasher A function that returns a 64-bit hash of a key. This
/// parameter can be NULL, in which case the default hash function will be
/// used.
/// @param free_key A function that frees a key. This parameter can be NULL,
/// in which case the key will not be freed and it is assumed that the key
/// is either allocated on the stack or is freed by the user.
/// @param free_val A function that frees a value. This parameter can be
/// NULL, in which case the value will not be freed and it is assumed that
/// the value is either allocated on the stack or is freed by the user.
/// @return A new map.
map_t map_new(
    const keysize_t keysize,
    const hasher_t hasher,
    const free_t free_key,
    const free_t free_val
);
```

### Ex2: Free

Create a function `map_free` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Free the map. If a free function is defined for the keys or
/// values, it will be called for each key and value in the map.
///
/// @param map The map to free.
void map_free(map_t *map);
```

### Ex3: Len

Create a function `map_len` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Return the number of elements in the map.
///
/// @param map The map to query.
size_t map_len(const map_t map);
```

### Ex4: Get

Create a function `map_get` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Get a value from the map. If the key exists, the value is returned.
/// If the key does not exist, NULL is returned.
///
/// @param map The map to query.
/// @param key The key to search for.
void *map_get(const map_t map, const void *key);
```

### Ex5:Contains

Create a function `map_contain` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Return true if the key exists in the map and false otherwise.
///
/// @param map The map to query.
/// @param key The key to search for.
bool map_contains(const map_t map, const void *key);
```

### Ex6: Insert

Create a function `map_insert` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Insert a value into the map. If the key already exists, the value is
/// updated and the old value is returned. If the key does not exist, the value
/// is updated and NULL is returned.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_insert(map_t *map, const void *key, void *value);
```

### Ex7: Replace

Create a function `map_replace` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Replace a value in the map. If the key exists, the value is updated
/// and the old value is returned. If the key does not exist, NULL is returned
/// and no value is inserted.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_replace(map_t *map, const void *key, void *value);
```

### Ex8: Emplace

Create a function `map_emplace` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Emplace a value into the map. If the key already exists, the value
/// is not updated and the old value is returned, otherwise the value is
/// inserted and NULL is returned.
///
/// @param map The map to insert into.
/// @param key The key to insert.
/// @param val The value to insert.
void *map_emplace(map_t *map, const void *key, void *value);
```

### Ex9: Remove

Create a function `map_remove` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Remove a value from the map. The value and key are freed using the
/// free functions provided to map_new. If a key does not exist, returns false,
/// otherwise returns true.
///
/// @param map The map to remove from.
/// @param key The key to remove.
bool map_remove(map_t *map, const void *key);
```

### Ex10: Take

Create a function `map_take` with the following prototype:

```c
///////////////////////////////////////////////////////////////////////////////
/// @brief Take a value from the map. If the key exists, the value is returned
/// and the key and value are removed from the map. If the key does not exist,
/// NULL is returned. The key is freed using the free function provided to
/// map_new, but the value is not freed and must be freed by the caller.
///
/// @param map The map to take from.
/// @param key The key to take.
void *map_take(map_t *src, const void *key);
```

## Conclusion

Congratulations on completing the exercises! If you find any errors from the exercises or the documentation, please feel free to leave a pull request. New tests for the test-suite are also welcome!

author: [Julius Koskela](https://github.com/juliuskoskela)
