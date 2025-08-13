tests=(
	lexer/nothing.c 
	lexer/ints.c
	lexer/keywords.c
	parser/nothing.c
	parser/int.c
	parser/expressionStatements.c
)
ARGS=$@

for i in ${tests[@]}; do 
	echo "=== Testing" ${i} "==="
	gcc tests/${i} ${ARGS} -o test.out -DALLOW_TESTS
	./test.out
	rm test.out
done
