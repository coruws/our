#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"

#if _DEBUG


void appendlog(char *fname,char *buf){
	HANDLE hf=CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);
	if (hf != INVALID_HANDLE_VALUE){
		DWORD a = 0;
		SetFilePointer(hf,0,0, FILE_END);
		WriteFile(hf, buf,strlen(buf), &a, 0);
		FlushFileBuffers(hf);
		CloseHandle(hf);
	}
}

void appendlog(wchar_t *fname, wchar_t *buf){
	HANDLE hf = CreateFileW(fname, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);
	if (hf != INVALID_HANDLE_VALUE){
		DWORD a = 0;
		SetFilePointer(hf,0,0, FILE_END);
		WriteFile(hf, buf, wcslen(buf)*2, &a, 0);
		FlushFileBuffers(hf);
		CloseHandle(hf);
	}
}


int logfilter = VTRACE;
const wchar_t *wdbgmsg[] = { L"ERROR", L"WARNING", L"INFO",L"DEBUG",L"TRACE" };
const char *dbgmsg[] = {"ERROR", "WARNING", "INFO", "DEBUG", "TRACE" };
void dbg(int loglevel,char *fmt,...){
	if (loglevel > logfilter)
		return;
	SYSTEMTIME st;
	GetSystemTime(&st);
	char buf[1024],tfmt[1024];
	_snprintf(tfmt,sizeof(tfmt), "%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d <%s> %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, dbgmsg[loglevel], fmt);
	va_list list;
	va_start(list, fmt);
	_vsnprintf(buf, sizeof(buf)-2, tfmt, list);
	//DWORD rw;
	//WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)strlen(buf), &rw, 0);
	OutputDebugStringA(buf);
	appendlog("C:\\test\\vx.log", buf);
	va_end(list);
}

void dbg(int loglevel, wchar_t *fmt, ...){
	if (loglevel > logfilter)
		return;
	SYSTEMTIME st;
	GetSystemTime(&st);
	wchar_t buf[1024], tfmt[1024];
	_snwprintf(tfmt, sizeof(tfmt), L"%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d <%s> %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, wdbgmsg[loglevel], fmt);
	va_list list;
	va_start(list, fmt);
	_vsnwprintf(buf, sizeof(buf)-1, tfmt, list);
	//DWORD rw;
	//WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)strlen(buf), &rw, 0);
	OutputDebugStringW(buf);
	appendlog(L"c:\\test\\vxw.log", buf);
	va_end(list);
}

#endif
