#pragma once
#include <Windows.h>
#include "common.h"

class creg{
public:
	static bool get(HKEY key,wchar_t *subkey,wchar_t *name,uint8 *data,int *size);
	static bool set(HKEY key,wchar_t* subkey,wchar_t *name,uint8* data,int size);
	static bool set(HKEY key, wchar_t* subkey, wchar_t *name, wchar_t *v);
};