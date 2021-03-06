#pragma once
#include "common.h"

class cmem{
public:
	uint8* data;
	uint32 size;
	bool realloc(int size);
	bool push(void *p,int size);
	bool push(uint64 v);
	bool push(uint32 v);
	bool push(uint16 v);
	bool push(uint8 v);
	bool push(cmem &v);
	void free();
	cmem(const cmem & v);
	void operator =(cmem &v);
	cmem operator+(cmem &v);
	cmem();
	cmem(void *data,int size);
	~cmem();
	bool pop(void *in,uint32 size);
	bool pop(cmem &in, uint32 size);
	bool pop(uint32 &v);
	bool pop(uint8 &v);
	bool pop(uint16 &v);
	bool pop(uint64 &v);
};
