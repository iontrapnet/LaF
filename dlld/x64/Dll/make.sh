#!/bin/sh

gcc -w ../../dllc/dllc.c -o dllc
gcc -w -shared -fPIC ../../dllc/dllc.c -o libdllc.so
gcc -w -Wl,-rpath=. lua51.c _lua51.c -L. -ldllc -o lua51