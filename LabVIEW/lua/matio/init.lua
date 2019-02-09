require 'torch'
local ffi = require 'ffi'
local mat = require 'matio.ffi'

local matio = {}
matio.ffi = mat

-- optional setting: loads lua strings instead of CharTensor
matio.use_lua_strings = false

-- compression mode for saving
matio.compression = mat.COMPRESSION_ZLIB

-- mapping of MAT matrix types into torch tensor
local tensor_types_mapper = {
  [mat.C_CHAR]   = {constructor='CharTensor',  sizeof=1},  
  [mat.C_INT8]   = {constructor='CharTensor',  sizeof=1},
  --[mat.C_UINT8]  = {constructor='ByteTensor',  sizeof=1},
  [mat.C_UINT8]  = {constructor='CharTensor',  sizeof=1},
  [mat.C_INT16]  = {constructor='ShortTensor', sizeof=2},
  [mat.C_UINT16] = {constructor='ShortTensor', sizeof=2},
  [mat.C_INT32]  = {constructor='IntTensor',   sizeof=4},
  [mat.C_UINT32] = {constructor='IntTensor',   sizeof=4},
  [mat.C_INT64]  = {constructor='LongTensor',  sizeof=8},
  [mat.C_UINT64] = {constructor='LongTensor',  sizeof=8},
  [mat.C_SINGLE] = {constructor='FloatTensor', sizeof=4},
  --[mat.C_DOUBLE] = {constructor='DoubleTensor',sizeof=8}
  [mat.C_DOUBLE] = {constructor='Tensor',sizeof=8}
}

-- mapping of torch tensor into MAT matrix types
local tensor_types_invmapper = {
  ['torch.CharTensor']   = {c_type=mat.C_CHAR,   t_type=mat.T_CHAR},
  --['torch.ByteTensor']   = {c_type=mat.C_UINT8,  t_type=mat.T_UINT8},
  ['torch.ShortTensor']  = {c_type=mat.C_INT16,  t_type=mat.T_INT16},
  ['torch.IntTensor']    = {c_type=mat.C_INT32,  t_type=mat.T_INT32},
  ['torch.LongTensor']   = {c_type=mat.C_INT64,  t_type=mat.T_INT64},
  ['torch.FloatTensor']  = {c_type=mat.C_SINGLE, t_type=mat.T_SINGLE},
  --['torch.DoubleTensor'] = {c_type=mat.C_DOUBLE, t_type=mat.T_DOUBLE}
  ['torch.Tensor'] = {c_type=mat.C_DOUBLE, t_type=mat.T_DOUBLE}
}

--mapping of lua types to necessary variables for creating matlab variables
local lua_types_invmapper = {

  ['number']   = { c_type=mat.C_DOUBLE,   
    t_type=mat.T_DOUBLE, 
    rank = 2, 
    dims_fn = function() return ffi.new("uint64_t [2]", {1,1}) end,
    output_fn = function(data) return ffi.new("double [1]", data) end,
    flag = 0
  },

  ['string']   = { c_type=mat.C_CHAR,
    t_type=mat.T_UINT8,
    rank = 2,
    dims_fn = function(data) return ffi.new("uint64_t [2]", {1, data:len()}) end,
    output_fn = function(data) 
      return ffi.new("char [" .. (data:len()+1) .. "]", data) 
    end,
    flag = 0
  },

  ['boolean']  = { c_type=mat.C_UINT8,
    t_type=mat.T_UINT8,
    rank = 2,
    dims_fn = function() return ffi.new("uint64_t [2]", {1,1}) end,
    output_fn = function(data) return ffi.new("uint8_t [1]", data and 1 or 0) end,
    flag = mat.F_LOGICAL
  }
}

local function make_struct_var_for_table(data, name)
  local structVar
  local fields  = {}
  local vars = {}
  local tableSize = 0
  for k,v in pairs(data) do
    tableSize = tableSize + 1
    table.insert(fields, k)
    table.insert(vars, matio.__make_var(v,k))
  end
  --make the matlab struct here
  local rank = 2
  local dims = ffi.new("uint64_t [2]", {1, 1})
  local cFields = ffi.new('const char* [' .. #fields ..']', fields)
  structVar = mat.varCreateStruct(name, rank, dims, cFields, #fields)

  for i = 1, #fields do
    mat.varSetStructFieldByName(structVar, fields[i], 0, vars[ i ])
  end

  return structVar
end

local function make_var_for_tensor(tensor, name)

  -- get type of tensor
  local mapper = tensor_types_invmapper[tensor:type()]
  local c_type, t_type
  if mapper then
    c_type = mapper.c_type
    t_type = mapper.t_type
  else
    print('Unsupported type of tensor: ' .. tensor:type())
    return
  end

  -- every vector is at least 2d in matlab
  if tensor:dim() == 1 then
    tensor = tensor:view(-1,1)
  end
  local dims = tensor:dim()
  local sizes = tensor:size()

  -- transpose, because matlab is column-major
  if dims > 1 then
    for i=1,math.floor(dims/2) do
      tensor=tensor:transpose(i, dims-i+1)
    end
    tensor = tensor:contiguous()
  end

  local var = mat.varCreate(name, c_type, t_type, dims,
    sizes:data(), tensor:data(), 0)

  return var
end

function matio.__make_var(data, name)
  local mapper = lua_types_invmapper[type(data)]
  if mapper then
    c_type = mapper.c_type
    t_type = mapper.t_type
    rank = mapper.rank
    dims = mapper.dims_fn(data)
    output = mapper.output_fn(data)
    flag = mapper.flag
    local var = mat.varCreate(name, c_type, t_type, rank, dims, output, flag)
    return var
  end

  -- if we've reached this code, we're type is either: table, torch Tensor, 
  -- or unsupported types
  if torch.isTensor(data) then
    return make_var_for_tensor(data, name)
  elseif type(data) == 'table' then
    return make_struct_var_for_table(data, name)
  else
    error('Unsupported type: ' .. type(data))
  end

end

local function load_tensor(file, var)
  local out
  local sizeof
  -- type check
  local mapper = tensor_types_mapper[tonumber(var.class_type)]
  if mapper then
    out = torch[mapper.constructor]()
    sizeof = mapper.sizeof   
  else
    print('Unsupported type of tensor: ' .. var.class_type)
    return nil
  end

  -- rank check
  if var.rank > 8 or var.rank < 1 then
    print('Rank of input matrix is invalid: ' .. var.rank)

    return nil
  end

  local sizes = {}
  for i=0,var.rank-1 do
    table.insert(sizes, tonumber(var.dims[i]))
  end
  -- reverse initialize because of column-major order of matlab
  --local revsizes = {}
  local revsizes = torch.LongStorage(var.rank)
  for i=1, var.rank do
    --revsizes[i] = sizes[var.rank-i+1]
    revsizes[i] = sizes[i]
  end

  -- resize tensor
  --out:resize(torch.LongStorage(revsizes))
  out:resize(revsizes)

  -- memcpy
  ffi.copy(out:data(), var.data, out:nElement() * sizeof);
  --[[local mem = out:storage()
  local ptr = ffi.cast('double*',var.data)
  for i=1, out:nElement() do
    mem[i] = ptr[i-1]
  end]]
  mat.varFree(var)

  -- transpose, because matlab is column-major
  --[[if out:dim() > 1 then
    for i=1,math.floor(out:dim()/2) do
      out=out:transpose(i, out:dim()-i+1)
    end
  end]]
  return out
end

local function load_struct(file, var)
  local out = {}
  local n_fields = mat.varGetNumberOfFields(var)
  local field_names =  mat.varGetStructFieldnames(var)
  for i=0,n_fields-1 do
    local field_name = ffi.string(field_names[i])
    local field_value = mat.varGetStructFieldByIndex(var, i, 0)
    out[field_name] = mat_read_variable(file, field_value)
  end  
  return out
end

local function load_cell(file, var)
  local out = {};
  local index = 0
  while true do
    local cell = mat.varGetCell(var, index) 
    if cell == nil then
      break
    end 
    index = index + 1
    -- using array index starting at 1 (lua standard)
    out[index] = mat_read_variable(file, cell)
  end
  return out
end

local function load_string(file, var)
  return var.data~=nil and ffi.string(var.data,var.nbytes) or ''
end

function mat_read_variable(file, var) 

  -- will load C_CHAR sequence as a lua string, instead of torch tensor
  if matio.use_lua_strings == true and var.class_type == mat.C_CHAR then
    return load_string(file, var)
  end

  if var.data == nil then
    return nil
  end

  if tensor_types_mapper[tonumber(var.class_type)] then
    return load_tensor(file, var)
  end

  if var.class_type == mat.C_CELL then
    return load_cell(file, var)
  end

  if var.class_type == mat.C_STRUCT then
    local nelems = 1
    for i=0,var.rank-1 do nelems = nelems*tonumber(var.dims[i]) end

    if nelems>1 and var.rank==2 then
      local array = {}
      for i=0,tonumber(var.dims[0])-1 do
        array[i+1] = {}
        for j=0,tonumber(var.dims[1])-1 do
          local fieldvar = mat.varGetStructsLinear(var,i+j*tonumber(var.dims[0]),1,1,0)
          array[i+1][j+1] = load_struct(file, fieldvar)
          mat.varFree(fieldvar);
        end
      end
      return array
    elseif var.rank>2 then
      print('Multidimensional structs currently not implemented.')
    else
      return load_struct(file, var)
    end
  end

  print('Unsupported variable type: ' .. tonumber(var.class_type))
  return nil

end

--[[
    Load all variables (or just the requested ones) from a given .mat file 
    It supports structs, cell arrays and tensors of the appropriate types.
    Sequences of characters can optionally be loaded as lua strings instead
    of torch CharTensors.

    matio.load(filename, variableName)
    matio.load(filename)
    matio.load(filename,{'var1','var2'})

    Example:
    local img1 = matio.load('data.mat', 'img1')
--]]
function matio.load(filename, name)

  local file = mat.open(filename, mat.ACC_RDONLY);
  if file == nil then
    print('File could not be opened: ' .. filename)
    return
  end

  local names
  local string_name
  -- if name is not given then load everything
  if not name then
    names = {}
  elseif type(name) == 'string' then
    names = {name}
    string_name = true
  elseif type(name) == 'table' then
    names = name
  end

  if #names == 0 then
    -- go over the file and get the names
    local var = mat.varReadNextInfo(file)
    while var ~= nil do
      local var_name_str = ffi.string(var.name)
      table.insert(names, var_name_str)
      var = mat.varReadNextInfo(file)
    end
  end

  if #names == 0 then
    print('No variables in this file')
    return
  end

  local out = {}
  for i, varname in ipairs(names) do
    local var = mat.varRead(file, varname);

    if var ~= nil then
      local x = mat_read_variable(file, var)
      if x ~= nil then
        out[varname] = x
      end
    else   
      print('Could not find variable with name: ' .. varname .. ' in file: ' .. ffi.string(mat.getFilename(file)))
    end

  end

  mat.close(file)

  -- conserve backward compatibility
  if #names == 1 and string_name then
    return out[names[1]]
  else
    return out
  end
end

--[[
    Allows saving the following formats to a .mat file:
      1. a single torch Tensor (gets named 'x' when loaded in
      MATLAB)

      2. a dictionary with string keys and values of types:
        - number
        - string
        - boolean
        - Torch tensor
        - dictionary meeting these same requirements, although
        anything below the top-level gets saved as a struct. 
        See difference between examples 2 and 3 below if this
        is confusing.

    It supports all Torch tensor types (except cuda tensors).
    If you provide a table, it saves all tensors in the table
    as separate variables, whose name in the .mat file is the
    key of the table.

    By default, save the tensors in MAT5 format using ZLIB
    compression. The compression can be changed by setting 
    matio.compression variable to matio.ffi.COMPRESSION_NONE
    or matio.ffi.COMPRESSION_ZLIB

    EXAMPLE 1:
    =========
    save in lua:
      tensor = torch.Tensor(3,4):zero()
      matio.save('test1.mat', tensor)

    load in matlab:
      load('test1.mat');

      whos()
      Name    Size  
      -------------
      x       3x4

    EXAMPLE 2:
    ==========
    save in lua: 
      tensor1 = torch.Tensor(3,4):zero()
      tensor2 = torch.Tensor(4,5,6):zero()
      matio.save('test2.mat', {name1=tensor1, name2=tensor2})

    load in matlab:
      load('test2.mat');

      whos()
      Name    Size  
      -------------
      name1   3x4
      name2   4x5x6

    EXAMPLE 3:
    ==========
    save in lua: 
      matio.save('test3.mat', {name = 'rodrigo', info = {height = 10, is_tired = true}})

    load in matlab:
      load('test3.mat');

      whos()

      Name      Size            Bytes  Class     Attributes
      -----------------------------------------------------
      info      1x1               361  struct              
      name      1x7                14  char                

      % view struct
      info

      info = 

            height: 10
            is_tired: 1


--]]
function matio.save(filename, data)
  local file = mat.createVer(filename, nil, mat.FT_MAT5)

  local compression = matio.compression
  if type(data) == 'table' then
    for k,v in pairs(data) do
      local var = matio.__make_var(v,k,compression)
      if var then
        mat.varWrite(file, var, compression)
        mat.varFree(var)
      else
        -- closing file and asserting error
        mat.close(file)
        error('only tensor or table of tensors supported!')
      end
    end
  elseif torch.isTensor(data) then
    local var = make_var_for_tensor(data, 'x', compression)
    mat.varWrite(file, var, compression)
    mat.varFree(var)
  else
    -- closing file and asserting error
    mat.close(file)
    error('only tensor or table of tensors supported!')
  end

  mat.close(file)
end

return matio
