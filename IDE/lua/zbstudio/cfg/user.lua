--[[--
  Use this file to specify **System** preferences.
  Review [examples](+D:\Develop\ZeroBrane\cfg\user-sample.lua) or check [online documentation](http://studio.zerobrane.com/documentation.html) for details.
--]]--
language = 'cn'
local __dir = require('lfs').currentdir()
local __arch = require('ffi').arch == 'x64' and '64' or ''
path.lua = __dir..[[\..\..\..\lua]]..__arch..'.cmd'
path.torch = __dir..[[\..\..\..\th]]..__arch..'.cmd'
editor.fontname = "Consolas"
editor.fontsize = 12
--styles = loadfile('cfg/tomorrow.lua')('SolarizedDark')
styles = loadfile('cfg/tomorrow.lua')('Tomorrow')
--styles = loadfile('cfg/tomorrow.lua')('TomorrowNightEighties')
--styles = loadfile('cfg/tomorrow.lua')('Monokai')
stylesoutshell = styles -- apply the same scheme to Output/Console windows
styles.auxwindow = styles.text -- apply text colors to auxiliary windows
styles.calltip = styles.text -- apply text colors to tooltips