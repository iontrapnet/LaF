#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "dllc.h"

FILE* stdlog = 0;

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
static SOCKET sock = 0;
#else
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
static int sock = 0;
#endif

#ifdef LOG
static void logtime() {
	time_t now;
	struct tm *tm_now;
 
	time(&now);
	tm_now = localtime(&now);
 
    LOG("[%04d-%02d-%02d %02d:%02d:%02d]",
		1900+tm_now->tm_year, tm_now->tm_mon, tm_now->tm_mday,
		tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}
#else
#define logtime(...)
#define LOG(...)
#endif

static const char *HOST = "127.0.0.1";
static int PORT = 0;

API int dllc_open(const char *host, int port) {
#ifdef _WIN32
	SOCKADDR_IN addr;
	char nodelay = 1;
#else
	struct sockaddr_in addr;
	int nodelay = 1;
#endif
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host ? (HOST = host) : HOST);
	addr.sin_port = htons(port ? (PORT = port) : PORT);
	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))) {
		logtime();
		LOG(": <connect error>\n");
		return 0;
	}
	return 1;
}

API void dllc_close() {
	int32 end = 0;

	send(sock,(const char*)&end,4,0);
	//shutdown(sock,SD_SEND);
#ifdef _WIN32
	closesocket(sock);
#else
	close(sock);
#endif
}

API int dllc_init(const char *host, int port) {
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
	if (!stdlog) {
		stdlog = stderr;
		//stdlog = fopen("dllc.log","a"); setbuf(stdlog, NULL);
	}
	return dllc_open(host,port);
}

API void dllc_exit() {
	dllc_close();
#ifdef _WIN32
	WSACleanup();
#endif
	fclose(stdlog);
}

#ifdef _DEBUG
static void lograw(const char* prefix, byte* bytes, int size) {
	int i;
	logtime();
	LOG("%s[%d]",prefix,size);
	if (bytes) for (i = 0; i < size; ++i) LOG("%02X ", bytes[i]);
	LOG("\n");
}
#else
#define lograw(...)
#endif

#define CHECK(expr)\
	if ((expr) <= 0) {\
	logtime();LOG(": <call error>\n");\
	dllc_close();dllc_open(0,0);\
	return 0;}

API int dllc_stdio() {
	static char buf[1024];
	int len;
	char ret = 0;

	while (1) {
		CHECK(recv(sock,&ret,sizeof(ret),0));	
		if (!ret) break;
		if (ret == -1) {
			gets(buf);
			len = strlen(buf);
			buf[len] = '\n';
			CHECK(send(sock,buf,len+1,0));
		} else
			fputc(ret,stdlog);
	}
	return 1;
}

API int dllc_write(const char *data, int size) {
	int ret = 0;
	
	while (size) {
		CHECK(ret = send(sock,data,size,0));
		data += ret;
		size -= ret;
	}
	return 1;
}

API int dllc_read(char *data, int size) {
	int ret = 0;

	while (size) {
		CHECK(ret = recv(sock,data,size,0));
		data += ret;
		size -= ret;
	}
	return 1;
}

API ptr_t dllc_load(const char* path) {
	static ptr_t func = 1;
	static byte head[] = {1,0x26};
	
	int32 size = sizeof(func) + sizeof(head);
	byte *args = 0;
	ptr_t lib = 0;

	NEXT_STR(size,path);
	FUNC(args,size,func,head);
	PUSH_STR(args,path);
	CALL(args,size);
	RET(lib);

	return lib;
}

API ptr_t dllc_symbol(ptr_t lib, const char* name) {
	static ptr_t func = 2;
	static byte head[] = {2,0x62,0x2};
	
	int32 size = sizeof(func) + sizeof(head);
	byte *args = 0;
	ptr_t sym = 0;

	NEXT(size,lib);
	NEXT_STR(size,name);
	FUNC(args,size,func,head);
	PUSH(args,lib);
	PUSH_STR(args,name);
	CALL(args,size);
	RET(sym);

	return sym;
}

API double dllc_doublef(ptr_t func, double x) {
	static byte head[] = {1,0x55};

	int32 size = sizeof(func) + sizeof(head);
	byte *args = 0;
	double d = 0.;

	NEXT(size,x);
	FUNC(args,size,func,head);
	PUSH(args,x);
	CALL(args,size);
	RET(d);

	return d;
}

typedef union arg_t {
	int8 b;
	int16 h;
	int32 i;
	int64 w;
	real32 f;
	real64 d;
	void *p;
} arg_t;

API int dllc_callf(ptr_t func, const char* proto, ...) {
	static byte head[128], out[256];
	static arg_t args[256];
	static int32 sizes[256];

	byte *buf = 0;
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
	
	//LOG(">[%d]%p[%d]\n",isize,func,*head);
	buf = (byte*)malloc(sizeof(isize) + isize);
	PUSH(buf,isize);
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
	if (!dllc_write((const char*)(buf -= sizeof(isize)),sizeof(isize) + isize)) return 0;
	if (!dllc_stdio()) return 0;
	if (!dllc_read((char*)&isize,sizeof(isize))) return 0;
	if (isize < osize || (isize > osize && sizes[*head] != -1)) return 0;
	osize = isize;
	if (outc == -1 || (outc == 0 && sizes[*head] == 0)) {
		free(buf);
		if (outc == -1) {
			if (sizes[*head] == -1) {
				buf = *(byte**)ret;
				if (!buf) {
					buf = (byte*)malloc(osize);
					*((byte**)ret) = buf;
				}
			} else
				buf = (byte*)ret;	
		} else
			buf = (byte*)(args[out[0]].p);
		if (!dllc_read((char*)buf,osize)) return 0;
		lograw(": <",buf,osize);
		return 1;
	}
	if (osize > isize) { free(buf); buf = (byte*)malloc(osize); }
	if (!dllc_read((char*)buf,osize)) return 0;
	lograw(": <",buf,osize);

	for (i = 0; i <= outc; ++i) {
		POP_MEM(buf,(byte*)(args[out[i]].p),sizes[out[i]]);
	}
	if (sizes[*head] == -1) {
		POP_STR(buf,*(char**)ret);
	} else {
		POP_MEM(buf,(byte*)ret,sizes[*head]);
	}
	buf -= osize; free(buf);
	return 1;
}

int main(int argc, char **argv) {
	const char *path = "msvcrt.dll", *name = "floor", *host = 0;
	uint32 lib = 0, func = 0, port = 0, size;
	double d = 0., d0 = 0., *d1 = 0, *d2 = 0;
	char *s = 0;

    if (argc > 2) {
		host = argv[1];
		port = atoi(argv[2]);
	} else if (argc > 1) {
		port = atoi(argv[1]);
	} else {
		fprintf(stderr,"usage: %s [host] port\n",argv[0]);
		return -1;
	}

	if (!dllc_init(host,port)) return -2;
	
	LOG("lib = %p\n",lib = dllc_load(path));
	LOG("func = %p\n",func = dllc_symbol(lib,name));
	LOG("ret = %lf\n",dllc_doublef(33,3.14));

	LOG("lib = %p\n",lib = dllc_load(path));
	LOG("ret = %lf\n",dllc_doublef(func,3.14));

	dllc_callf(1,"s=i",path,&lib);
	LOG("lib = %p\n",lib);
	dllc_callf(2,"is=i",lib,name,&func);
	LOG("func = %p\n",func);
	dllc_callf(func,"d=d",3.14,&d);
	LOG("d = %lf\n",d);

	dllc_callf(2,"is=i",lib,"strstr",&func);
	dllc_callf(func,"ss=s","abcd","bc",&s);
	LOG("s = %s\n",s);

	dllc_callf(2,"is=i",0,"dlld_call",&func);
	LOG("func = %p\n",func);

	dllc_callf(0,"&d&di=",&d0,&d,8);
	LOG("d0 = %lf\n",d0);

	d0 = 3.14;
	dllc_callf(0,"DDi=",&d,&d0,8);
	LOG("d = %lf\n",d);

	size = 1000000;
	d1 = (double*)malloc(size*sizeof(double));
	d2 = (double*)malloc(size*sizeof(double));

	d1[size-4] = 3.;d1[size-3] = 0.;d1[size-2] = 1.;d1[size-1] = 4.;
	LOG("d1 = {...,%lf,%lf,%lf,%lf}\n",d1[size-4],d1[size-3],d1[size-2],d1[size-1]);

	dllc_callf(0,">d<di=",size,d2,size,d1,size*sizeof(double));
	LOG("d2 = {...,%lf,%lf,%lf,%lf}\n",d2[size-4],d2[size-3],d2[size-2],d2[size-1]);

	d1[size-3] = 2.;
	dllc_callf(0,"*d*di=",size,d2,size,d1,size*sizeof(double));
	LOG("d2 = {...,%lf,%lf,%lf,%lf}\n",d2[size-4],d2[size-3],d2[size-2],d2[size-1]);

	dllc_exit();
	return 0;
}