#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

typedef unsigned char byte;
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#define real32 float
#define real64 double

static FILE *stdlog = 0;

//#define LOG(...) fprintf(stdlog,__VA_ARGS__)

#ifdef _WIN32
#define DLL __declspec(dllexport)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifdef LOG
static void logtime() {
	SYSTEMTIME t;
    //GetSystemTime(&t);
	GetLocalTime(&t);
    LOG("[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
		t.wYear, t.wMonth, t.wDay,
        t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
}
#else
#define logtime()
#endif
#ifdef _WIN64
typedef uint64 ptr_t;
#include "../64/include/dyncall.h"
#include "../64/include/dynload.h"
#pragma comment(lib,"../64/lib/libdyncall_s.lib")
#pragma comment(lib,"../64/lib/libdynload_s.lib")
#else
typedef uint32 ptr_t;
#include "../32/include/dyncall.h"
#include "../32/include/dynload.h"
#pragma comment(lib,"../32/lib/libdyncall_s.lib")
#pragma comment(lib,"../32/lib/libdynload_s.lib")
#endif
#else
#define DLL
static void logtime() {
	time_t now;
	struct tm *tm_now;
 
	time(&now);
	tm_now = localtime(&now);
 
    LOG("[%04d-%02d-%02d %02d:%02d:%02d]",
		1900+tm_now->tm_year, tm_now->tm_mon, tm_now->tm_mday,
		tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

#ifdef LOG
static void lograw(const char* prefix, byte* bytes, int size) {
	int i;
	logtime();
	LOG("%s[%d]",prefix,size);
	for (i = 0; i < size; ++i) LOG("%02X ", bytes[i]);
	LOG("\n");
}
#else
#define lograw(...)
#define LOG(...)
#endif

static DCCallVM *vm = 0;
API DCCallVM* dlld_init(int size, int mode) {
	stdlog = fopen("dlld.log","a"); setbuf(stdlog, NULL);
	vm = dcNewCallVM(size);
	dcMode(vm, mode);
	return vm;
}

API void dlld_exit() {
	fclose(stdlog);
	dcFree(vm);
}

API void* dlld_funcs[256] = {memcpy, dlLoadLibrary, dlFindSymbol, dlFreeLibrary};

API byte* dlld_call(byte *args, int *len) {
	static void *bufs[256];
	static int sizes[256];
	static char alloc[256];
    ptr_t func = *(ptr_t*)args;
	int argc = 1 + *(args += sizeof(ptr_t)), bufc = -1, odd = 0, type = 0, size = 0, i;
	byte *argv = (++args) + (argc >> 1) + (argc & 1), *ret = 0;

	logtime();
	LOG(": %p(",func);
	if (func < 256) func = (ptr_t)dlld_funcs[func];
	
	dcReset(vm);
 	while (--argc) {
#define TYPE_DECODE type = odd ? ((0xF0 & *args) >> 4) : (0xF & *args)
		TYPE_DECODE;
		LOG("[%d]",type);
		switch (type) {
		case 0x0:
			LOG("%c,",*(DCchar*)argv);
			dcArgChar(vm, *(DCchar*)argv); argv += 1;
			break;
		case 0x1:
			LOG("%hd,",*(DCshort*)argv);
			dcArgShort(vm, *(DCshort*)argv); argv += 2;
			break;
		case 0x2:
			LOG("%d,",*(DCint*)argv);
			dcArgInt(vm, *(DCint*)argv); argv += 4;
			break;
		case 0x3:
			LOG("%lld,",*(DClonglong*)argv);
			dcArgLongLong(vm, *(DClonglong*)argv); argv += 8;
			break;
		case 0x4:
			LOG("%f,",*(DCfloat*)argv);
			dcArgFloat(vm, *(DCfloat*)argv); argv += 4;
			break;
		case 0x5:
			LOG("%lf,",*(DCdouble*)argv);
			dcArgDouble(vm, *(DCdouble*)argv); argv += 8;
			break;
		case 0x6: // \0 terminated string
			LOG("\"%s\",",argv);
			dcArgPointer(vm, (DCpointer)argv); argv += strlen((const char*)argv) + 1;
			break;
		case 0x7: // byte[]: {in,out,in+out} x {size=1,size<256,other}
			dcArgPointer(vm, (DCpointer)argv); argv += 1;
			break;
		case 0x8:
			dcArgPointer(vm, (DCpointer)(argv + 1)); argv += 1 + *argv;
			break;
		case 0x9:
			dcArgPointer(vm, (DCpointer)(argv + 4)); argv += 4 + *(uint32*)argv;
			break;
		case 0xA:
			bufs[++bufc] = malloc(1); sizes[bufc] = 1; alloc[bufc] = 1;
			dcArgPointer(vm, (DCpointer)bufs[bufc]);
			break;
		case 0xB:
			bufs[++bufc] = malloc(*argv); sizes[bufc] = *argv; alloc[bufc] = 1;
			dcArgPointer(vm, (DCpointer)bufs[bufc]); argv += 1;
			break;
		case 0xC:
			size = *(uint32*)argv;
			bufs[++bufc] = malloc(size); sizes[bufc] = size; alloc[bufc] = 1;
			dcArgPointer(vm, (DCpointer)bufs[bufc]); argv += 4;
			break;
		case 0xD:
			bufs[++bufc] = argv; sizes[bufc] = 1; alloc[bufc] = 0;
			dcArgPointer(vm, (DCpointer)argv); argv += 1;
			break;
		case 0xE:
			bufs[++bufc] = argv + 1; sizes[bufc] = *argv; alloc[bufc] = 0;
			dcArgPointer(vm, (DCpointer)(argv + 1)); argv += 1 + *argv;
			break;
		case 0xF:
			size = *(uint32*)argv;
			bufs[++bufc] = argv + 4; sizes[bufc] = size; alloc[bufc] = 0;
			dcArgPointer(vm, (DCpointer)(argv + 4)); argv += 4 + size;
			break;
		}
		if (odd) ++args;
		odd = 1 - odd;
	}

	TYPE_DECODE;
	LOG(">[%d]",type);
	ret = (byte*)malloc(8); alloc[++bufc] = 1;
	switch (type) {
		case 0:
			*(DCchar*)ret = dcCallChar(vm, (DCpointer)func); size = 1;
			LOG("%c)\n",*(DCchar*)ret);
			break;
		case 1:
			*(DCshort*)ret = dcCallShort(vm, (DCpointer)func); size = 2;
			LOG("%hd)\n",*(DCshort*)ret);
			break;
		case 2:
			*(DCint*)ret = dcCallInt(vm, (DCpointer)func); size = 4;
			LOG("%d)\n",*(DCint*)ret);
			break;
		case 3:
			*(DClonglong*)ret = dcCallLongLong(vm, (DCpointer)func); size = 8;
			LOG("%lld)\n",*(DClonglong*)ret);
			break;
		case 4:
			*(DCfloat*)ret = dcCallFloat(vm, (DCpointer)func); size = 4;
			LOG("%f)\n",*(DCfloat*)ret);
			break;
		case 5:
			*(DCdouble*)ret = dcCallDouble(vm, (DCpointer)func); size = 8;
			LOG("%lf)\n",*(DCdouble*)ret);
			break;
		case 6:
			free(ret); alloc[bufc] = 0;
			ret = (byte*)dcCallPointer(vm, (DCpointer)func);
			size = strlen((const char*)ret) + 1;
			LOG("\"%s\")\n",(const char*)ret);
			break;
	}

	bufs[bufc] = ret; sizes[bufc] = size; size = 0;
	for (i = 0; i <= bufc; ++i) size += sizes[i];
	ret = (byte*)malloc(size); size = 0;
    for (i = 0; i <= bufc; ++i) {
		memcpy(ret + size, bufs[i], sizes[i]);
		size += sizes[i];
		if (alloc[i]) free(bufs[i]);
	}
	*len = size;
	return ret;
}

int main(int argc, char **argv) {
	DCCallVM *vm = dlld_init(4096, 0);
	byte *args = 0, *ret = 0;
	int size = 0, i;

	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
	setvbuf(stdout, NULL, _IONBF, 0);

	while (1) {
		fread(&size,4,1,stdin);
		if (size == 0) break;
		args = (byte*)malloc(size);
		fread(args,size,1,stdin);
		lograw(": <",args,size);
		__try {
			ret = dlld_call(args,&size);
		} __except(1) {
			LOG("<call error>\n");
			dlld_exit();
			return -1;
		}
		free(args);
		lograw(": >",ret,size);
		fwrite(ret,size,1,stdout); //fflush(stdout);
		free(ret);
	}
	
	dlld_exit();
	return 0;
}