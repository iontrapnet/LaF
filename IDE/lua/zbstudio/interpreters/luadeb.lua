dofile 'interpreters/luabase.lua'

local interpreter = MakeLuaInterpreter()
interpreter.api = {"baselib","luajit2","wxwidgets","torch5"}

return interpreter