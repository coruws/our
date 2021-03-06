#pragma once
#include <Windows.h>
#include "common.h"
#include "mem.h"

bool loadlibrary_x32(cmem &f,BYTE *&pp,DWORD &size);
bool get_image_nt_header32(cmem &f, IMAGE_NT_HEADERS32 *&nt);
bool get_proc_address32(BYTE *base,DWORD size,char *name,void* &addr);
bool get_entry_point32(BYTE *base,DWORD size,void* &ep);