#!/bin/sh

gcc -shared -fPIC -Wl,--whole-archive -L. -lML64i4 -luuid -Wl,--no-whole-archive -o ml64i4.so
