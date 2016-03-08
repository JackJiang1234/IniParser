/*************************************************************************
	> File Name: ini_parser.c
	> Author: jack
	> Mail: jjy_8888@163.com 
	> Brief: the parser impl 
	> Created Time: Tue 08 Mar 2016 02:24:31 AM EST
 ************************************************************************/
#include<stdio.h>
#include "ini_parser.h"

typedef struct _Entry
{
	char* key;
	char* val;
	struct _Entry* next;
}Entry;

typedef struct _Section
{
	char* sname;
	Entry* first;	
	_Section* next;
}Section;

struct _IniParser
{
	char* content;
	Section* first;
}

IniParser* ini_parser_new(char* content)
{
	return_val_if_fail(content != NULL, NULL);

	IniParser* thiz = malloc(sizeof(IniParser));
	
	if (thiz != NULL)
	{
		thiz->first = NULL;
		thiz->content = content;
	}

	return thiz;
}

static void ini_parser_section_destroy(Section* sec)
{
	Entry* iter = sec->first;
	Entry* next = NULL;
	while(iter != NULL)
	{
		next = iter->next;
		
		SAFE_FREE(iter->key);	
		SAFE_FREE(iter->val);
		free(iter);

		iter = next;
	}

	free(sec);

	return;
}

void ini_parser_destroy(IniParser* thiz)
{
	if (thiz != NULL)
	{
		Section* iter = thiz->first;
		Section* next = NULL;
		while(iter != NULL)
		{
			next = iter->next;
			ini_parser_section_destroy(iter);
			iter = next;	
		}
		thiz->first = NULL;
		free(thiz);
	}

	return;
}
