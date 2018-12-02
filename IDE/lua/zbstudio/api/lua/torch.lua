local function populateAPI(t)
  t = t or {}
  local api = {}
  for k,v in pairs(t) do
    api[k] = {
      type = (type(v) == "function" and "function" or "value"),
      description = "",
      returns = "",
    }
  end
  return api
end

return {
  torch = {
    type = "lib",
    description = "Torch lib",
    childs = populateAPI(pcall(require,'torch') and torch),
  },
  lab = {
    type = "lib",
    description = "Torch5 lab",
    childs = populateAPI(pcall(require,'lab') and lab),
  },
}
