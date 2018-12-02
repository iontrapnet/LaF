local server = require "zerorpc.server"
local S = server.new("tcp://127.0.0.1:4242")

local echo = function(args)
    table.foreach(args,print)
    return args
end

S:add("echo",echo)
S:run()
