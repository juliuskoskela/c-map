#include "map.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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

void bench_hash_function(void) {
	char *key = "abcdefghijklmnopqrstuvwxyz";
	size_t len = strlen(key);
	// uint64_t hash1_result = hash_function(key, strlen(key));

	clock_t start = clock();
	for (volatile int i = 0; i < 1000000; i++) {
		hash_function(key, len);
		// assert(result == hash1_result);
	}
	clock_t end = clock();
	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	printf("bench: hash_function\t%f\n", time_spent);
}

void count_collosions(hasher_t f) {
	int buckets[10000];

	memset(buckets, 0, sizeof(buckets));

	for (int i = 0; i < 10000; i++) {
		uint64_t hash = f(&i, sizeof(int));
		buckets[hash % 10000]++;
	}

	int collisions = 0;
	for (int i = 0; i < 10000; i++) {
		if (buckets[i] > 1) {
			collisions += buckets[i] - 1;
		}
	}

	printf("collisions:\t\t%d\n", collisions);
}

int main(void) {
	printf("\nUNIT TESTS:\n\n");
	test_strmap();
	test_intmap();
	test_collision();
	bench_hash_function();
	count_collosions(hash_function);
	printf("\n");
}