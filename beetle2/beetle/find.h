#pragma once

typedef DWORD (_stdcall *FUNC)(wchar_t *dir,DWORD attr,void *arg);
enum efind{
	FIND_NEXT = 1,
	FIND_SKIP = 0,
	FIND_EXIT = -1,
	FIND_ERROR = -2,
};

int find_dir(wchar_t *dir, wchar_t *m,FUNC f,void *arg);
int find_file(wchar_t *dir, wchar_t *m,FUNC f,void *arg);
bool DisableTrastedInstaller(wchar_t *fname);
bool WFPBypass(wchar_t *lpFileName);
bool delete_file(wchar_t *fname);
bool is_file_protected(wchar_t *fname);
void find_drive(wchar_t *mask,FUNC f,void *arg);