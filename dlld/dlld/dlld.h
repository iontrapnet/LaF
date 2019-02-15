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
typedef uint32 ptr_t;
#define DLL __declspec(dllexport)
#else
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint64 ptr_t;
#define DLL
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

extern FILE* stdlog;
//#define LOG(...) fprintf(stdlog,__VA_ARGS__)

#define NEXT(buf,x) buf += sizeof(x)

#define NEXT_STR(buf,x) buf += ((x) ? strlen(x) : 0) + 1

#define NEXT_MEM(buf,x,n) buf += sizeof(*(x))*(n)

#define PUSH(buf,x) memcpy(buf,&(x),sizeof(x)); NEXT(buf,x)

#define PUSH_STR(buf,x) strcpy((char*)buf,(x) ? (x) : ""); NEXT_STR(buf,x)

#define PUSH_MEM(buf,x,n) memcpy(buf,x,sizeof(*x)*(n)); NEXT_MEM(buf,x,n)

#define POP(buf,x) memcpy(&(x),buf,sizeof(x)); NEXT(buf,x)

#define POP_STR(buf,x) strcpy(x,(const char*)buf); NEXT_STR(buf,x)

#define POP_MEM(buf,x,n) memcpy(x,buf,sizeof(*(x))*(n)); NEXT_MEM(buf,x,n)

API void dlld_init(int,int);
API void dlld_exit();
API void* dlld_funcs[256];
API byte* dlld_call(byte*,int32*);

//#define DLLD_CALLF
#ifdef DLLD_CALLF
API int dlld_callf(ptr_t,const char*,...);
#endif