///////////////////////////////////////////////////////////////////////////////
/// @file str.h
/// @author Julius Koskela
///
/// @brief A hash for the C programming language.
#ifndef MEM_H
#define MEM_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct mem_s {
	uint8_t *data;
	size_t cap;
} mem_t;

mem_t mem_new(void);
void mem_free(mem_t *mem);
int mem_alloc(mem_t *mem, const size_t cap);
int mem_calloc(mem_t *mem, const size_t cap);
int mem_realloc(mem_t *mem, const size_t cap);
void mem_set(mem_t *mem, const uint8_t byte);
void mem_zero(mem_t *mem);
slice_t mem_slice(mem_t *mem, const size_t start, const size_t end);

typedef struct slice_s {
	const void *data;
	const size_t stride;
	const size_t len;
} slice_t;

bool slice_is_some(const slice_t slice);
bool slice_is_none(const slice_t slice);
int slice_cpy(slice_t dst, const slice_t src);
int slice_cmp(const slice_t a, const slice_t b);
bool slice_eq(const slice_t a, const slice_t b);
void *slice_ptr(slice_t slice);

typedef struct iterator_s {
	const slice_t slice;
	size_t position;
	bool reverse;
} iterator_t;

iterator_t iter_new(const slice_t slice);
void iter_reverse(iterator_t *iter);
void *iter_next(iterator_t *iter);

// typedef struct str_s {
// 	mem_t mem;
// 	size_t len;
// } str_t;

// str_t str_new(void);
// str_t str_free(const str_t *src);
// str_t str_from(const char *src);