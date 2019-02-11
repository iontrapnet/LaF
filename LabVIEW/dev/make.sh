#!/bin/sh

gcc -shared -fPIC -Iinclude/jnlua -O3 lua53.c -L.. -ljnlua-5.3-linux-amd64 -lm -o liblua53.so
strip liblua53.so
mv liblua53.so ..

gcc -Iinclude/jnlua -O3 -Wl,-rpath,. lua.c -L.. -ljnlua-5.3-linux-amd64 -lm -o jnlua
strip jnlua
mv jnlua ..

gcc -Iinclude/luajit -O3 -Wl,-rpath,. lua.c -L.. -lluajit -lm -o luajit
strip luajit
mv luajit ..

cd ..
ln -s libjnlua-5.3-linux-amd64.so libjnlua.so
ln -s libluajit.so libluajit-5.1.so.2

#gcc -shared -fPIC -Wl,--whole-archive -L. -lML64i4 -luuid -Wl,--no-whole-archive -o ml64i4.so
