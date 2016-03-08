#ifndef _INI_PARSE_H
#define _INI_PARSE_H

struct  _IniParser;
typedef struct _IniParser IniParser;
typedef enum _Ret
{
	SECTION_NOT_EXISTS,
	KEY_NOT_EXISTS,
} Ret;
typedef void(*EntryVisitFunc)(char* section, char* key, char* val);

IniParser* ini_parser_new(char* fname);
Ret ini_parser_get_by_key(IniParser* thiz, char* section, char* key, char** val);
R ini_parser_foreach(IniParser* thiz, EntryVisitFunc* visit);
void ini_parser_destroy(IniParser* thiz);

#endif
