#pragma once
#include <Windows.h>
#include <stdio.h>


typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long long uint64;

void * __cdecl operator new(size_t n);
void __cdecl operator delete(void * p);

void * __cdecl malloc(_In_ size_t _Size);
void   __cdecl free(_Pre_maybenull_ _Post_invalid_ void * _Memory);
void * __cdecl realloc(_Post_ptr_invalid_ void * _Memory, _In_ size_t _NewSize);
void __cdecl exit(_In_ int _Code);

