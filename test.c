#include "map.h"
#include <assert.h>

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
	printf("test: strmap		PASSED!\n");
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
	map_free(&intmap);
	printf("test: intmap		PASSED!\n");
}

int main(void) {
	printf("\nUNIT TESTS:\n\n");
	test_strmap();
	test_intmap();
	printf("\n");
}