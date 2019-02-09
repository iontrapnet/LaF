local client = require "zerorpc.client"
local S = client.new("tcp://127.0.0.1:4242")

pprint = require'pprint'

local Sfunc = function(name) return function(...) return S:call(name,...) end end
local seval = Sfunc('eval')
local scall = Sfunc('call')
local sget = Sfunc('get')
local sset = Sfunc('set')

print(S:call('nil'))

print(scall('os.clock'))

print(seval('ffi=require"ffi"'))
print(seval('ffi.arch'))

scall('ffi.cdef','void printf(const char*);')
scall('ffi.C.printf','hehe')

print(sset('x',{1,2,{a=3}}))
print(sget('x',1))
print(sset('x',1,5))
r = sget('x')

pprint(r)

S:close()
