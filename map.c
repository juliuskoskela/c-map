///////////////////////////////////////////////////////////////////////////////
/// @file map.c
/// @author Julius Koskela
///
/// @brief A hash map implementation.
#include "map.h"

# define ERROR(msg) { fprintf(stderr, "%s\n%s\n%d\n", msg, __FILE__, __LINE__); }
# define PANIC(msg) { ERROR(msg); exit(EINVAL); }
# define IS_FULL(map) (map->len > (map->cap * LOAD_FACTOR))
# define IS_UNINIT(map) (map->cap == 0)
# define IS_EMPTY(map) (map->len == 0)
# define IS_VACANT(node) (node->key == NULL)
# define IS_OCCUPIED(node) (!IS_VACANT(node))
# define KEYCMP(a, b, as, bs) (as == bs && memcmp(a, b, as) == 0)

typedef struct node_s {
	void *value;
	const void *key;
} node_t;

typedef struct map_private_s {
	node_t *data;
	size_t len;
	size_t cap;
	hasher_t hasher;
	keysize_t keysize;
	free_t free_key;
	free_t free_val;
} map_private_t;

# define SHIFT(x, n) ((x << n) >> n)

uint64_t hash_function(const void *key, const size_t len) {
	uint8_t *data = (uint8_t *)key;
	uint64_t hash64 = 14695981039346656037LLU;
	uint64_t prime = 591798841;
	uint64_t ndhead = 0;

	if (len > 8) {
		uint64_t cycles = ((len - 1) >> 4) + 1;
		ndhead = len - (cycles << 3);
		while (cycles--) {
			hash64 = (hash64 ^ (*(uint64_t *)(data))) * prime;
			hash64 = (hash64 ^ (*(uint64_t *)(data + ndhead))) * prime;
			data += 8;
		}
		return (hash64 ^ (hash64 >> 32));
	}
	memcpy(&ndhead, data, len);
	hash64 = (hash64 ^ SHIFT(ndhead, (8 - len) << 3)) * prime;
	return (hash64 ^ (hash64 >> 32));
}

static node_t node_new(const void *key, void *value) {
	return (node_t) {
		.key = key,
		.value = value
	};
}

static node_t node_empty(void) {
	return (node_t) {
		.key = NULL,
		.value = NULL
	};
}

static void node_free(node_t *node, free_t free_key, free_t free_val) {
	if (node->key != NULL && free_key != NULL) {
		free_key((void *)node->key);
	}
	if (node->value != NULL && free_val != NULL) {
		free_val(node->value);
	}
	memset(node, 0, sizeof(node_t));
}

static int map_resize(map_private_t *map, size_t cap) {
	if (IS_UNINIT(map)) {
		map->data = (node_t *)calloc(cap, sizeof(node_t));
		map->cap = cap;
		return 1;
	}
	map_private_t newmap = (map_private_t) {
		.data = (node_t *)calloc(cap, sizeof(node_t)),
		.len = 0,
		.cap = cap,
		.hasher = map->hasher,
		.keysize = map->keysize,
		.free_key = map->free_key,
		.free_val = map->free_val
	};
	map_t *newmap_ptr = (map_t *)&newmap;
	for (size_t i = 0; i < map->cap; i++) {
		if (map->data[i].key) {
			map_insert(
				newmap_ptr,
				map->data[i].key,
				map->data[i].value
			);
		}
	}
	free(map->data);
	memcpy(map, newmap_ptr, sizeof(map_private_t));
	return 1;
}

map_t map_new(
	const keysize_t keysize,
	const hasher_t hasher,
	const free_t free_key,
	const free_t free_val
) {
	if (!keysize) {
		PANIC("Must specify a function to calculate key size");
	}
	map_private_t newmap_priv = (map_private_t) {
		.data = NULL,
		.len = 0,
		.cap = 0,
		.hasher = hasher == NULL ? hash_function : hasher,
		.keysize = keysize,
		.free_key = free_key,
		.free_val = free_val
	};
	map_t newmap;
	memcpy(&newmap, &newmap_priv, sizeof(map_private_t));
	return newmap;
}

void map_free(map_t *map) {
	map_private_t *priv = (map_private_t *)map;
	if (!priv || !priv->data) {
		return;
	}
	for (size_t i = 0; i < priv->cap; i++) {
		if (priv->data[i].key) {
			node_free(&priv->data[i], priv->free_key, priv->free_val);
		}
	}
	free(priv->data);
	memset(priv, 0, sizeof(map_t));
}

size_t map_len(const map_t map) {
	map_private_t *priv = (map_private_t *)&map;
	return priv->len;
}

bool map_contains(const map_t map, const void *key) {
	return map_get(map, key) != NULL;
}

void *map_get(const map_t map, const void *key) {
	if (!key) {
		PANIC("Invalid map or key");
	}
	map_private_t *priv = (map_private_t *)&map;
	if (IS_UNINIT(priv) || IS_EMPTY(priv)) {
		return NULL;
	}
	size_t keysize = priv->keysize(key);
	uint64_t hash = priv->hasher(key, keysize);
	size_t i = 0;
	while (1) {
		size_t probe = PROBE(hash + i, priv->cap);
		node_t *curr = &priv->data[probe];
		if (IS_VACANT(curr)) {
			return NULL;
		} else if (KEYCMP(key, curr->key, keysize, priv->keysize(curr->key))) {
			return curr->value;
		}
		i++;
	}
}

void *map_insert(map_t *map, const void *key, void *value) {
	if (!map || !key || !value) {
		PANIC("Invalid map, key or value");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv) || IS_FULL(priv)) {
		if (!map_resize(priv, GROW(priv->cap))) {
			PANIC("Failed allocating memory for map");
		}
	}
	size_t keysize = priv->keysize(key);
	uint64_t hash = priv->hasher(key, keysize);
	size_t i = 0;
	while (1) {
		size_t probe = PROBE(hash + i, priv->cap);
		node_t *curr = &priv->data[probe];
		if (IS_VACANT(curr)) {
			*curr = node_new(key, value);
			priv->len++;
			return NULL;
		} else if (KEYCMP(key, curr->key, keysize, priv->keysize(curr->key))) {
			void *oldval = curr->value;
			curr->value = value;
			return oldval;
		}
		i++;
	}
}

void *map_replace(map_t *map, const void *key, void *value) {
	if (!map || !key || !value) {
		PANIC("Invalid map, key or value");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv) || IS_FULL(priv)) {
		if (!map_resize(priv, GROW(priv->cap))) {
			PANIC("Failed allocating memory for map");
		}
	}
	size_t keysize = priv->keysize(key);
	uint64_t hash = priv->hasher(key, keysize);
	size_t i = 0;
	while (1) {
		size_t probe = PROBE(hash + i, priv->cap);
		node_t *curr = &priv->data[probe];
		if (IS_VACANT(curr)) {
			return NULL;
		} else if (KEYCMP(key, curr->key, keysize, priv->keysize(curr->key))) {
			void *oldval = curr->value;
			curr->value = value;
			return oldval;
		}
		i++;
	}
}

void *map_emplace(map_t *map, const void *key, void *value) {
	if (!map || !key || !value) {
		PANIC("Invalid map, key or value");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv) || IS_FULL(priv)) {
		if (!map_resize(priv, GROW(priv->cap))) {
			PANIC("Failed allocating memory for map");
		}
	}
	size_t keysize = priv->keysize(key);
	uint64_t hash = priv->hasher(key, keysize);
	size_t i = 0;
	while (1) {
		size_t probe = PROBE(hash + i, priv->cap);
		node_t *curr = &priv->data[probe];
		if (IS_VACANT(curr)) {
			*curr = node_new(key, value);
			priv->len++;
			return NULL;
		} else if (KEYCMP(key, curr->key, keysize, priv->keysize(curr->key))) {
			return curr->value;
		}
		i++;
	}
}

bool map_remove(map_t *map, const void *key) {
	if (!map || !key) {
		PANIC("Invalid map or key");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv) || IS_EMPTY(priv)) {
		return false;
	}
	size_t keysize = priv->keysize(key);
	uint64_t hash = priv->hasher(key, keysize);
	size_t i = 0;
	while (1) {
		node_t *curr = &priv->data[PROBE(hash + i, priv->cap)];
		if (IS_VACANT(curr)) {
			return false;
		} else if (KEYCMP(key, curr->key, keysize, priv->keysize(curr->key))) {
			// Find end of collision chain
			node_t *end = &priv->data[PROBE(hash + i, priv->cap)];
			while (end->key && hash == priv->hasher(end->key, priv->keysize(end->key))) {
				i++;
				end = &priv->data[PROBE(hash + i, priv->cap)];
			}
			end = &priv->data[PROBE(hash + i - 1, priv->cap)];
			if (end == curr) {
				// No collision chain
				node_free(curr, priv->free_key, priv->free_val);
				*curr = node_empty();
			} else {
				// Collision chain
				node_free(curr, priv->free_key, priv->free_val);
				memcpy(curr, end, sizeof(node_t));
				*end = node_empty();
			}
			priv->len--;
			return true;
		}
		i++;
	}
}

void *map_take(map_t *map, const void *key) {
	if (!map || !key) {
		PANIC("Invalid map or key");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv) || IS_EMPTY(priv)) {
		return NULL;
	}
	void *value = map_get(*map, key);
	if (value) {
		map_remove(map, key);
	}
	return value;
}

void map_print_cells(map_t *map) {
	if (!map) {
		PANIC("Invalid map");
	}
	map_private_t *priv = (map_private_t *)map;
	if (IS_UNINIT(priv)) {
		return;
	}
	for (size_t i = 0; i < priv->cap; i++) {
		node_t *curr = &priv->data[i];
		if (IS_OCCUPIED(curr)) {
			printf("%zu: is occupied\n", i);
		} else if (IS_VACANT(curr)) {
			printf("%zu: is vacant\n", i);
		} else {
			printf("%zu: is empty\n", i);
		}
	}
}
