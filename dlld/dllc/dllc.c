#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define byte unsigned char
#define real32 float
#define real64 double

static FILE* stdlog = 0;
#define LOG(...) fprintf(stdlog,__VA_ARGS__)

#ifdef _WIN32
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
static SOCKET sock = 0;
#define DLL __declspec(dllexport)
#ifdef _WIN64
typedef uint64 ptr_t;
#else
typedef uint32 ptr_t;
#endif
#else
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
static int sock = 0;
#define DLL
typedef uint64 ptr_t;
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

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

static const char *HOST = 0;
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
	int ret = 0;

	send(sock,(const char*)&ret,4,0);
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
	stdlog = stderr;//stdlog = fopen("dllc.log","a"); setbuf(stdlog, NULL);
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
	for (i = 0; i < size; ++i) LOG("%02X ", bytes[i]);
	LOG("\n");
}
#else
#define lograw(...)
#endif

#define ARG(args,x) memcpy(args,&x,sizeof(x)); args += sizeof(x)

#define STRING_ARG(args,x) strcpy((char*)args,x); args += strlen(x)+1

#define FUNC(args,size,func,head)\
	byte *args = (byte*)malloc(4 + size);\
	memcpy(args,&size,4); args += 4;\
	ARG(args,func);ARG(args,head)

#define CHECK(expr)\
	if (expr <= 0) {\
	logtime();LOG(": <call error>\n");\
	dllc_close();dllc_open(0,0);\
	return 0;\
	}\

#define CALL(args,size)\
	args -= size;\
	lograw(": >",args,size);\
	CHECK(send(sock,(const char*)(args-=4),4+size,0));\
	free(args);
	
#define RET(ret)\
	CHECK(recv(sock,(char*)&ret,sizeof(ret),0));\
	return ret

API uint32 dllc_load(const char* path) {
	static uint32 func = 1;
	static byte head[] = {1,0x26};
	
	int size = sizeof(func) + sizeof(head) + strlen(path) + 1;
	uint32 ret = 0;

	FUNC(args,size,func,head);
	STRING_ARG(args,path);
	CALL(args,size);
	RET(ret);
}

API uint32 dllc_symbol(uint32 lib, const char* name) {
	static uint32 func = 2;
	static byte head[] = {2,0x62,0x2};
	
	int size = sizeof(func) + sizeof(head) + sizeof(lib) + strlen(name) + 1;
	uint32 ret = 0;

	FUNC(args,size,func,head);
	ARG(args,lib);
	STRING_ARG(args,name);
	CALL(args,size);
	RET(ret);
}

API int dllc_Write(const char *data, int size) {
	CHECK(send(sock,data,size,0));
	return 1;
}

API int dllc_read(char *data, int size) {
	CHECK(recv(sock,data,size,0));
	return 1;
}

API double dllc_doublef(uint32 func, double x) {
	static byte head[] = {1,0x55};

	int size = sizeof(func) + sizeof(head) + sizeof(x);
	double ret = 0.;

	FUNC(args,size,func,head);
	ARG(args,x);
	CALL(args,size);
	RET(ret);
}

int main(int argc, char **argv) {
	const char *path = "msvcrt.dll", *name = "floor", *host = "127.0.0.1";
	uint32 lib = 0, func = 0, port = 0;

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

	dllc_exit();
	return 0;
}