--local luastate = require'luastate53'
--local lua = luastate.open()
--lua:openlibs()

if select('#',...) > 0 then
--lua:loadstring[[
-- perms are items to be substituted at serialization
perms = { [_ENV] = "_ENV", [coroutine.yield] = 1, [pcall] = 2, [xpcall] = 3 }
-- the functions that we want to execute as a coroutine
function foo()
    local someMessage = 'hello from a long dead variable!'
    local i = 4
    bar(someMessage)
    print(i)
end
function test(arg)
    coroutine.yield()
    error(arg, 0)
end
function bar(someMessage)
    print('enter bar:')
    local res, err = pcall(test, someMessage)
    print('call test err:', err)
    return res
end
-- create and start the coroutine
co = coroutine.create(foo)
coroutine.resume(co)
-- the coroutine has now stopped at yield. so we can
-- persist its state with eris
buf = eris.persist(perms, co)
-- save the serialized state to a file
outfile = io.open('eris.bin', 'wb')
outfile:write(buf)
outfile:close()
--]]
else

--lua:loadstring[[
-- perms are items to be substituted at serialization
-- (reverse the key/value pair that you used to serialize)
perms = { _ENV = _ENV, [1] = coroutine.yield, [2] = pcall, [3] = xpcall }
-- get the serialized coroutine from disk
infile, err = io.open('eris.bin', 'rb')
if infile == nil then
    error('While opening: ' .. (err or 'no error'))
end
buf, err = infile:read('*a')
if buf == nil then
    error('While reading: ' .. (err or 'no error'))
end
infile:close()
-- deserialize it
co = eris.unpersist(perms, buf)
-- and run it
coroutine.resume(co)
--]]
end
--lua:call()

--lua:loadstring[[
javavm = require'javavm'
javavm.create'-Djava.class.path=JNLua.jar'
System = java.require("java.lang.System")
System.out:println("Hello, world!")

require'ilua'.repl()
--]]
--lua:call()

--lua:close()