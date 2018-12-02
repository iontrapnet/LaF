local co = coroutine.create(function()
    local line = ''
    for i=1,3 do
        line = coroutine.yield(line..i)
    end
end)

function test()
    local ok = true
    local line, ret = '', ''
    while ok do
      ok, line = coroutine.resume(co, ret)
      ret = coroutine.yield(line..ret)
    end
end