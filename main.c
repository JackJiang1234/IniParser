/*************************************************************************
	> File Name: main.c
	> Author: jack
	> Mail: jjy_8888@163.com 
	> Brief: 
	> Created Time: Fri 18 Mar 2016 10:32:34 AM EDT
 ************************************************************************/
#include<stdio.h>
#include "ini_parser.h"

int main(void)
{
	char* ini = "[group]\nkey=value";
	IniParser* parser = ini_parser_create(NUL, NUL);
	ini_parser_parse(parser, ini);
	ini_parser_destroy(parser);

	return 0;
}
