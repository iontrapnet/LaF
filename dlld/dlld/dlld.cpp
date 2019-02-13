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

static FILE *log;

#ifdef _WIN32
#include <windows.h>
void logtime(const char *s) {
	SYSTEMTIME t;
    //GetSystemTime(&t);
	GetLocalTime(&t);
    fprintf(log,"%04d-%02d-%02d %02d:%02d:%02d.%03d%s",
		t.wYear, t.wMonth, t.wDay,
        t.wHour, t.wMinute, t.wSecond, t.wMilliseconds,s);
}
#define DLL __declspec(dllexport)
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
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

static DCCallVM *vm = 0;
API DCCallVM* dlld_init(int size, int mode) {
	vm = dcNewCallVM(size);
	dcMode(vm, mode);
	return vm;
}

API void dlld_exit() {
	dcFree(vm);
}

API void* dlld_funcs[256] = {memcpy, dlLoadLibrary, dlFindSymbol, dlFreeLibrary};

API byte* dlld_call(byte *args, int *len) {
	static void *bufs[256];
	static int sizes[256];
	static bool alloc[256];
    ptr_t func = *(ptr_t*)args;
	int argc = 1 + *(args += sizeof(ptr_t)), bufc = -1, odd = 0, type = 0, size = 0, i;
	byte *argv = (++args) + (argc >> 1) + (argc & 1), *ret = 0;

	logtime(": ");
	fprintf(log,"%p(",func);
	if (func < 256) func = (ptr_t)dlld_funcs[func];

	dcReset(vm);
 	while (--argc) {
#define TYPE_DECODE type = odd ? ((0xF0 & *args) >> 4) : (0xF & *args)
		TYPE_DECODE;
		fprintf(log,"[%d]",type);
		switch (type) {
		case 0x0:
			fprintf(log,"%c,",*(DCchar*)argv);
			dcArgChar(vm, *(DCchar*)argv); argv += 1;
			break;
		case 0x1:
			fprintf(log,"%hd,",*(DCshort*)argv);
			dcArgShort(vm, *(DCshort*)argv); argv += 2;
			break;
		case 0x2:
			fprintf(log,"%d,",*(DCint*)argv);
			dcArgInt(vm, *(DCint*)argv); argv += 4;
			break;
		case 0x3:
			fprintf(log,"%lld,",*(DClonglong*)argv);
			dcArgLongLong(vm, *(DClonglong*)argv); argv += 8;
			break;
		case 0x4:
			fprintf(log,"%f,",*(DCfloat*)argv);
			dcArgFloat(vm, *(DCfloat*)argv); argv += 4;
			break;
		case 0x5:
			fprintf(log,"%lf,",*(DCdouble*)argv);
			dcArgDouble(vm, *(DCdouble*)argv); argv += 8;
			break;
		case 0x6: // \0 terminated string
			fprintf(log,"\"%s\",",argv);
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
			bufs[++bufc] = malloc(1); sizes[bufc] = 1; alloc[bufc] = true;
			dcArgPointer(vm, (DCpointer)bufs[bufc]);
			break;
		case 0xB:
			bufs[++bufc] = malloc(*argv); sizes[bufc] = *argv; alloc[bufc] = true;
			dcArgPointer(vm, (DCpointer)bufs[bufc]); argv += 1;
			break;
		case 0xC:
			size = *(uint32*)argv;
			bufs[++bufc] = malloc(size); sizes[bufc] = size; alloc[bufc] = true;
			dcArgPointer(vm, (DCpointer)bufs[bufc]); argv += 4;
			break;
		case 0xD:
			bufs[++bufc] = argv; sizes[bufc] = 1; alloc[bufc] = false;
			dcArgPointer(vm, (DCpointer)argv); argv += 1;
			break;
		case 0xE:
			bufs[++bufc] = argv + 1; sizes[bufc] = *argv; alloc[bufc] = false;
			dcArgPointer(vm, (DCpointer)(argv + 1)); argv += 1 + *argv;
			break;
		case 0xF:
			size = *(uint32*)argv;
			bufs[++bufc] = argv + 4; sizes[bufc] = size; alloc[bufc] = false;
			dcArgPointer(vm, (DCpointer)(argv + 4)); argv += 4 + size;
			break;
		}
		if (odd) ++args;
		odd = 1 - odd;
	}

	TYPE_DECODE;
	fprintf(log,">[%d]",type);
	ret = (byte*)malloc(8); alloc[++bufc] = true;
	switch (type) {
		case 0:
			*(DCchar*)ret = dcCallChar(vm, (DCpointer)func); size = 1;
			fprintf(log,"%c)\n",*(DCchar*)ret);
			break;
		case 1:
			*(DCshort*)ret = dcCallShort(vm, (DCpointer)func); size = 2;
			fprintf(log,"%hd)\n",*(DCshort*)ret);
			break;
		case 2:
			*(DCint*)ret = dcCallInt(vm, (DCpointer)func); size = 4;
			fprintf(log,"%d)\n",*(DCint*)ret);
			break;
		case 3:
			*(DClonglong*)ret = dcCallLongLong(vm, (DCpointer)func); size = 8;
			fprintf(log,"%lld)\n",*(DClonglong*)ret);
			break;
		case 4:
			*(DCfloat*)ret = dcCallFloat(vm, (DCpointer)func); size = 4;
			fprintf(log,"%f)\n",*(DCfloat*)ret);
			break;
		case 5:
			*(DCdouble*)ret = dcCallDouble(vm, (DCpointer)func); size = 8;
			fprintf(log,"%lf)\n",*(DCdouble*)ret);
			break;
		case 6:
			free(ret); alloc[bufc] = false;
			ret = (byte*)dcCallPointer(vm, (DCpointer)func);
			size = strlen((const char*)ret) + 1;
			fprintf(log,"\"%s\")\n",(const char*)ret);
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
	
	log = fopen("dlld.txt","w");

	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
	setvbuf(stdout, NULL, _IONBF, 0);
	setbuf(stdout, NULL);

	while (true) {
		fread(&size,4,1,stdin);
		if (size == 0) break;
		logtime(": ");
		fprintf(log,"<[%d]",size);
		args = (byte*)malloc(size);
		fread(args,size,1,stdin);
		for (i = 0; i < size; ++i) fprintf(log, "%02X ", args[i]);
		fprintf(log, "\n");
		__try {
			ret = dlld_call(args,&size);
		} __except(1) {
			return -1;	
		}
		free(args);
		logtime(": ");
		fprintf(log,">[%d]",size);
		for (i = 0; i < size; ++i) fprintf(log, "%02X ", ret[i]);
		fprintf(log, "\n");
		fwrite(ret,size,1,stdout); //fflush(stdout);
		free(ret);
	}
	
	fclose(log);
	dlld_exit();
	return 0;
}