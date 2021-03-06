#include "file.h"

bool cfile::load(wchar_t *fname,cmem &mem,uint32 max){
	bool r=false;
	HANDLE hf=CreateFileW(fname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hf!=INVALID_HANDLE_VALUE){
		DWORD l = GetFileSize(hf, 0);
		if (l > 0){
			if (max == 0 || l<max){
				mem.realloc(l);
				DWORD rw;
				if (ReadFile(hf, mem.data, mem.size, &rw, 0) == TRUE){
					r = true;
				}
			}
		}
		CloseHandle(hf);
	}
	return r;
}

bool cfile::load(char *fname,cmem &mem,uint32 max){
	bool r = false;
	HANDLE hf = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf != INVALID_HANDLE_VALUE){
		DWORD l = GetFileSize(hf, 0);
		if (l > 0){
			if (max == 0 || l<max){
				mem.realloc(l);
				DWORD rw;
				if (ReadFile(hf, mem.data, mem.size, &rw, 0) == TRUE){
					r = true;
				}
			}
		}
		CloseHandle(hf);
	}
	return r;
}


bool cfile::save(wchar_t *fname,cmem &mem){
	bool r=false;
	if (!mem.size || !mem.data) return r;
	HANDLE hf=CreateFileW(fname,GENERIC_WRITE,FILE_SHARE_WRITE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hf != INVALID_HANDLE_VALUE){
		DWORD rw;
		if (WriteFile(hf, mem.data, mem.size, &rw, 0) == TRUE){
			r = true;
		}
		CloseHandle(hf);
	}
	return r;
}


bool cfile::save(char *fname,cmem &mem){
	bool r = false;
	if (!mem.size || !mem.data) return r;
	HANDLE hf = CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf != INVALID_HANDLE_VALUE){
		DWORD rw;
		if (WriteFile(hf, mem.data, mem.size, &rw, 0) == TRUE){
			r = true;
		}
		CloseHandle(hf);
	}
	return r;
}
