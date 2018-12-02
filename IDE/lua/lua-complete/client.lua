local socket = require("socket")
local cjson = require("cjson")
local client = {}

-- send any generic message (a request or not) to the server
local function sendMessage(message, host, port)
     -- create a tcp connection
    -- print("creating tcp connection", host, port)
    local tcp = assert(socket.tcp())
    tcp:connect(host, port);

    -- print("Messagelen", #message)
    tcp:send(message .. "\n")
    -- tcp:send("HERP\n")

    -- send it out
    -- the message is fully recieved when the status is not closed.
    -- the loop after the message is recieved sets the s and partial values to nil
    -- print("trying to receive message")
    local s, status, partial, response
    while status ~= "closed" and status ~= 'Socket is not connected' do
        s, status, partial = tcp:receive()
        if response == nil and (s or partial) then
            response = s or partial
        end
        -- print(s or partial)
    end
    tcp:close()

    return response
end

function client.sendRequest(filename, src, cursorOffset, packagePath, port)
    -- default host and port
    local host = "127.0.0.1"

    -- print("creating request")
    -- the main request body
    local request = {
        filename = filename,
        src = src,
        cursor = cursorOffset,
        packagePath = packagePath
    }

    -- SEND IT!
    local packedMessage = cjson.encode(request)
    local packedResponse = sendMessage(packedMessage, host, port)

    -- print("passing back request")
    local ok,response = pcall(cjson.decode,packedResponse)
    return ok and response
end

function client.shutdown(port)
    print("Shutting down server on port: " .. port)
    local host = "127.0.0.1"
    local value = sendMessage("shutdown", host, port)
    if value == "OK" then
        print(value)
        os.exit(0)
    else
        print("No response from server. Might already by closed.")
        os.exit(1)
    end
end

return client
