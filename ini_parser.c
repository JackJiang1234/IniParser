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

typedef struct _Group
{
	char* sname;
	Entry* first;	
	struct _Group* next;
}Group;

struct _IniParser
{
	char* ini;
	char* comment;
	char* delim;
	Group* first;
};

IniParser* ini_parser_create(char* comment, char* delim)
{
	IniParser* thiz = (IniParser*)malloc(sizeof(IniParser));
	
	if (thiz != NULL)
	{
		thiz->comment = comment == NULL ? ";" : comment;
		thiz->delim = delim == NULL ? "=" : delim;
		thiz->first = NULL;
	}

	return thiz;
}

Ret ini_parser_parse(IniParser* thiz, char* ini)
{
	return_val_if_fail((ini != NULL) && (thiz != NULL) , RET_INVALID_PARAMS);

	enum _State
	{
		STAT_NONE = 0,
		STAT_GROUP,
		STAT_KEY,
		STAT_VAL,
		STAT_COMMENT
	}state = STAT_NONE;
	thiz->ini = ini;
	char* p = ini;
	char* group_start = NULL;
	char* key_start = NULL;
	char* val_start = NULL;

	while(*p != '\0')
	{
		switch(state)
		{

		}
	}
	
	return RET_OK;
}

Ret ini_parser_get_by_key(IniParser* thiz, char* section, char* key, char** val)
{
	return RET_OK;
}

static void ini_parser_group_destroy(Group* group)
{
	Entry* iter = group->first;
	Entry* next = NULL;
	while(iter != NULL)
	{
		next = iter->next;
		
		SAFE_FREE(iter->key);	
		SAFE_FREE(iter->val);
		free(iter);

		iter = next;
	}

	free(group);

	return;
}

void ini_parser_destroy(IniParser* thiz)
{
	if (thiz != NULL)
	{
		Group* iter = thiz->first;
		Group* next = NULL;
		while(iter != NULL)
		{
			next = iter->next;
			ini_parser_group_destroy(iter);
			iter = next;	
		}
		thiz->first = NULL;
		free(thiz);
	}

	return;
}
