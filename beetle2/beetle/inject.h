#pragma once
#include "mem.h"
bool inject_to_x32(cmem *in,DWORD pid);
bool inject(cmem &in);
DWORD get_pid_explorer();

BOOL setPrivilege(HANDLE hProc, LPCTSTR szPrivName, BOOL fEnable);