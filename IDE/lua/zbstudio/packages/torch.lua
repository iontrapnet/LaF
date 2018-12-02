-- Copyright 2015 Paul Kulchenko, ZeroBrane LLC; All rights reserved
-- Path handling for Torch and QLua is based on Torch/QLua interpreters from ZBS-torch by Soumith Chintala

local win = ide.osname == 'Windows'
local sep = win and ';' or ':'

local debinit = [[
local mdb = require('mobdebug')
local line = mdb.line
mdb.line = function(v)
  local r = line(v)
  return type(v) == 'userdata' and loadstring("return "..r)() or r
end]]

local function fixBS(s) -- string callback to eliminate backspaces from Torch output
  while s:find("\b") do
    s = s
      :gsub("[^\b\r\n]\b","") -- remove a backspace and a previous character
      :gsub("^\b+","") -- remove all leading backspaces (if any)
      :gsub("([\r\n])\b+","%1") -- remove a backspace and a previous character
  end
  return s
end

local function findCmd(cmd, env)
  local path = (os.getenv('PATH') or '')..sep
             ..(env or '')..sep
             ..(os.getenv('HOME') and os.getenv('HOME') .. '/bin' or '')
  local paths = {}
  local res
  for p in path:gmatch("[^"..sep.."]+") do
    res = res or GetFullPathIfExists(p, cmd)
    table.insert(paths, p)
  end

  if not res then
    ide:Print(("Can't find %s in any of the folders in PATH or TORCH_BIN: "):format(cmd)
      ..table.concat(paths, ", "))
    return
  end
  return res
end

local torchInterpreter = {
  name = "Torch",
  description = "Torch machine learning package",
  api = {"baselib", "torch"},
  frun = function(self,wfilename,rundebug)
    -- check if the path is configured
    local torch = ide.config.path.torch or findCmd(win and 'th.cmd' or 'th64.cmd', os.getenv('TORCH_BIN'))
    if not torch then return end

    local filepath = wfilename:GetFullPath()
    if rundebug then
      ide:GetDebugger():SetOptions({runstart = ide.config.debugger.runonstart == true, init = debinit})
      -- update arg to point to the proper file
      rundebug = ('if arg then arg[0] = [[%s]] end '):format(filepath)..rundebug

      local tmpfile = wx.wxFileName()
      tmpfile:AssignTempFileName(".")
      filepath = tmpfile:GetFullPath()
      local f = io.open(filepath, "w")
      if not f then
        ide:Print("Can't open temporary file '"..filepath.."' for writing.")
        return
      end
      f:write("io.stdout:setvbuf('no'); " .. rundebug)
      f:close()
    else
      -- if running on Windows and can't open the file, this may mean that
      -- the file path includes unicode characters that need special handling
      local fh = io.open(filepath, "r")
      if fh then fh:close() end
      if win and pcall(require, "winapi")
      and wfilename:FileExists() and not fh then
        winapi.set_encoding(winapi.CP_UTF8)
        filepath = winapi.short_path(filepath)
      end
    end

    -- doesn't need set environment with setEnv as it's already done in onInterpreterLoad
    local params = ide.config.arg.any or ide.config.arg.torch or ''
    local uselua = wx.wxDirExists(torch)
    local cmd = ([["%s" "%s" %s]]):format(
      uselua and ide:GetInterpreters().luadeb:GetExePath("") or torch, filepath, params)
    -- CommandLineRun(cmd,wdir,tooutput,nohide,stringcallback,uid,endcallback)
    return CommandLineRun(cmd,self:fworkdir(wfilename),true,false,fixBS,nil,
      function() if rundebug then wx.wxRemoveFile(filepath) end end)
  end,
  hasdebugger = true,
  fattachdebug = function(self)
    ide:GetDebugger():SetOptions({
        runstart = ide.config.debugger.runonstart == true,
        init = debinit
    })
  end,
  scratchextloop = true,
  takeparameters = true,
}

return {
  name = "Torch",
  description = "Implements integration with torch environment.",
  author = "Paul Kulchenko",
  version = 0.58,
  dependencies = "1.40",

  onRegister = function(self)
    ide:AddInterpreter("torch", torchInterpreter)
  end,
  onUnRegister = function(self)
    ide:RemoveInterpreter("torch")
  end,

  onInterpreterLoad = function(self, interpreter)
    if interpreter:GetFileName() ~= "torch" then return end
    local torch = ide.config.path.torch or findCmd(win and 'th.cmd' or 'th64.cmd', os.getenv('TORCH_BIN'))
    if not torch then return end
  end,
  onInterpreterClose = function(self, interpreter)
    if interpreter:GetFileName() ~= "torch" then return end
  end,
}
