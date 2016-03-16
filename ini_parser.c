/*************************************************************************
	> File Name: ini_parser.c
	> Author: jack
	> Mail: jjy_8888@163.com 
	> Brief: the parser impl 
	> Created Time: Tue 08 Mar 2016 02:24:31 AM EST
 ************************************************************************/
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>
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
	char comment;
	char delim;
	Group* first;
	char* ini;
};

IniParser* ini_parser_create(char comment, char delim)
{
	IniParser* thiz = (IniParser*)malloc(sizeof(IniParser));
	
	if (thiz != NULL)
	{
		thiz->comment = comment == ' ' ? ';' : comment;
		thiz->delim = delim == ' ' ? '=' : delim;
		thiz->first = NULL;
		thiz->ini = NULL;
	}

	return thiz;
}

Ret ini_parser_parse(IniParser* thiz, char* ini)
{
	return_val_if_fail((ini != NULL) && (thiz != NULL) , RET_INVALID_PARAMS);

	thiz->ini = (char*)malloc(strlen(ini) + 1);
	strcpy(thiz->ini, ini);

	enum _State
	{
		STAT_NONE = 0,
		STAT_GROUP,
		STAT_KEY,
		STAT_VAL,
		STAT_COMMENT
	}state = STAT_NONE;
	char* p = thiz->ini;
	char* group_start = NULL;
	char* key_start = NULL;
	char* val_start = NULL;

	while(*p != '\0')
	{
		switch(state)
		{
			case STAT_NONE:
			{
				if (*p == '[')
				{
					state = STAT_GROUP;
					group_start = p + 1;
				}
				else if (*p == thiz->comment)
				{
					state = STAT_COMMENT;
				}
				else if (!isspace(*p))
				{
					state = STAT_KEY;
					key_start = p + 1;
				}
			}
			break;
			case STAT_GROUP:
			{
				if (*p == ']')
				{
					*p = '\0';
					state = STAT_NONE;
					strtrim(group_start);
					//add a new group
					printf("[%s]\n", group_start);
				}
			}
			break;
			case STAT_KEY:
			{
				if (*p == thiz->delim)
				{
					*p = '\0';
					state = STAT_VAL;
					val_start = p + 1;
				}
			}
			break;
			case STAT_VAL:
			{
				if (*p == '\n' || *p == '\r')
				{
					*p = '\0';
					state = STAT_NONE;
					strtrim(key_start);
					strtrim(val_start);
					//add a entry to a group
					printf("%s=%s\n", key_start, val_start);
				}
			}
			break;
			case STAT_COMMENT:
			{
				if (*p == '\n')
				{
					state = STAT_NONE;
				}
			}
			break;
			default:
			break;
		}

		p++;
	}

	if (state == STAT_VAL)
	{
		strtrim(key_start);
		strtrim(val_start);
		//add to group
		printf("%s=%s", key_start, val_start);
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
		free(thiz->ini);
		free(thiz);
	}

	return;
}
