--[[
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
]]--
local itorch = {}

require 'torch'
require 'lab'

torch.DoubleTensor = torch.Tensor
torch.isTensor = function(x) return torch.typename(x)~=nil end
torch.type = function(x) return torch.typename(x) or type(x) or 'nil' end
getmetatable(torch.DoubleStorage).totable = function (self)
    local d = self:data()
    local r = {}
    for i=1,self:size() do
        r[i]=d[i-1]
    end
    return r
end
getmetatable(torch.Tensor).contiguous = function(self) return self end
getmetatable(torch.LongTensor).zero = function (self) self:fill(0) return self end
getmetatable(torch.LongTensor).add = function (self,x) self:apply(function (self) return self+x end) end 

torch.randn = lab.randn
torch.linspace = lab.linspace

return itorch
