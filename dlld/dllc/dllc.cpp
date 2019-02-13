#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#define real32 float
#define real64 double

static FILE* log = 0;

#ifdef _WIN32
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
#define DLL
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#define API EXTERNC DLL

//shutdown(sock, SD_SEND);
#define CALL\
	args -= size;\
	send(sock,(const char*)&size,4,0);\
	send(sock,(const char*)args,size,0);\
	free(args);\
	recv(sock,(char*)&ret,sizeof(ret),0);\
	return ret

API void* dllc_load(const char* path) {
	static ptr_t func = 1;
	static byte head[] = {1,0x26};
	
	int size = sizeof(func) + sizeof(head) + strlen(path) + 1;
	byte *args = (byte*)malloc(size);
	void *ret = 0;

	memcpy(args,&func,sizeof(func)); args += sizeof(func);
	memcpy(args,head,sizeof(head)); args += sizeof(head);
	strcpy((char*)args,path); args += strlen(path) + 1;
	
	CALL;
}

API void* dllc_symbol(void* lib, const char* name) {
	static ptr_t func = 2;
	static byte head[] = {2,0x62,0x2};
	
	int size = sizeof(func) + sizeof(head) + sizeof(lib) + strlen(name) + 1;
	byte *args = (byte*)malloc(size);
	void *ret = 0;

	memcpy(args,&func,sizeof(func)); args += sizeof(func);
	memcpy(args,head,sizeof(head)); args += sizeof(head);
	memcpy(args,&lib,sizeof(lib)); args += sizeof(lib);
	strcpy((char*)args,name); args += strlen(name) + 1;

	CALL;
}

API double dllc_doublef(ptr_t func, double x) {
	static byte head[] = {1,0x55};

	int size = sizeof(func) + sizeof(head) + sizeof(x);
	byte *args = (byte*)malloc(size);
	double ret = 0.;

	memcpy(args,&func,sizeof(func)); args += sizeof(func);
	memcpy(args,head,sizeof(head)); args += sizeof(head);
	memcpy(args,&x,sizeof(x)); args += sizeof(x);

	CALL;
}

int main(int argc, char **argv) {
	int ret = 0;
	WSADATA wsaData;
    SOCKADDR_IN addr;

	const char *path = "msvcrt.dll", *name = "floor";
	void *lib = 0, *func = 0;

	//log = fopen("dllc.txt","w");
	log = stdout;

	WSAStartup(MAKEWORD(2,2), &wsaData);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
    if (argc > 2) {
		addr.sin_port = htons(atoi(argv[2]));
		addr.sin_addr.s_addr = inet_addr(argv[1]);
	} else if (argc > 1) {
		addr.sin_port = htons(atoi(argv[1]));
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	} else {
		fprintf(log,"usage: %s [host] port\n",argv[0]);
		return -1;
	}
	if (ret = connect(sock, (sockaddr*)&addr, sizeof(addr))) {
		fprintf(log,"connect error: %d\n",ret);
		closesocket(sock);
		WSACleanup();
		fclose(log);
		return ret;
	}

	fprintf(log,"lib = %p\n",lib = dllc_load(path));
	fprintf(log,"func = %p\n",func = dllc_symbol(lib,name));
	fprintf(log,"ret = %f\n",dllc_doublef((ptr_t)func, 3.14));

	ret = 0; send(sock,(const char*)&ret,4,0);
	closesocket(sock);
	WSACleanup();
	fclose(log);
	return 0;
}