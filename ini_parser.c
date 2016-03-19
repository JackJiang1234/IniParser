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
#include<assert.h>
#include "ini_parser.h"

typedef struct _Entry
{
	const char* key;
	const char* val;
	struct _Entry* next;
}Entry;

typedef struct _Group
{
	const char* name;
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
		thiz->comment = comment == NUL ? ';' : comment;
		thiz->delim = delim == NUL ? '=' : delim;
		thiz->first = NULL;
		thiz->ini = NULL;
	}

	return thiz;
}

static const char* strtrim(char* str)
{
	char* p = str + strlen(str) - 1;
	while(p != str && isspace(*p))
	{
		*p = '\0';
		p--;
	}

	p = str;
	while(*p != '\0' && isspace(*p))
	{
		p++;
	}

	if (p != str)
	{
		char* d = str;
		char* s = p;
		while(*s != '\0')
		{
			*d = *s;
			s++;
			d++;
		}
		*d = '\0';
	}

	return str;
}

static Ret on_group(IniParser* thiz, const char* gname)
{
	Group* g = (Group*)malloc(sizeof(Group));
	if (g == NULL)
	{
		return RET_OOM;
	}
	g->name = gname;
	g->first = NULL;
	g->next = NULL;
	
	if (thiz->first == NULL)
	{
		thiz->first = g;
	}
	else 
	{
		Group* p = thiz->first;
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = g;
	}

	return RET_OK;
}

static Group* ini_parser_find_group(IniParser* thiz, const char* gname)
{
	Group* fg = thiz->first;
	while(strcmp(fg->name, gname) != 0)
	{
		fg = fg->next;
	}
	assert(fg != NULL);
	
	return fg;
}

static Ret on_entry(IniParser* thiz, const char* g, const char* key, const char* val)
{
	Ret ret = RET_OK;
	if (thiz->first == NULL)
	{
		ret = on_group(thiz, g);
		return_val_if_fail(ret == RET_OK, ret);
	}

	Group* fg = ini_parser_find_group(thiz, g);
	
	Entry* new_entry = (Entry*)malloc(sizeof(Entry));
	if (new_entry == NULL)
	{
		return RET_OOM;
	}
	new_entry->key = key;
	new_entry->val = val;
	
	if (fg->first == NULL)
	{
		fg->first = new_entry;
	}
	else 
	{
		Entry* fe = fg->first;
		while(fe->next != NULL)
		{
			fe = fe->next;
		}
		fe->next = new_entry;
	}

	return ret;
}

Ret ini_parser_parse(IniParser* thiz, const char* ini)
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
	Ret ret = RET_OK;
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
					key_start = p;
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
					//printf("[%s]\n", group_start);
					ret = on_group(thiz, group_start);
					if (ret != RET_OK)
					{
						return ret;
					}
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
					//printf("%s=%s\n", key_start, val_start);
					ret = on_entry(thiz, group_start, key_start, val_start);
					if (ret != RET_OK)
					{
						return ret;
					}
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
		//printf("%s=%s\n", key_start, val_start);
		ret = on_entry(thiz, group_start, key_start, val_start);
		if (ret != RET_OK)
		{
			return ret;
		}
	}
	
	return ret;
}

Ret ini_parser_get_by_key(IniParser* thiz, const char* group, const char* key, const char** val)
{
	return_val_if_fail((thiz != NULL) && (key != NULL), RET_INVALID_PARAMS);	
	
	if (thiz->first == NULL)
	{
		return RET_FAIL;
	}
	
	Group* fg = ini_parser_find_group(thiz, group);
	Entry* fe = fg->first;
	while(fe != NULL)
	{
		if (strcmp(fe->key, key) == 0)
		{
			*val = fe->val;
			return RET_OK;
		}
		fe = fe->next;
	}

	return RET_FAIL;
}

Ret ini_parser_foreach(IniParser* thiz, void* ctx, EntryVisitFunc visit)
{
	return_val_if_fail((thiz != NULL) && (visit != NULL), RET_INVALID_PARAMS);
	
	Ret ret = RET_OK;
	Group* g_iter = thiz->first;
	while(g_iter != NULL)
	{
		Entry* e_iter = g_iter->first;
		while(e_iter != NULL)
		{
			ret = visit(ctx, g_iter->name, e_iter->key, e_iter->val);
			if (ret == RET_STOP)
			{
				return ret;
			}
			e_iter = e_iter->next;
		}
		g_iter = g_iter->next;
	}

	return ret;
}

static void ini_parser_group_destroy(Group* group)
{
	Entry* iter = group->first;
	Entry* next = NULL;
	while(iter != NULL)
	{
		next = iter->next;
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

#ifdef INI_PARSER_TEST

Ret print(void* ctx, const char* gname, const char* key, const char* val)
{
	printf("[%s] %s=%s**\n", gname, key, val);
	return RET_OK;
}

int main(void)
{
	char* ini = "[group]\nkey=value\nkey1 = value1\n[group2]\nkey3=value3";
	IniParser* thiz = ini_parser_create(NUL, NUL);
	ini_parser_parse(thiz, ini);
	ini_parser_foreach(thiz, NULL, print);
	
	const char* val = NULL;
	ini_parser_get_by_key(thiz, "group", "key", &val);
	assert(strcmp(val, "value") == 0);	

	ini_parser_get_by_key(thiz, "group", "key1", &val);
	assert(strcmp(val, "value1") == 0);	

	ini_parser_get_by_key(thiz, "group2", "key3", &val);
	assert(strcmp(val, "value3") == 0);	

	ini_parser_destroy(thiz);

	return 0;
}

#endif
