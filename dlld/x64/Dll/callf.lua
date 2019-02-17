local arg = arg or {...}
if #arg < 1 then
    print'usage: callf libname [[host] port]'
    os.exit(0)
end

local lib = arg[1]
lines = {}
--string.gmatch(text,'[^\r\n]+')
for line in io.lines(lib..'.h') do lines[#lines+1] = line end

local pprint = require'pprint'
local cpp = require'cparser'

callf_type = {'i',void='',int='i',size_t='i',lua_Number='d'}
function callf_proto(t)
    if t.tag == 'Function' then
        local r = ''
        for _,v in ipairs(t) do
            r = r..callf_proto(v)
        end
        return r..'='..callf_proto(t.t)
    end
    if t.tag == 'Pair' then
        return (t.ellipsis and '.') or callf_proto(t[1])
    elseif t.tag == 'Qualified' then
        return callf_proto(t.t)
    elseif t.tag == 'Pointer' then
        if t.t.tag == 'Qualified' then
           t = t.t 
        end
        if t.t.tag == 'Type' then
            if t.t.n == 'char' then
                return 's'
            elseif t.t.n == 'void' then
                return callf_type[1]
            end
            t = callf_type[t.t.n]
            return (t and t:upper()) or callf_type[1]
        else
            return '&'..callf_proto(t.t)
        end
    elseif t.tag == 'Type' then
        return callf_type[t.n] or callf_type[1]
    else
        return '['..t.tag..']'
    end
end

callf_typename = {b='int8',h='int16',i='int32',w='int64',f='real32',d='real64',s='char*',B='int8*',H='int16*',I='int32*',W='int64*',F='real32*',D='real64*'}
function callf_ccode(func,proto)
    local types = {}
    local decl, body
    local k = 1
    while true do
        local c = proto:sub(k,k)
        if c == '&' then
            k = k + 1
            c = proto:sub(k,k)
            types[#types+1] = callf_typename[c]..'*'
        elseif c == '*' then
            types[#types+1] = callf_typename['i']
            k = k + 1
            c = proto:sub(k,k)
            types[#types+1] = callf_typename[c]..'*'
        elseif c == '=' then
           k = k + 1
           c = proto:sub(k,k)
           local r = c and callf_typename[c]
           decl = {}
           body = {}
           for i,v in ipairs(types) do
               decl[i] = v..' x'..i
               body[i] = 'x'..i
           end
           decl = (r or 'void')..' '..func..'('..table.concat(decl,',')..')'
           if r then
               body[#body+1] = '&x0'
           end
           body = 'CALLF(_'..func..',"'..proto..'",'..table.concat(body,',')..');'
           if r then
               body = r..' x0;\n'..body..'\nreturn x0;'
           end
           return 'API '..decl..' {\n'..body..'\n}'
        else
            types[#types+1] = callf_typename[c]
        end
        k = k + 1
    end
end
--print(callf_ccode('foo','i*dBs=i'))
--os.exit(0)

local decl = {}
local init = {}
local body = {}
for _,line in ipairs(lines) do
t,n = cpp.stringToType(line)
decl[#decl+1] = '_'..n
init[#init+1] = 'CALLF(2,"'..callf_type[1]..'s='..callf_type[1]..'",lib,"'..n..'",&_'..n..');\n'
body[#body+1] = callf_ccode(n,callf_proto(t))
end

io.output('_'..lib..'.c')
io.write'#include "callf.h"\n'
io.write('ptr_t '..table.concat(decl,',')..';\n')
io.write('API void '..lib..'_init() {\nptr_t lib;\n')
if #arg < 2 then
    io.write'#define CALLF(...) dlld_callf(__VA_ARGS__)\n'
    io.write'dlld_init(4096,0);\n'
else
    io.write'#define CALLF(...) dllc_callf(__VA_ARGS__)\n'
    if #arg < 3 then
        io.write('dllc_init(0,'..arg[2]..');\n')
    else
        io.write('dllc_init("'..arg[2]..'",'..arg[3]..');\n')
    end
end
io.write('CALLF(1,"s='..callf_type[1]..'","'..lib..'.dll",&lib);\n')
io.write(table.concat(init)..'}\n')
io.write(table.concat(body,'\n'))
io.close()

-- tcc32 / tcc lua51.c _lua51.c dllc.dll / dlld.dll

--[[
local ffi = require'ffi'
local ffir = require'ffi_reflect'
local lua = require'luastate51'

local func = ffir.typeof(lua.C.lua_tolstring)
pprint(func)
pprint(func.return_type.what)
for i in func:arguments() do pprint(i) end
]]
