--[[
	HMAC implementation
	http://tools.ietf.org/html/rfc2104
	http://en.wikipedia.org/wiki/HMAC

	hmac.compute(key, message, hash_function, blocksize, [opad], [ipad]) -> HMAC string, opad, ipad

	hmac.new(hash_function, block_size) -> function(message, key) -> HMAC string

]]

local string = string
local sha2 = require 'sha2'
local ffi = require 'ffi'
local table = table
local unpack = unpack
local setmetatable = setmetatable

module(...)

--any hash function works, md5, sha256, etc.
--blocksize is that of the underlying hash function (64 for MD5 and SHA-256, 128 for SHA-384 and SHA-512)
function compute(key, message, hash, blocksize, opad, ipad)
    if #key > blocksize then
        key = hash(key) --keys longer than blocksize are shortened
    end
    key = key .. string.rep('\0', blocksize - #key) --keys shorter than blocksize are zero-padded
    opad = opad or sha2.exor(key, string.rep(string.char(0x5c), blocksize))
    ipad = ipad or sha2.exor(key, string.rep(string.char(0x36), blocksize))
	return hash(opad .. hash(ipad .. message)), opad, ipad --opad and ipad can be cached for the same key
end

local config = {
sha256 = {sha2.sha256,64},
sha384 = {sha2.sha384,128},
sha512 = {sha2.sha512,128},
--md5 = {require('md5').sum,64},
}

local mt = {}
mt.__index = mt

local tohex = function(buf, n)
  local x = ffi.cast("unsigned char*", buf)
  local t = {}
  for i=0,n-1 do t[i+1] = string.format("%02x", x[i]) end
  return table.concat(t)
end

function mt:update(m)
    self.data = self.data .. m
end

function mt:final()
    return tohex(compute(self.key,self.data,unpack(config[self.hash])),32)
end

function new(hash, key)
    local r = {hash = hash,key = key,data = ''}
    setmetatable(r, mt)
    return r
end