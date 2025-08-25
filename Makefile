SRC=$(wildcard src/*.c)
TEST=sh tests/tests.sh
CFLAGS=-Iinclude

OBJECT_FILE=$(notdir $(SRC:.c=.o))
BIN_FILE=interpreter

$(BIN_FILE):
	gcc $(SRC) -o $@ $(CFLAGS)

%.o: src/%.c
	gcc -c $< -o $@ $(CFLAGS) -DALLOW_TESTS

build: $(BIN_FILE)
	@echo "All Built!"

test: $(OBJECT_FILE)
	@$(TEST) $^ $(CFLAGS) -DALLOW_TESTS
	@echo "All Tested!"

clean:
	@if [[ -f "lexer.o" ]]; then rm $(OBJECT_FILE); fi
	@if [[ -f "$(BIN_FILE)" ]]; then rm $(BIN_FILE); fi
	@echo "All Cleaned!"
