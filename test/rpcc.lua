local client = require "zerorpc.client"
local S = client.new("tcp://127.0.0.1:4242")

r = S:call("echo",{1,2,3})
if type(r) == 'table' then
    table.foreach(r,print)
else
    print(r)
end
--r = S:call("echo",{1,2,a=3})
--print(r)
--table.foreach(r,print)
S:close()