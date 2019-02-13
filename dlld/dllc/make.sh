#!/bin/sh

gcc dllc.c -o dllc
gcc -shared -fPIC dllc.c -o libdllc.so