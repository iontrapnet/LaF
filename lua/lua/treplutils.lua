return {
isatty = function ()
    local ffi = require('ffi')
    ffi.cdef[[int _isatty(int);]]
    return ffi.C._isatty(1)
end
}