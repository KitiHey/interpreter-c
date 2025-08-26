#include "arena.h"
#include <string.h>
#include "tools.h"

char* Strdup(arena_t* arena, char* str) {
		char* newstr = ArenaPush(arena, (size_t) (strlen(str)+1)*sizeof(char));
		if (newstr == NULL) return NULL;
		strcpy(newstr, str);
		return newstr;
}
