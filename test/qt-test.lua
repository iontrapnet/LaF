--require 'qtcore'
--require 'qtgui'
--require 'qtuitools'
require 'qtuiloader'

app = QApplication.new(select('#',...) + 1, {'lua', ...})

--loader = QUiLoader.new()
--file = QFile.new('test.ui')
--win = QMainWindow.new()
--loader:load(file,win)
win = qtuiloader.load('test.ui')

horizontalSlider = win:findChild('horizontalSlider')
horizontalSlider:connect('2valueChanged(int)',function(self)
    print(self:value())
end)

win:show()
app.exec()