require('LabVIEW')

local dir = io.popen'cd':read'*l'
local ctrls = GetAllCtrls(dir..'\\Test.Queue.vi')
local names = {}
for k,v in pairs(ctrls) do names[v] = k end
while true do
local r = Queue('Test')
print(names[r[0]]..' '..r[1])
end