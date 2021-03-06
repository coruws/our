#include <Windows.h>
#include "reg.h"


bool creg::set(HKEY key,wchar_t* subkey,wchar_t *name,uint8* data,int size){
	bool r=false;
	HKEY hkey=0;
	DWORD keyinf=0;
	if (RegCreateKeyExW(key,subkey,0,0,0,KEY_ALL_ACCESS ,0,&hkey,&keyinf)==ERROR_SUCCESS){
		if (RegSetValueExW(hkey, name, 0, REG_BINARY, (unsigned char*)data, size) == ERROR_SUCCESS){
			r = true;
		}
		RegCloseKey(hkey);
	}
	return r;
}

bool creg::set(HKEY key, wchar_t* subkey, wchar_t *name, wchar_t *v){
	bool r = false;
	HKEY hkey = 0;
	DWORD keyinf = 0;
	if (RegCreateKeyExW(key, subkey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkey, &keyinf) == ERROR_SUCCESS){
		if (RegSetValueExW(hkey, name, 0, REG_SZ,(BYTE*)v, wcslen(v)*2) == ERROR_SUCCESS){
			r = true;
		}
		RegCloseKey(hkey);
	}
	return r;
}


bool creg::get(HKEY key,wchar_t *subkey,wchar_t *name,uint8 *data,int *size){
	bool r=false;
	HKEY hkey;
	if (RegOpenKeyExW(key, subkey, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS){
		if (data){
			if (RegQueryValueExW(hkey, name, 0, 0, data, (DWORD*)size) == ERROR_SUCCESS) r = true;
		}
		else{
			if (RegQueryValueExW(hkey, name, 0, 0, 0, (DWORD*)size) == ERROR_SUCCESS) r = true;
		}
		RegCloseKey(hkey);
	}
	r=true;
	return r;
}