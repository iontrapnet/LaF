local eval = function(str)
    if not str:match'^[^{]+=' then
       str = 'return '..str
    end
    return loadstring(str)()
end

local call = function(...)
    local arg = {...}
    local r = eval(arg[1])
    local args = {}
    for i=2,#arg do
        args[i-1] = arg[i]
    end
    return r(unpack(args))
end

local get = function(...)
    local arg = {...}
    local r = _G
    for i=1,#arg do
        local t = type(r)
        if t == 'nil' or t == 'number' or t == 'string' then
            break
        else
            r = r[arg[i]]
        end
    end
    return r
end

local set = function(...)
    local arg = {...}
    local r = _G
    local len = #arg
    for i=1,len-1 do
        local t = type(r)
        if t == 'nil' or t == 'number' or t == 'string' then
            return false
        else
            if i == len-1 then
                r[arg[i]] = arg[len]
            end
            r = r[arg[i]]
        end
    end
    return true
end

if 1 then
local server = require "zerorpc.server"
local S = server.new("tcp://127.0.0.1:4242")
S:add('eval',eval)
S:add('call',call)
S:add('get',get)
S:add('set',set)
S:run()
else
eval('ffi=require"ffi"')
print(eval('ffi.arch'))
call('ffi.cdef','void printf(const char*);')
call('ffi.C.printf','hehe')
print(set('x',{1,2,{a=3}}))
print(get('x',3,'a'))
end