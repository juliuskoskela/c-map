#include "map.h"
#include <assert.h>
#include <time.h>

size_t keysize_str(const void *a) {
	return strlen((const char *)a);
}

size_t keysize_int(const void *a) {
	return !a ? 0 : sizeof(int);
}

void test_strmap(void) {
	map_t strmap = map_new(keysize_str, NULL, NULL, NULL);

	map_insert(&strmap, "Luke Skywalker", "I'm a Jedi, like my father before me");
	map_insert(&strmap, "Darth Vader", "I am your father");
	map_insert(&strmap, "Han Solo", "I love Leia");
	map_insert(&strmap, "Leia Organa", "I thought about it");

	assert(map_len(strmap) == 4);
	assert(strcmp(map_get(strmap, "Luke Skywalker"), "I'm a Jedi, like my father before me") == 0);
	map_insert(&strmap, "Luke Skywalker", "I hate my father!");
	assert(strcmp(map_get(strmap, "Luke Skywalker"), "I hate my father!") == 0);
	map_emplace(&strmap, "Luke Skywalker", "I'm a Jedi, like my father before me");
	assert(strcmp(map_get(strmap, "Luke Skywalker"), "I hate my father!") == 0);
	map_free(&strmap);
	printf("test: strmap\t\tPASSED!\n");
}

void test_intmap(void) {
	map_t intmap = map_new(keysize_int, NULL, NULL, NULL);

	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	map_insert(&intmap, &a, "one");
	map_insert(&intmap, &b, "two");
	map_insert(&intmap, &c, "three");
	map_insert(&intmap, &d, "four");

	assert(map_len(intmap) == 4);
	assert(strcmp(map_get(intmap, &a), "one") == 0);
	assert(strcmp(map_get(intmap, &b), "two") == 0);
	assert(strcmp(map_get(intmap, &c), "three") == 0);
	assert(strcmp(map_get(intmap, &d), "four") == 0);
	assert(map_contains(intmap, &a));
	assert(map_contains(intmap, &b));
	assert(map_contains(intmap, &c));
	assert(map_contains(intmap, &d));
	map_remove(&intmap, &a);
	assert(map_contains(intmap, &a) == false);
	map_free(&intmap);
	printf("test: intmap\t\tPASSED!\n");
}

static uint64_t crappy_hash(const void *key, const size_t len) {
	if (!key || !len) {
		return 0;
	}
	return 42;
}

void test_collision(void) {
	map_t intmap = map_new(keysize_int, crappy_hash, free, NULL);

	for (int i = 0; i < 100; i++) {
		int *a = malloc(sizeof(int));
		*a = i;
		map_insert(&intmap, a, "one");
	}

	int x = 1;
	int y = 99;
	int z = 42;
	assert(map_contains(intmap, &x));
	assert(map_contains(intmap, &y));
	assert(map_contains(intmap, &z));

	map_remove(&intmap, &z);
	assert(!map_contains(intmap, &z));
	assert(map_contains(intmap, &x) == true);
	assert(map_contains(intmap, &y) == true);
	assert(map_len(intmap) == 99);

	map_remove(&intmap, &x);
	assert(!map_contains(intmap, &x));
	int new = 76;
	assert(map_contains(intmap, &new) == true);
	assert(map_contains(intmap, &y) == true);
	assert(map_len(intmap) == 98);

	map_free(&intmap);

	printf("test: collision\t\tPASSED!\n");
}

static uint64_t hash_function_1(const void *key, const size_t len) {
	uint8_t rk[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f };
	__m128i data = _mm_setzero_si128();
	int size = len < 16 ? len : 16;
	memcpy(&data, key, size);
	__m128i aeskey = _mm_loadu_si128((__m128i *)rk);
	__m128i result = _mm_aesenc_si128(data, aeskey);
	return _mm_extract_epi64(result, 0);
}

# define SHIFT(x, n) ((x << n) >> n)

static uint64_t hash_function_2(const void *key, const size_t len) {
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

void bench_hash_functions(void) {
	char *key = "Hello World!";
	uint64_t hash1_result = hash_function_1(key, strlen(key));

	clock_t start = clock();
	for (volatile int i = 0; i < 1000000; i++) {
		hash1_result = hash_function_1(key, strlen(key));
		assert(hash1_result == hash1_result);
	}
	clock_t end = clock();
	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	printf("bench: hash1\t\t%f\n", time_spent);

	uint64_t hash2_result = hash_function_2(key, strlen(key));
	start = clock();
	for (volatile int i = 0; i < 1000000; i++) {
		hash2_result = hash_function_2(key, strlen(key));
		assert(hash2_result == hash2_result);
	}
	end = clock();
	time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	printf("bench: hash2\t\t%f\n", time_spent);
}

int main(void) {
	printf("\nUNIT TESTS:\n\n");
	test_strmap();
	test_intmap();
	test_collision();
	bench_hash_functions();
	simple();
	printf("\n");
}