local client = require "zerorpc.client"
local S = client.new("tcp://127.0.0.1:4242")

pprint = require'pprint'

r = S:call("echo",{1,2,3})
pprint(r)
S:close()