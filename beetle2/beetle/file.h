#pragma once
#include <Windows.h>
#include "common.h"
#include "mem.h"


class cfile{
public:
	static bool load(wchar_t *fname,cmem &mem,uint32 max=0);
	static bool load(char *fname,cmem &mem,uint32 max=0);
	static bool save(wchar_t *fname,cmem &mem);
	static bool save(char *fname,cmem &mem);
};
