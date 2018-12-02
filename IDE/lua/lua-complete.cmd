::rem:: --[[
@setlocal&  set luafile="%~f0" & if exist "%~f0.cmd" set luafile="%~f0.cmd"
@%~dp0..\..\lua %luafile% %*
::rem:: ]]

local VERSION = "0.0.0"
local ArgParse = require("lua-complete.argparse")
local lfs = require("lfs")

-- function overrides
function ArgParse.printFullHelp(serverArgs, clientArgs)
    print("lua-complete mode [options]")
    print("mode: (client | server)")
    print("\nServer options")
    print("lua-complete server [-p <port>]")
    serverArgs:print()

    print("\nClient options")
    print("lua-complete client -c <cursor> [-f <file>] [-p <port>] [-r <packagePath>] [-i] [-x]")
    clientArgs:print()

    os.exit()
end

function ArgParse.printShortHelp(code)
    print("see 'lua-complete --help' for more information")
    os.exit(code or 1)
end

function ArgParse.printVersion()
    print("lua-complete v" .. VERSION)
    print("a lua autocompletion helper")
    ArgParse.printShortHelp(0)
end

-- load an entire file into memory
local function loadFile(textFile)
    local file = io.open(textFile, "r")
    if not file then error("File not found at " .. textFile) end
    local allLines = file:read("*all")
    file:close()
    return allLines
end

local function main(args)

    local serverArgs = ArgParse:new()
    serverArgs:addArg("p", "port", "port number to run the server on (default: 51371)", true, "51371")

    local clientArgs = ArgParse:new()
    clientArgs:addArg("f", "filename", "path to file for autocompletion", false)
    clientArgs:addArg("c", "cursor", "cursor offset (in bytes) of variable to analyze", true)
    clientArgs:addArg("p", "port", "port number to connect to (default: 51371)", true, "51371")
    clientArgs:addArg("r", "packagePath", "path to load packages from (default: current dir)", true, lfs.currentdir())
    clientArgs:addFlag("x", "shutdown", "shutdown the server", true)
    clientArgs:addFlag("i", "stdin", "read file from stdin. filename argument now used for cache filename")

    if ArgParse.commonHelp[args[1]] then
        ArgParse.printFullHelp(serverArgs, clientArgs)
    end

    -- handle version
    if ArgParse.versions[args[1]] then
        ArgParse.printVersion()
    end

    local commands
    if args[1] == "client" then
        local client = require("lua-complete.client")
        commands = clientArgs:parse(args, 2)
        -- check for the override
        if commands["shutdown"] then
            client.shutdown(commands.port)
        else
            -- keep track of the response
            local response
            if commands.stdin then
                -- read all of stdin
                local stdin = io.read("*all")
                commands.filename = commands.file or "stdin"
                response = client.sendRequest(
                    commands.filename, stdin,
                    commands.cursor, commands.packagePath,
                    tonumber(commands.port)
                )
            else
                response = client.sendRequest(
                    commands.filename, loadFile(commands.filename),
                    commands.cursor, commands.packagePath,
                    tonumber(commands.port)
                )
            end
            print(response.type)
            if response.type == "function" then
                for _, v in ipairs(response.info) do
                    print(v)
                end

            elseif response.type == "table" then
                for _, v in ipairs(response.info) do
                    if v.type == "function" and v.paramList then
                        print(string.format("%s: %s | %s", v.name, v.type, table.concat(v.paramList, ' ')))
                    else
                        print(string.format("%s: %s", v.name, v.type))
                    end
                end
            end
        end

    -- fire up the server
    elseif args[1] == "server" then
        local server = require("lua-complete.server")
        commands = serverArgs:parse(args, 2)
        print(commands.port)
        server.main(tonumber(commands.port))

    else
        print("Mode must be either 'client' or 'server'")
        ArgParse.printShortHelp()
    end

end

main({...})
