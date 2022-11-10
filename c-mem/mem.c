#include "mem.h"

# define ERROR(msg) { fprintf(stderr, "%s\n%s\n%d\n", msg, __FILE__, __LINE__); }
# define PANIC(msg) { ERROR(msg); exit(1); }

mem_t mem_new(void) {
	return (mem_t) {
		.cap = 0,
		.data = NULL
	};
}

void mem_free(mem_t *mem) {
	if (!mem || !mem->data) {
		return;
	}
	free(mem->data);
	mem->data = NULL;
	mem->cap = 0;
}

int mem_alloc(mem_t *mem, size_t cap) {
	if (!mem || cap == 0) {
		ERROR("Invalid arguments");
		return 0;
	}
	if (mem->cap < cap) {
		mem->data = realloc(mem->data, cap);
		if (!mem->data) {
			ERROR("Failed to allocate memory");
			return 0;
		}
		mem->cap = cap;
	}
	return 1;
}

int mem_calloc(mem_t *mem, size_t cap) {
	if (!mem || cap == 0) {
		ERROR("Invalid arguments");
		return 0;
	}
	if (mem->cap < cap) {
		mem->data = realloc(mem->data, cap);
		memset(mem->data, 0, cap);
		if (!mem->data) {
			ERROR("Failed to allocate memory");
			return 0;
		}
		mem->cap = cap;
	}
	memset(mem->data, 0, cap);
	return 1;
}

int mem_realloc(mem_t *mem, size_t cap) {
	if (!mem || cap == 0) {
		ERROR("Invalid arguments");
		return 0;
	}
	if (mem->cap < cap) {
		mem->data = realloc(mem->data, cap);
		if (!mem->data) {
			ERROR("Failed to allocate memory");
			return 0;
		}
		mem->cap = cap;
	}
	return 1;
}

void mem_set(mem_t *mem, uint8_t byte) {
	if (!mem || !mem->data) {
		return;
	}
	memset(mem->data, byte, mem->cap);
}

void mem_zero(mem_t *mem) {
	if (!mem || !mem->data) {
		return;
	}
	memset(mem->data, 0, mem->cap);
}

slice_t mem_slice(mem_t *mem, size_t start, size_t end) {
	if (!mem || !mem->data || start > end || end > mem->cap) {
		return (slice_t) {
			.data = NULL,
			.stride = 0,
			.len = 0
		};
	}
	return (slice_t) {
		.data = mem->data + start,
		.stride = 1,
		.len = end - start
	};
}

bool slice_is_some(slice_t slice) {
	return slice.data != NULL;
}

bool slice_is_none(slice_t slice) {
	return slice.data == NULL;
}

int slice_cpy(slice_t dst, slice_t src) {
	if (!dst->data || !src->data || dst->stride != src->stride) {
		ERROR("Invalid arguments");
		return 0;
	}
	memcpy(dst->data, src->data, dst->len * dst->stride);
	return 1;
}

int slice_cmp(slice_t a, slice_t b) {
	if (!a->data || !b->data || a->stride != b->stride) {
		ERROR("Invalid arguments");
		return 0;
	}
	return memcmp(a->data, b->data, a->len * a->stride);
}

bool slice_eq(slice_t a, slice_t b) {
	if (!a->data || !b->data || a->stride != b->stride) {
		ERROR("Invalid arguments");
		return 0;
	}
	if (a->len != b->len) {
		return 0;
	}
	return slice_cmp(a, b) == 0;
}