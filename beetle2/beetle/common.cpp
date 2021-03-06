#include "common.h"


void * __cdecl operator new(size_t n) { 
	return (void *)HeapAlloc(GetProcessHeap(), 0, n); 
}

void __cdecl operator delete(void * p){ 
	if (p!=NULL) HeapFree(GetProcessHeap(), 0, p); 
}

void * __cdecl malloc(_In_ size_t _Size){
	return HeapAlloc(GetProcessHeap(),0,_Size);
}

void   __cdecl free(_Pre_maybenull_ _Post_invalid_ void * p){
	HeapFree(GetProcessHeap(),0,p);
}

void * __cdecl realloc(_Post_ptr_invalid_ void * _Memory, _In_ size_t _NewSize){
	return HeapReAlloc(GetProcessHeap(),0,_Memory,_NewSize);
}

void __cdecl exit(_In_ int _Code){
	ExitProcess(0);
}
