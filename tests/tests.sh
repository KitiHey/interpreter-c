tests=(
	lexer/nothing.c 
	lexer/ints.c
	lexer/keywords.c
)
CODE=$1
HEADERS=$2

for i in ${tests[@]}; do 
	echo "=== Testing" ${i} "==="
	gcc tests/${i} ${CODE} -o test.out ${HEADERS} -DALLOW_TESTS
	./test.out
	rm test.out
done
