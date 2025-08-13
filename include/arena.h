#pragma once
#include <stddef.h>
#include <stdint.h>
#define ARENA_CAP 1000000

typedef struct Arena {
	int64_t capacity;
	size_t position;
	void* data;
} arena_t;

arena_t* ArenaCreate(int64_t capacity);
void* ArenaPush(arena_t* arena, size_t capacity);
void ArenaFree(arena_t* arena);
