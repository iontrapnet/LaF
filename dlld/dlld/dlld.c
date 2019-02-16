#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include "dlld.h"

FILE* stdlog = 0;

#ifdef _WIN32
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
#include "../64/include/dyncall.h"
#include "../64/include/dynload.h"
#pragma comment(lib,"../64/lib/libdyncall_s.lib")
#pragma comment(lib,"../64/lib/libdynload_s.lib")
#else
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

#ifdef _DEBUG
static void lograw(const char* prefix, byte* bytes, int size) {
	int i;
	logtime();
	LOG("%s[%d]",prefix,size);
	for (i = 0; i < size; ++i) LOG("%02X ", bytes[i]);
	LOG("\n");
}
#else
#define lograw(...)
#endif

#ifndef LOG
#define LOG(...)
#endif

static DCCallVM *vm = 0;
API void dlld_init(int size, int mode) {
	if (!stdlog) {
		//stdlog = stderr;
		stdlog = fopen("dlld.log","a"); setbuf(stdlog, NULL);
	}
	vm = dcNewCallVM(size);
	dcMode(vm, mode);
}

API void dlld_exit() {
	fclose(stdlog);
	dcFree(vm);
}

API void* dlld_funcs[256] = {memcpy, dlLoadLibrary, dlFindSymbol, dlFreeLibrary};

API byte* dlld_call(byte *args, int32 *len) {
	static byte *bufs[256];
	static int32 sizes[256], size;

	ptr_t func = *(ptr_t*)args;
	int argc = 1 + *(args += sizeof(ptr_t)), bufc = -1, odd = 0, i;
	byte *ret = 0, *argv = (++args) + (argc >> 1) + (argc & 1);

	logtime(); LOG(": %p(",func);
	if (func < 256) func = (ptr_t)dlld_funcs[func];
	
	dcReset(vm);
	while (--argc) {
#define ARG_TYPE (odd ? ((0xF0 & *args) >> 4) : (0xF & *args))
		switch (ARG_TYPE) {
		case 0x0: // int8
			LOG("%hhd,",*(DCchar*)argv);
			dcArgChar(vm, *(DCchar*)argv); argv += 1;
			break;
		case 0x1: // int16
			LOG("%hd,",*(DCshort*)argv);
			dcArgShort(vm, *(DCshort*)argv); argv += 2;
			break;
		case 0x2: // int32
			LOG("%d,",*(DCint*)argv);
			dcArgInt(vm, *(DCint*)argv); argv += 4;
			break;
		case 0x3: // int64
			LOG("%lld,",*(DClonglong*)argv);
			dcArgLongLong(vm, *(DClonglong*)argv); argv += 8;
			break;
		case 0x4: // real32
			LOG("%f,",*(DCfloat*)argv);
			dcArgFloat(vm, *(DCfloat*)argv); argv += 4;
			break;
		case 0x5: // real64
			LOG("%lf,",*(DCdouble*)argv);
			dcArgDouble(vm, *(DCdouble*)argv); argv += 8;
			break;
		case 0x6: // c string (\0)
			LOG("\"%s\",",argv);
			dcArgPointer(vm, argv); argv += strlen((const char*)argv) + 1;
			break;
		case 0x7: // byte[] (in/out)
			size = *(int32*)argv;
			LOG("[%d],",size);
			bufs[++bufc] = argv + 4; sizes[bufc] = size; argv += 4 + size;
			dcArgPointer(vm, bufs[bufc]);
			break;
		case 0x8: // int8 & (ref)
			LOG("&%hhd,",*(DCchar*)argv);
			dcArgPointer(vm, argv);
			bufs[++bufc] = argv; sizes[bufc] = 1; argv += 1;
			break;
		case 0x9: // int16 & (ref)
			LOG("&%hd,",*(DCshort*)argv);
			dcArgPointer(vm, argv);
			bufs[++bufc] = argv; sizes[bufc] = 2; argv += 2;
			break;
		case 0xA: // int32/real32 & (ref)
			LOG("&%d,",*(DCint*)argv);
			dcArgPointer(vm, argv);
			bufs[++bufc] = argv; sizes[bufc] = 4; argv += 4;
			break;
		case 0xB: // int64/real64 & (ref)
			LOG("&%lld,",*(DClonglong*)argv);
			dcArgPointer(vm, argv);
			bufs[++bufc] = argv; sizes[bufc] = 8; argv += 8;
			break;
		case 0xC: // small byte[] < (in)
			size = *(byte*)argv;
			LOG("<[%d],",size);
			dcArgPointer(vm, argv + 1); argv += 1 + size;
			break;
		case 0xD: // byte[] < (in)
			size = *(int32*)argv;
			LOG("<[%d],",size);
			dcArgPointer(vm, argv + 4); argv += 4 + size;
			break;
		case 0xE: // small byte[] > (out)
			size = *(byte*)argv;
			LOG(">[%d],",size);
			bufs[++bufc] = (byte*)malloc(size); sizes[bufc] = -size; argv += 1;
			dcArgPointer(vm, bufs[bufc]); 
			break;
		case 0xF: // byte[] > (out)
			size = *(int32*)argv;
			LOG(">[%d],",size);
			bufs[++bufc] = (byte*)malloc(size); sizes[bufc] = -size; argv += 4;
			dcArgPointer(vm, bufs[bufc]); 
			break;
		}
		if (odd) ++args;
		odd = 1 - odd;
	}

	ret = (byte*)malloc(8);
	switch (ARG_TYPE) {
		case 0:
			*(DCchar*)ret = dcCallChar(vm, (DCpointer)func); size = -1;
			LOG("=%hhd)\n",*(DCchar*)ret);
			break;
		case 1:
			*(DCshort*)ret = dcCallShort(vm, (DCpointer)func); size = -2;
			LOG("=%hd)\n",*(DCshort*)ret);
			break;
		case 2:
			*(DCint*)ret = dcCallInt(vm, (DCpointer)func); size = -4;
			LOG("=%d)\n",*(DCint*)ret);
			break;
		case 3:
			*(DClonglong*)ret = dcCallLongLong(vm, (DCpointer)func); size = -8;
			LOG("=%lld)\n",*(DClonglong*)ret);
			break;
		case 4:
			*(DCfloat*)ret = dcCallFloat(vm, (DCpointer)func); size = -4;
			LOG("=%f)\n",*(DCfloat*)ret);
			break;
		case 5:
			*(DCdouble*)ret = dcCallDouble(vm, (DCpointer)func); size = -8;
			LOG("=%lf)\n",*(DCdouble*)ret);
			break;
		case 6:
			free(ret);
			ret = (byte*)dcCallPointer(vm, (DCpointer)func);
			if (!ret) ret = (byte*)"";
			size = strlen((const char*)ret) + 1;
			LOG("=\"%s\")\n",(const char*)ret);
			break;
		default:
			free(ret);
			dcCallVoid(vm, (DCpointer)func);
			ret = 0;
			LOG("=)\n");
			break;
	}
#undef ARG_TYPE

	if (ret) {
		bufs[++bufc] = ret; sizes[bufc] = size;
	}
	size = 0;
	for (i = 0; i <= bufc; ++i) size += sizes[i] >= 0 ? sizes[i] : -sizes[i];
	ret = (byte*)malloc(size); size = 0;
    for (i = 0; i <= bufc; ++i) {
		if (sizes[i] >= 0) {
			memcpy(ret + size, bufs[i], sizes[i]);
			size += sizes[i];
		} else {
			memcpy(ret + size, bufs[i], -sizes[i]);
			size += -sizes[i];
			free(bufs[i]);
		}
	}
	*len = size;
	return ret;
}

#ifdef _WINDLL
typedef union arg_t {
	int8 b;
	int16 h;
	int32 i;
	int64 w;
	real32 f;
	real64 d;
	void *p;
} arg_t;

API int dlld_callf(ptr_t func, const char* proto, ...) {
	static byte head[128], out[256];
	static arg_t args[256];
	static int32 sizes[256];

	byte *buf = 0, *tmp = 0;
	void *ret = 0;
	int32 isize = sizeof(func), osize = 0, outc = -1, i = 0;
	va_list vl;

	va_start(vl,proto);
	while (*proto != '=') {
		int32 size;
		switch (*proto) {
#define ARG_TYPE(t) if(i&1) head[(i>>1)+1]|=(t<<4);else head[(i>>1)+1]=t
		case '0':case 'b':
			args[i].b = va_arg(vl,int8);
			size = -1; ARG_TYPE(0x0);
			break;
		case '1':case 'h':
			args[i].h = va_arg(vl,int16);
			size = -2; ARG_TYPE(0x1);
			break;
		case '2':case 'i':
			args[i].i = va_arg(vl,int32);
			size = -4; ARG_TYPE(0x2);
			break;
		case '3':case 'w':
			args[i].w = va_arg(vl,int64);
			size = -8; ARG_TYPE(0x3);
			break;
		case '4':case 'f':
			args[i].f = va_arg(vl,real32);
			size = -4; ARG_TYPE(0x4);
			break;
		case '5':case 'd':
			args[i].d = va_arg(vl,real64);
			size = -8; ARG_TYPE(0x5);
			break;
		case '6':case 's':
			args[i].p = va_arg(vl,void*);
			if (!args[i].p) args[i].p = ""; 
			size = strlen((const char*)args[i].p) + 1;
			ARG_TYPE(0x6);
			break;
		case '*':
			size = va_arg(vl,int32);
			isize += 4;
			switch (*(++proto)) {
			case '1':case 'h':
			size <<= 1; break;
			case '2':case 'i':
			case '4':case 'f':
			size <<= 2; break;
			case '3':case 'w':
			case '5':case 'd':
			size <<= 3; break;
			break;
			}
			args[i].p = va_arg(vl,void*); ARG_TYPE(0x7);
			out[++outc] = i; osize += size;
			break;
		case '&':
			switch (*(++proto)) {
			case '0':case 'b':
			size = 1; ARG_TYPE(0x8); break;
			case '1':case 'h':
			size = 2; ARG_TYPE(0x9); break;
			case '2':case 'i':
			case '4':case 'f':
			size = 4; ARG_TYPE(0xA); break;
			case '3':case 'w':
			case '5':case 'd':
			size = 8; ARG_TYPE(0xB); break;
			}
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			break;
		case 'B':
			size = 1; ARG_TYPE(0x8);
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			break;
		case 'H':
			size = 2; ARG_TYPE(0x9);
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			break;
		case 'I':case 'F':
			size = 4; ARG_TYPE(0xA);
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			break;
		case 'W':case 'D':
			size = 8; ARG_TYPE(0xB);
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			break;
		case '<':
			size = va_arg(vl,int32);
			switch (*(++proto)) {
			case '1':case 'h':
			size <<= 1; break;
			case '2':case 'i':
			case '4':case 'f':
			size <<= 2; break;
			case '3':case 'w':
			case '5':case 'd':
			size <<= 3; break;
			break;
			}
			if (size < 256) {
				isize += 1;
				ARG_TYPE(0xC);
			} else {
				isize += 4;
				ARG_TYPE(0xD);
			}
			args[i].p = va_arg(vl,void*);
			break;
		case '>':
			size = va_arg(vl,int32);
			switch (*(++proto)) {
			case '1':case 'h':
			size <<= 1; break;
			case '2':case 'i':
			case '4':case 'f':
			size <<= 2; break;
			case '3':case 'w':
			case '5':case 'd':
			size <<= 3; break;
			break;
			}
			if (size < 256) {
				isize += 1;
				ARG_TYPE(0xE);
			} else {
				isize += 4;
				ARG_TYPE(0xF);
			}
			args[i].p = va_arg(vl,void*);
			out[++outc] = i; osize += size;
			sizes[i] = size; size = 0;
			break;
		}
		if (size) sizes[i] = size;
		isize += size >= 0 ? size : -size;
		++i; ++proto;
	}
	*head = i;
	isize += 2 + (*head >> 1);

	switch (*(++proto)) {
	case '0':case 'b':
	sizes[*head] = 1; ARG_TYPE(0x0);
	break;
	case '1':case 'h':
	sizes[*head] = 2; ARG_TYPE(0x1);
	break;
	case '2':case 'i':
	sizes[*head] = 4; ARG_TYPE(0x2);
	break;
	case '3':case 'w':
	sizes[*head] = 8; ARG_TYPE(0x3);
	break;
	case '4':case 'f':
	sizes[*head] = 4; ARG_TYPE(0x4);
	break;
	case '5':case 'd':
	sizes[*head] = 8; ARG_TYPE(0x5);
	break;
	case '6':case 's':
	sizes[*head] = -1; ARG_TYPE(0x6);
	break;
	default:
	sizes[*head] = 0; ARG_TYPE(0x7);
	break;
	}
	if (sizes[*head]) ret = va_arg(vl,void*);
	va_end(vl);
	osize += sizes[*head] >= 0 ? sizes[*head] : -sizes[*head];
#undef ARG_TYPE

	buf = (byte*)malloc(isize);
	PUSH(buf,func);
	PUSH_MEM(buf,head,2 + (*head >> 1));
	for (i = 0; i < *head; ++i) {
#undef ARG_TYPE
#define ARG_TYPE(i) (i&1?((0xF0&head[(i>>1)+1])>>4):(0xF&head[(i>>1)+1]))
		byte type = ARG_TYPE(i);
		if (sizes[i] < 0) {
			PUSH_MEM(buf,(byte*)(args + i),-sizes[i]);
		} else {
			if (type > 0xB && sizes[i] < 256) {
				PUSH(buf,*(byte*)&sizes[i]);
			} else if (type == 0x7 || type > 0xB) {
				PUSH(buf,sizes[i]);
			}
			if (type < 0xE) {
				PUSH_MEM(buf,(byte*)(args[i].p),sizes[i]);
			}
		}
	}
	buf -= isize;

	lograw(": >",buf,isize);
	tmp = buf;
	buf = dlld_call(tmp,&osize);
	free(tmp);
	lograw(": <",buf,osize);
	
	for (i = 0; i <= outc; ++i) {
		POP_MEM(buf,(byte*)(args[out[i]].p),sizes[out[i]]);
	}
	if (sizes[*head] == -1) {
		POP_STR(buf,(char*)ret);
	} else {
		POP_MEM(buf,(byte*)ret,sizes[*head]);
	}
	buf -= osize; free(buf);
	return 1;
}
#endif

int main(int argc, char **argv) {
	byte *args = 0, *ret = 0;
	int32 size = 0;
	char eof = 0;

	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
	setvbuf(stdout, NULL, _IONBF, 0);

	dlld_init(4096,0);
	while (1) {
		fread(&size,sizeof(size),1,stdin);
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
		fwrite(&eof,sizeof(eof),1,stdout);
		fwrite(&size,sizeof(size),1,stdout);
		fwrite(ret,size,1,stdout); //fflush(stdout);
		free(ret);
	}
	dlld_exit();
	return 0;
}