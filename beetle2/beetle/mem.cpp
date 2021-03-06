#include "mem.h"
#include "dbg.h"

cmem::cmem(void *data,int size){
	if (!data || !size)
		return;
	this->data=0;
	this->size=0;
	push(data,size);	
}

void cmem::free(){
	if (size){
		::free(data);
		size=0;
	}
}
void cmem::operator =(cmem &w){
	free();
	this->data=(uint8*)malloc(w.size);
	if (!this->data){
		dbg(VERROR, "cmem::operator =(cmem &w) %d",w.size);
		exit(1);
	}

	this->size=w.size;
	memcpy(this->data,w.data,w.size);
}

bool cmem::push(cmem &v){
	return push(v.data,v.size);
}

cmem cmem::operator+(cmem &v){
	cmem a;
	a.push(*this);
	a.push(v);
	return a;
}

cmem::cmem(const cmem & w){
	this->data=(uint8*)malloc(w.size);
	if (!this->data){
		dbg(VERROR, "cmem::cmem(const cmem & w) %d",w.size);
		exit(1);
	}
	this->size=w.size;
	memcpy(this->data,w.data,w.size);
}

bool cmem::realloc(int size){
	bool r=true;
	if (!size){
		free();
		return r;
	}

	if (this->size>0){
		data=(uint8*)::realloc(data,size);
		if (!data){
			dbg(VERROR, "cmem::realloc(int size) realloc size %d",size);
			exit(1);
		}

		this->size=size;
	}else{
		data=(uint8*)::malloc(size);
		if (!data){
			dbg(VERROR, "cmem::realloc(int size) malloc size %d", size);
			exit(1);
		}
		this->size=size;
	}
	
	if (!data){
		r=false;
		this->size=0;
	}
	return r;
}

bool cmem::push(void *p,int size){
	bool r=false;
	if (!size)
		return true;
	realloc(this->size + size);
	memcpy(&data[this->size-size],p,size);
	r=true;
	return r;
}

bool cmem::push(uint32 v){
	push(&v, 4);
	return true;
}

bool cmem::push(uint64 v){
	push(&v, 8);
	return true;
}

bool cmem::push(uint16 v){
	push(&v, 2);
	return true;
}

bool cmem::push(uint8 v){
	push(&v, 1);
	return true;
}


cmem::cmem(){
	data=0;
	size=0;
}

cmem::~cmem(){
	free();
}

bool cmem::pop(void *in, uint32 size){
	if (this->size < size)
		return false;
	memcpy(in, data, size);
	memcpy(data, &data[size],this->size-size);
	realloc(this->size - size);
	return true;
}

bool cmem::pop(cmem &in, uint32 size){
	in.realloc(size);
	in.pop(in.data, size);
	return true;
}

bool cmem::pop(uint32 &v){
	pop(&v, 4);
	return true;
}

bool cmem::pop(uint16 &v){
	pop(&v, 2);
	return true;
}

bool cmem::pop(uint8 &v){
	pop(&v, 1);
	return true;
}

bool cmem::pop(uint64 &v){
	pop(&v, 8);
	return true;
}
