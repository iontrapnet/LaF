local ffi = require'ffi'
ffi.cdef[[
int time(int);
double floor(double);
void memcpy(void*,void*,int);
]]
local dllc = ffi.load'dllc'
ffi.cdef[[
int dllc_init(const char*,int);
void dllc_exit();
int dllc_open(const char*,int);
void dllc_close();
int dllc_write(const char*,int);
int dllc_read(char*,int);
int dllc_load(const char*);
int dllc_symbol(int,const char*);
double dllc_doublef(int,double);
int dllc_callf(int,const char*,...);
]]
local path = 'msvcrt.dll'
local name = 'floor'

dllc.dllc_init('127.0.0.1',8096)
local lib = dllc.dllc_load(path)
print('lib = ',lib)
local func = dllc.dllc_symbol(lib,name)
print('func = ',func)
local d = dllc.dllc_doublef(33,3.14)
print('d = ',d)
lib = dllc.dllc_load(path)
print('lib = ',lib)
d = dllc.dllc_doublef(func,3.14)
print('d = ',d)

local size = 1000000
local d1 = ffi.new('double[?]',size)
local d2 = ffi.new('double[?]',size)
for i=1,size do d1[i] = i end
print('d1 = {...,'..d1[size-4]..','..d1[size-3]..','..d1[size-2]..','..d1[size-1]..'}')
tic = ffi.C.time(0)
for i=1,10 do
dllc.dllc_callf(0,'>d<di=',ffi.cast('int',size),d2,ffi.cast('int',size),d1,ffi.cast('int',size*ffi.sizeof('double')))
end
print('time = ',ffi.C.time(0) - tic)
print('d2 = {...,'..d2[size-4]..','..d2[size-3]..','..d2[size-2]..','..d2[size-1]..'}')

tic = ffi.C.time(0)
for i=1,3000 do
ffi.C.memcpy(d2,d1,size*ffi.sizeof('double'))
end
print('time = ',ffi.C.time(0) - tic)

d = 0
tic = ffi.C.time(0)
for i=1,30000 do
    d = d + dllc.dllc_doublef(func,i+3.14)
end
print('d = ',d)
print('time = ',ffi.C.time(0) - tic)

lib = ffi.new('int[1]')
dllc.dllc_callf(1,'s=i',path,lib)
func = ffi.new('int[1]')
dllc.dllc_callf(2,'is=i',ffi.cast('int',lib[0]),name,func)
d = ffi.new('double[1]')
d0 = 0
tic = ffi.C.time(0)
for i=1,30000 do
    dllc.dllc_callf(func[0],'d=d',i+3.14,d)
    d0 = d0 + d[0]
end
print('d0 = ',d0)
print('time = ',ffi.C.time(0) - tic)

d = math.floor(3.14)
print('d = ',d)
tic = ffi.C.time(0)
for i=1,30000000 do
    d = d + math.floor(i+3.14)
end
print('d = ',d)
print('time = ',ffi.C.time(0) - tic)

d = ffi.C.floor(3.14)
print('d = ',d)
tic = ffi.C.time(0)
for i=1,30000000 do
    d = d + ffi.C.floor(i+3.14)
end
print('d = ',d)
print('time = ',ffi.C.time(0) - tic)

dllc.dllc_exit()