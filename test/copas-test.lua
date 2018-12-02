-- create client:

local websocket = require'websocket'
--local client = websocket.client.sync({timeout=2})
local client = websocket.client.copas({timeout=2})
local copas = require'copas'

copas.addthread(function()
-- connect to the server:

local ok,err = client:connect('wss://echo.websocket.org','echo')

if not ok then
   print('could not connect',err)
end

-- send data:

local ok = client:send('hello')
if ok then
   print('msg sent')
else
   print('connection closed')
end

-- receive data:

local message,opcode = client:receive()
if message then
   print('msg',message,opcode)
else
   print('connection closed')
end

-- close connection:

local close_was_clean,close_code,close_reason = client:close(4001,'lost interest')
end)

local asynchttp = require("copas.http").request

local list = {
  "http://www.baidu.com",
  "http://www.qq.com",
  "http://www.apple.com",
}

local handler = function(host)
  res, err = asynchttp(host)
  print("Host done: "..host..' '..#res)
end

for _, host in ipairs(list) do copas.addthread(handler, host) end

copas.loop()