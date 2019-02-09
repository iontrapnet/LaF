local server = require "zerorpc.server"
local S = server.new("tcp://127.0.0.1:4242")

pprint = require'pprint'

local echo = function(args)
    pprint(args)
    return args
end

S:add("echo",echo)
S:run()
