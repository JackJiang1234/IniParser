/* File: ini_parse.h
 * Author: jack
 * Email: jjy_8888@163.com
 * Brief: ini parser head file
 *
 * */
#ifndef _INI_PARSE_H
#define _INI_PARSE_H

#include <stdio.h>
#include <stdlib.h>

struct  _IniParser;
typedef struct _IniParser IniParser;
typedef enum _Ret
{
	RET_OK,
	RET_STOP,
	RET_INVALID_PARAMS,
	RET_FAIL
} Ret;

#define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n", \
			__func__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n", \
			__func__, __LINE__); return (ret);}

#define SAFE_FREE(p) if(p != NULL){free(p); p = NULL;}

typedef void(*EntryVisitFunc)(void* ctx, char* group, char* key, char* val);

IniParser* ini_parser_create(char* comment, char* delim);
Ret ini_parser_parse(IniParser* thiz, char* ini);
Ret ini_parser_get_by_key(IniParser* thiz, char* group, char* key, char** val);
Ret ini_parser_foreach(IniParser* thiz, EntryVisitFunc* visit);
void ini_parser_destroy(IniParser* thiz);

#endif
