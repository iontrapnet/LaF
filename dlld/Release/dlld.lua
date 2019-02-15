local ffi = require'ffi'
if ffi.os == 'Windows' then
    time = os.clock
else
    ffi.cdef[[int time(int);]]
    time = function() return ffi.C.time(0) end
end
ffi.cdef[[
double floor(double);
void memcpy(void*,void*,int);
]]
local dlld = ffi.load'dlld'
ffi.cdef[[
void dlld_init(int,int);
void dlld_exit();
void* dlld_funcs[256];
char* dlld_call(char*,int);
int dlld_callf(int,const char*,...);
]]
local path = 'msvcrt.dll'
local name = 'floor'

dlld.dlld_init(4096,0)

local size = 1000000
local d1 = ffi.new('double[?]',size)
local d2 = ffi.new('double[?]',size)
for i=1,size do d1[i] = i end
print('d1 = {...,'..d1[size-4]..','..d1[size-3]..','..d1[size-2]..','..d1[size-1]..'}')
tic = time()
for i=1,150 do
dlld.dlld_callf(0,'>d<di=',ffi.cast('int',size),d2,ffi.cast('int',size),d1,ffi.cast('int',size*ffi.sizeof('double')))
end
print('d2 = {...,'..d2[size-4]..','..d2[size-3]..','..d2[size-2]..','..d2[size-1]..'}')
print('time = ',time() - tic)

tic = time()
for i=1,3000 do
ffi.C.memcpy(d2,d1,size*ffi.sizeof('double'))
end
print('time = ',time() - tic)

lib = ffi.new('int[1]')
dlld.dlld_callf(1,'s=i',path,lib)
func = ffi.new('int[1]')
dlld.dlld_callf(2,'is=i',ffi.cast('int',lib[0]),name,func)
d = ffi.new('double[1]')
d0 = 0
tic = time()
for i=1,30000 do
    dlld.dlld_callf(func[0],'d=d',i+3.14,d)
    d0 = d0 + d[0]
end
print('d0 = ',d0)
print('time = ',time() - tic)

d = math.floor(3.14)
tic = time()
for i=1,30000000 do
    d = d + math.floor(i+3.14)
end
print('d = ',d)
print('time = ',time() - tic)

d = ffi.C.floor(3.14)
tic = time()
for i=1,30000000 do
    d = d + ffi.C.floor(i+3.14)
end
print('d = ',d)
print('time = ',time() - tic)

dlld.dlld_exit()