# A Hash Map for C

As simple generic hash-map implementation for C.

## Features

- Generic key and value types.
- Arbitrary sized keys.
- SIMD-optimized hashing for supported platforms.
- Recursive deallocation of keys and values.

## Example

A map with static string keys and static values using default the hasher.

```c

#include "map.h"

size_t keysize_str(const void *a) {
    return strlen((const char *)a);
}

int main(void) {
    map_t map = map_new(
        keysize_str, /* function that returns key size */
        NULL,        /* hash function, NULL = default */
        NULL,        /* free function for keys, NULL = no free */
        NULL         /* free function for values, NULL = no free */
    );
    map_insert(&map, "foo", "bar");
    map_insert(&map, "baz", "qux");
    printf("%s\n", (char *)map_get(map, "foo"));
    printf("%s\n", (char *)map_get(map, "baz"));
    map_free(&map);
    return 0;
}

```
