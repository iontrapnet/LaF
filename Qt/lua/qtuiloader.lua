require 'qtcore'
require 'qtgui'
require 'qtuitools'
module('qtuiloader', package.seeall)

local mLoader = QUiLoader.new()
local mObjectsByName = {}
mLoader.createWidget = function (loader, _className, parent, name)


        local className = _className:toUtf8()
        local widget
        local class = _G[className]


        if class then
                widget = class.new(parent)
        else
                error('Cannot create object: '..tostring(className))
        end
        widget.__isWidget = true
        widget:setObjectName(name)
        local n = name:toUtf8()
        mObjectsByName[n] = widget
        return widget
end
mLoader.createLayout = function (loader, _className, parent, name)
    local parentWidget = parent.__isWidget and parent or nil
    local parentLayout = parent.__isLayout and parent or nil
        local function create(class)
                return parentWidget and class.new(parentWidget) or class.new()
        end
        local layout

        local className = _className:toUtf8()

        local class = _G[className]
        if class then
                layout = create(class)
        else
                error('Cannot create layout: '..tostring(className))
        end

        layout.__isLayout = true
        layout:setObjectName(name)
        local n = name:toUtf8()
        mObjectsByName[n] = layout
        return layout
end
function load(fileName)
        local file = QFile.new(fileName)
        file:open({'ReadOnly'})
        return mLoader:load(file)
end

function findObjectByName(name)
        return mObjectsByName[name]
end