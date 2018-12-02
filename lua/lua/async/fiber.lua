-- c lib / bindings for libuv
local uv = require 'luv'

-- list of fibers:
local fibers = {}

-- new fiber:
local fiber = function(func)
   -- create coroutine:
   local co = coroutine.create(func)
   -- store:
   local f = {
      co = co,
      resume = function(...)
         return assert(coroutine.resume(co, ...))
      end,
      yield = function(...)
         if co == coroutine.running() then
            return coroutine.yield(...)
         else
            print('cannot pause a fiber if not in it!')
         end
      end
   }
   fibers[co] = f
   -- start:
   local reply = {f.resume()}
   -- run GC:
   for co,f in pairs(fibers) do
      if coroutine.status(co) == 'dead' then
         fibers[co] = nil
      end
   end
   -- return:
   return f, unpack(reply)
end

-- return context:
local context = function()
   local co = coroutine.running()
   if not co or not fibers[co] then
      print('async.fiber.current() : not currently in a managed fiber')
      return nil
   end
   return fibers[co]
end

-- wait:
local wait = function(funcs,args,limit,cb)
   -- current fiber:
   local f = context()

   if type(limit) == 'function' then
      cb = limit
      limit = 64
   elseif limit == nil then
      limit = 64
   end

   -- default cb
   cb = cb or function(...) return ... end

   -- onle one func?
   if type(funcs) == 'function' then
      funcs = {funcs}
      args = {args}
   end

   -- no args?
   args = args or {}

   -- run all functions:
   local results = {}
   local index = 1

   while true do
      local arg = args[index] or {}
      local index2 = index
      table.insert(arg, function(...)
         results[index2] = {cb(...)}
         f.resume()
      end)
      funcs[index](unpack(arg))
      index = index + 1
      if index > limit or not funcs[index] then break end
   end

   -- wait on all functions to complete
   for i = 1,#funcs do
      f.yield()
      if funcs[index] then
         local index2 = index
         local arg = args[index] or {}
         table.insert(arg, function(...)
            results[index2] = {cb(...)}
            f.resume()
         end)
         funcs[index](unpack(arg))

         index = index + 1
      end
   end

   -- return results
   if #results == 1 then
      return unpack(results[1])
   else
      return results
   end
end

-- sleep
local function sleep(millis)
   wait(require('async.time').setTimeout, {millis})
end

-- sync: magic wrapper that transforms any function into a syncable one
local function sincify(self,key)
   local async = require 'async'
   local afunc = async[key] or async.process[key] or async.fs[key] or async.curl[key]
   return function(...)
      return wait(afunc, {...})
   end
end
local sync = {}
setmetatable(sync, {
   __index = sincify
})

-- pkg
local pkg = {
   new = fiber,
   fibers = fibers,
   context = context,
   sleep = sleep,
   wait = wait,
   sync = sync,
}

-- metatable
setmetatable(pkg, {
   __call = function(self,func)
      return self.new(func)
   end
})

-- return
return pkg
