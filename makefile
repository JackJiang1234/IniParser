all:
	gcc -g -Wall -DINI_PARSER_TEST ini_parser.c -o ini_parser_test
clean:
	rm -f *test
