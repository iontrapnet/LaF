arg = arg or {...}
--if not table.unpack then table.unpack = unpack end

os.execute('cls')
pprint = require'pprint'
pprint.defaults.show_all=true
pprint.defaults.use_tostring=true

if type(arg[1]) == 'string' then
    __file = arg[1]:sub(arg[1]:find(' %- ')+3)
else
    __file = debug.getinfo(1).source:sub(2)
end
__dir = __file:gsub('/','\\'):match'(.*)\\.*$' or io.popen'cd':read'*l'
function vipath(path)
    if path ~= '' and path:sub(-3) ~= '.vi' then path = __dir..[[\vi\]]..path..'.vi' end
    return path
end

function callvi(vi,inames,ivals,onames)
    return coroutine.yield(vi,inames,ivals,onames,vipath'CallVI.lua',0)
end

VIs = {}
function loadvi(path)
    path = vipath(path)
    local vi = VIs[path]
    if not vi then
        vi = coroutine.yield(path,1)
        VIs[path] = vi
    end
    return vi
end

function GetAllCtrls(vi)
    local r = callvi(loadvi'GetAllCtrls',{'VI'},{vi},{'Ctrls'})
    local ctrls = {}
    for _,v in ipairs(r[1]) do
        ctrls[v.Name] = v.Ctrl
    end
    return ctrls
end

function CtrlGetValue(ctrl)
    return callvi(loadvi'CtrlGetValue',{'reference'},{ctrl},{'variant'})[1]
end

function CtrlSetValue(ctrl,val)
    callvi(loadvi'CtrlSetValue',{'reference','variant'},{ctrl,val},{})
end

write = function(s) coroutine.yield(s,'output') end
read = function() return coroutine.yield('input') end
function cls()write'\\cls\n'end
ilua = ilua or {delay=true}
require 'ilua'
ilua.set_writer(write)
ilua.set_reader(read)

function test()
local vi = loadvi[[C:\Desktop\CurrentTimeString.vi]]
pprint(vi)
local r = coroutine.yield({{1,2},{3,4},__tolv='array'},vipath'Test.lua','call')
pprint(r)
pprint(callvi(vi,{},{},{'date/time string'}))
local ctrls = GetAllCtrls(vi)
pprint(ctrls)
local ctrl = ctrls['date/time string']
pprint(CtrlGetValue(ctrl))

--write(coroutine.yield('hehe','test')..'\n')
ctrls = GetAllCtrls(loadvi'')
while ctrl ~= 'Run' do
    ctrl = coroutine.yield('event')
    val = CtrlGetValue(ctrls[ctrl])
    write(ctrl..'->'..pprint.pformat(val)..'\n')
end
CtrlSetValue(ctrls[ctrl],true)
end

function repl()
while true do
    write'> '
    local input = read()
    if input == 'quit' then
        break
    elseif input == 'cls' then
        write'\\cls\n'
    else
        --write(input..'\n')
        if #input > 0 then
          if input:sub(1,1) == '=' then
            input = 'return '..input:sub(2)
          end
          local line = ' '
          while not load(input) do
            write'>> '
            line = read()
            --write(line..'\n')
            if #line == 0 then break end
            input = input..'\n'..line
          end
          if not (#line == 0) then
              local output = load(input)()
              if output then
                write(pprint.pformat(output)..'\n')
              end
              --io.flush()
          end
        end
    end
end
end