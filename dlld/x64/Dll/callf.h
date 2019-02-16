#pragma once

#define byte unsigned char
#define real32 float
#define real64 double

#ifdef _MSVC
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

#ifdef _WIN32
#define DLL __declspec(dllexport)
#ifdef _WIN64
#define ptr_t uint64
#else
#define ptr_t uint32
#endif
#else
#define DLL
#define ptr_t uint64
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

//extern FILE* stdlog;
//#define LOG(...) fprintf(stdlog,__VA_ARGS__)

void dlld_init(int,int);
void dlld_exit();
void* dlld_funcs[256];
byte* dlld_call(byte*,int32*);
int dlld_callf(ptr_t,const char*,...);

int dllc_init(const char*,int);
void dllc_exit();
int dllc_open(const char*,int);
void dllc_close();
int dllc_write(const char*,int);
int dllc_read(char*,int);
int dllc_load(const char*);
int dllc_symbol(int,const char*);
double dllc_doublef(int,double);
int dllc_callf(int,const char*,...);