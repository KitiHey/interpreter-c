SRC=$(wildcard src/*.c) $(wildcard src/parser/*.c)
TEST=sh tests/tests.sh
CFLAGS=-Iinclude
BUILD_DIR=build/src/parser

BIN_FILE=interpreter
OBJ_FILES=$(SRC:%.c=build/%.o)

build/%.o: %.c
	mkdir -p $(dir $@)
	gcc -c $< -o $@ $(CFLAGS) -DALLOW_TESTS

$(BIN_FILE):
	gcc $(SRC) -o $@ $(CFLAGS)

build: $(BIN_FILE)
	echo "All Built!"

test: $(OBJ_FILES)
	$(TEST) $^ $(CFLAGS)
	@echo "All Tested!"

clean:
	@if [[ -d "build/" ]]; then rm -r build/; fi
	@if [[ -f "$(BIN_FILE)" ]]; then rm $(BIN_FILE); fi
	@echo "All Cleaned!"
