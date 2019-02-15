#!/bin/sh

gcc -w dllc.c -o dllc
gcc -w -shared -fPIC dllc.c -o libdllc.so
