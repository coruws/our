#include <Windows.h>
#include "common.h"
#include "ldasm.h"
#include "morf.h"
#include "crc32.h"


uint32 rnd(uint32 a){
	uint32 r=crc32(0,(uint8*)&a,4);
	return r;
}

uint32 rnd(){
	uint32 r=0;
	_asm {
		pushad
		rdtsc
		mov r,eax
		popad
	}
	r=rnd(r);
	return r;
}


void rnd(uint8 *data,int size){
	uint32 key=rnd();
	int j=0;
	for (int i=0;i<size;i++){
		if (j==0) key=rnd(key);
		data[i]=((uint8*)&key)[j];
		j++;
		j=j%4;
	}
}

