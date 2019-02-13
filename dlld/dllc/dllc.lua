local ffi = require'ffi'
local dllc = ffi.load'dllc'
ffi.cdef[[
int dllc_init(const char*,int);
void dllc_exit();
int dllc_load(const char*);
int dllc_symbol(int,const char*);
int dllc_write(const char*,int);
int dllc_read(char*,int);
double dllc_doublef(int,double);
]]
dllc.dllc_init('127.0.0.1',8096)
local lib = dllc.dllc_load'msvcrt.dll'
print('lib = ',lib)
local func = dllc.dllc_symbol(lib,'floor')
print('func = ',func)
local ret = dllc.dllc_doublef(33,3.14)
print('ret = ',ret)
lib = dllc.dllc_load'msvcrt.dll'
print('lib = ',lib)
ret = dllc.dllc_doublef(func,3.14)
print('ret = ',ret)

ffi.cdef[[int time(int);]]
local tic = ffi.C.time(0)
for i=1,10000 do
    ret = ret + dllc.dllc_doublef(func,i+3.14)
end
print('ret = ',ret)
print('time = ',ffi.C.time(0) - tic)
dllc.dllc_exit()

ret = math.floor(3.14)
print('ret = ',ret)
tic = ffi.C.time(0)
for i=1,10000000 do
    ret = ret + math.floor(i+3.14)
end
print('ret = ',ret)
print('time = ',ffi.C.time(0) - tic)

ffi.cdef[[double floor(double);]]
ret = ffi.C.floor(3.14)
print('ret = ',ret)
tic = ffi.C.time(0)
for i=1,10000000 do
    ret = ret + ffi.C.floor(i+3.14)
end
print('ret = ',ret)
print('time = ',ffi.C.time(0) - tic)
