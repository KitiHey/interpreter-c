#include "arena.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

arena_t* ArenaCreate(int64_t capacity) {
		if (capacity == 0) { return NULL; }

		arena_t* arena = malloc(sizeof(arena_t));
		if (!arena) return NULL;

		arena->capacity = capacity;
		arena->position = 0;
		arena->data = malloc(capacity);
		if (!arena->data) {
				free(arena);
				return NULL;
		}
		return arena;
};

void* ArenaPush(arena_t* arena, size_t capacity) {
		if (arena->position+capacity > arena->capacity) {
				printf("Arena out of bounds: %ld>%ld\n", arena->position+capacity, arena->capacity);
				return NULL;
		}

		void *p = arena->data+arena->position;
		arena->position += capacity;
		return p;
}

void ArenaFree(arena_t* arena) {
		if (!arena) return;
		free(arena->data);
		free(arena);
}
