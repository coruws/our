#include "loader32.h"

typedef BOOL (WINAPI *tDllMain)(_In_  HINSTANCE hinstDLL,_In_  DWORD fdwReason,_In_  LPVOID lpvReserved);
bool get_image_nt_header32(cmem &f,IMAGE_NT_HEADERS32 *&nt);

bool get_entry_point32(BYTE *base,DWORD size,void* &ep){
	if (size<0x40) return false;
	BYTE *p=base;
	BYTE *pend=base+size;
	IMAGE_NT_HEADERS32 *nt=(IMAGE_NT_HEADERS32 *)(base+*(DWORD*)&base[0x3C]);
	if (pend<((BYTE*)nt+sizeof(IMAGE_NT_HEADERS32)))
		return false;
	ep=base+nt->OptionalHeader.AddressOfEntryPoint;
	return true;
}

bool get_proc_address32(BYTE *base,DWORD size,char *name,void* &addr){
	if (size<0x40) return false;
	BYTE *p=base;
	BYTE *pend=base+size;
	IMAGE_NT_HEADERS32 *nt=(IMAGE_NT_HEADERS32 *)(base+*(DWORD*)&base[0x3C]);
	if (pend<((BYTE*)nt+sizeof(IMAGE_NT_HEADERS32)))
		return false;
	if (!nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress ||
		!nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
		return false;
	IMAGE_EXPORT_DIRECTORY *ed=(IMAGE_EXPORT_DIRECTORY *)(base+nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if (pend<((BYTE*)ed+nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size))
		return false;
	if (pend<((BYTE*)ed+sizeof(IMAGE_EXPORT_DIRECTORY)))
		return false;
	DWORD *tn=(DWORD*)(base+ed->AddressOfNames);
	if (pend<((BYTE*)tn+sizeof(DWORD)*ed->NumberOfFunctions))
		return false;
	WORD *to=(WORD*)(base+ed->AddressOfNameOrdinals);
	if (pend<((BYTE*)to+sizeof(WORD)*ed->NumberOfFunctions))
		return false;
	DWORD *ta=(DWORD*)(base+ed->AddressOfFunctions);
	if (pend<((BYTE*)ta+sizeof(DWORD)*ed->NumberOfFunctions))
		return false;
	for (unsigned int i=0;i<ed->NumberOfFunctions;i++){
		if (tn[i]){
			char *pname=(char*)(base+tn[i]);
			if (pend<(BYTE*)pname){
				return false;
			}
			if (strcmp(pname,name)==0){
				if (pend<((BYTE*)ta+to[i]))
					return false;
				addr=base+ta[to[i]];
				if (pend<(BYTE*)addr)
					return false;
				return true;
			}
		}
	}
	return false;
}

bool get_image_nt_header32(cmem &f,IMAGE_NT_HEADERS32 *&nt){
	if (f.size<0x40) return false;
	BYTE *p=f.data;
	BYTE *pend=f.data+f.size;
	if (*(WORD*)p!='ZM') return false;
	nt=(IMAGE_NT_HEADERS32 *)(p+*(DWORD*)&p[0x3C]);
	if (pend<((BYTE*)nt+sizeof(IMAGE_NT_HEADERS32))) return false;
	if (nt->Signature!='EP') return false;
	if (nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386) return false;
	return true;
}

bool loadlibrary_x32(cmem &f,BYTE *&pp,DWORD &size){
	if (f.size<0x40) return false;
	BYTE *p=f.data;
	BYTE *pend=f.data+f.size;
	IMAGE_NT_HEADERS32 *nt;
	if (!get_image_nt_header32(f,nt)) return false;

	IMAGE_SECTION_HEADER *sec=(IMAGE_SECTION_HEADER *)((BYTE*)nt+nt->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(nt->Signature));
	if (pend<(BYTE*)(sec+nt->FileHeader.NumberOfSections)) return false;
	BYTE *base=(BYTE*)VirtualAlloc(0,nt->OptionalHeader.SizeOfImage,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (!base)
		return false;
	BYTE *baseend=base+nt->OptionalHeader.SizeOfImage;

	if (f.size<nt->OptionalHeader.SizeOfHeaders){
		VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
		return false;
	}
	if (baseend<(base+nt->OptionalHeader.SizeOfHeaders)){
		VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
		return false;
	}
	memcpy(base,p,nt->OptionalHeader.SizeOfHeaders);
	for (int i=0;i<nt->FileHeader.NumberOfSections;i++){
		if (baseend<(base+sec[i].VirtualAddress+sec[i].Misc.VirtualSize)){
			VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
			return false;
		}
		if (pend<(p+sec[i].PointerToRawData+sec[i].SizeOfRawData)){
			VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
			return false;
		}
		memcpy(base+sec[i].VirtualAddress,p+sec[i].PointerToRawData,sec[i].SizeOfRawData);
	}
	
	if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress && nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size){

		BYTE *reloc = base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		if (baseend<(reloc + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)){
			VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
			return false;
		}

		DWORD delta = (DWORD)base - nt->OptionalHeader.ImageBase;
		IMAGE_BASE_RELOCATION *rb = (IMAGE_BASE_RELOCATION *)reloc;
		BYTE *relocend = (BYTE *)rb + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		while (relocend>(BYTE*)rb){
			if (baseend < ((BYTE*)rb + sizeof(IMAGE_BASE_RELOCATION))){
				VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}

			if (baseend < ((BYTE*)rb + rb->SizeOfBlock)){
				VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
				return false;
			}
			int n = (rb->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
			WORD *ro = (WORD*)((BYTE*)rb + sizeof(IMAGE_BASE_RELOCATION));
			for (int i = 0; i < n; i++){
				if (ro[i] == 0)
					continue;
				if ((ro[i] & 0x3000) != 0x3000){
					VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				DWORD *o = (DWORD*)(base + rb->VirtualAddress + (WORD)(ro[i] & 0xFFF));
				if (baseend < (BYTE*)o){
					VirtualFree(base, nt->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
					return false;
				}
				*o = *o + delta;
			}
			rb = (IMAGE_BASE_RELOCATION *)((BYTE*)rb + rb->SizeOfBlock);
		}
	}

	if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress &&
		nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size){
		IMAGE_IMPORT_DESCRIPTOR *imp=(IMAGE_IMPORT_DESCRIPTOR *)(base+nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		if (baseend<(BYTE*)(imp+sizeof(IMAGE_IMPORT_DESCRIPTOR))){
			VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
			return false;
		}
		while(imp->Name!=0){
			char *lib=(char*)(base+imp->Name);
			if (baseend<(BYTE*)lib){
				VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
				return false;
			}
			HMODULE hm=LoadLibraryA(lib);
			if (!hm){
				VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
				return false;
			}
			DWORD *FirstThunk=(DWORD*)(base+imp->FirstThunk);
			DWORD *OriginalFirstThunk=(DWORD*)(base+imp->OriginalFirstThunk);
			if (!imp->OriginalFirstThunk) 
				OriginalFirstThunk=(DWORD*)(base+imp->FirstThunk);
			
			if (baseend<(BYTE*)FirstThunk){
				VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
				return false;
			}

			if (baseend<(BYTE*)OriginalFirstThunk){
				VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
				return false;
			}

			while(true){
				if (baseend<(BYTE*)(OriginalFirstThunk+1)){
					VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
					return false;
				}

				if (baseend<(BYTE*)(FirstThunk+1)){
					VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
					return false;
				}

				if (*OriginalFirstThunk==0 &&  *FirstThunk==0){
					break;
				}

				if (baseend<(BYTE*)(FirstThunk+1)){
					VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
					return false;
				}
				char *name;
				if (((*OriginalFirstThunk)&0x80000000)==0x80000000){
					name=(char*)(*OriginalFirstThunk&0x7FFFFFFF);
				}else{
					name=(char*)(base+*OriginalFirstThunk+2);
					if (baseend<(BYTE*)name){
						VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
						return false;
					}
				}
				DWORD f=(DWORD)GetProcAddress(hm,name);
				if (!f){
					VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
					return false;
				}
				*FirstThunk=(DWORD)f;
				OriginalFirstThunk++;
				FirstThunk++;
			}

			imp++;
		}

	}
	if (nt->OptionalHeader.AddressOfEntryPoint){
		if ((nt->FileHeader.Characteristics&IMAGE_FILE_DLL)==IMAGE_FILE_DLL){
			tDllMain f=(tDllMain)(base+nt->OptionalHeader.AddressOfEntryPoint);
			if (baseend<(BYTE*)f){
				VirtualFree(base,nt->OptionalHeader.SizeOfImage,MEM_DECOMMIT);
				return false;
			}
			f((HINSTANCE)base,DLL_PROCESS_ATTACH,0);
		}
	}
	pp=base;
	size=nt->OptionalHeader.SizeOfImage;
	return true;
}