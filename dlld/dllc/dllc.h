#pragma once

#define byte unsigned char
#define real32 float
#define real64 double

#ifdef _WIN32
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#define DLL __declspec(dllexport)
#else
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
#define DLL
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

extern FILE* stdlog;
#define LOG(...) fprintf(stdlog,__VA_ARGS__)

#define NEXT(buf,x) buf+=sizeof(x)

#define NEXT_STR(buf,x) buf+=((x)?strlen(x):0)+1

#define NEXT_MEM(buf,x,n) buf+=sizeof(*(x))*(n)

#define PUSH(buf,x) memcpy(buf,&(x),sizeof(x));NEXT(buf,x)

#define PUSH_STR(buf,x) strcpy((char*)buf,(x)?(x):"");NEXT_STR(buf,x)

#define PUSH_MEM(buf,x,n) memcpy(buf,x,sizeof(*x)*(n));NEXT_MEM(buf,x,n)

#define POP(buf,x) memcpy(&(x),buf,sizeof(x));NEXT(buf,x)

#define POP_STR(buf,x)\
	if(!(x)){(x)=(char*)malloc(strlen((const char*)buf));}\
	strcpy(x,(const char*)buf);NEXT_STR(buf,x)

#define POP_MEM(buf,x,n) memcpy(x,buf,sizeof(*(x))*(n));NEXT_MEM(buf,x,n)

#define RET(x) if(!dllc_read((char*)&(x),sizeof(x)))return 0

#define RET_MEM(x,n) if(!dllc_read((char*)(x),sizeof(*(x))*(n)))return 0

#define FUNC(buf,size,func,head)\
	buf=(byte*)malloc(sizeof(size)+size);\
	PUSH(buf,size);\
	PUSH(buf,func);\
	PUSH(buf,head)

#define CALL(buf,size)\
	buf-=size;lograw(": >",buf,size);\
	dllc_write((const char*)(buf-=sizeof(size)),sizeof(size)+size);\
	free(buf);if (!dllc_print())return 0;\
	RET(size);lograw(": <",0,size)

#define CALL_RET(buf,isize,osize)\
	buf-=isize;lograw(": >",buf,isize);\
	dllc_write((const char*)(buf-=sizeof(isize)),sizeof(isize)+isize);\
	if (!dllc_print())return 0;RET(osize);\
	if(osize>isize) {free(buf);buf=(byte*)malloc(osize);}\
	RET_MEM(buf,osize);lograw(": <",buf,osize)

#define ptr_t int32

API int dllc_init(const char*,int);
API void dllc_exit();
API int dllc_open(const char*,int);
API void dllc_close();
API int dllc_print();
API int dllc_write(const char*,int);
API int dllc_read(char*,int);
API ptr_t dllc_load(const char*);
API ptr_t dllc_symbol(ptr_t,const char*);
API double dllc_doublef(ptr_t,double);
API int dllc_callf(ptr_t,const char*,...);
