-- list of path to search components
searchPath = { ".", "" }

-- list of path to search images
searchImagePath = { ".", "" }

-- show areas on which clicking is possible
showClickableAreas = false

-- returns true if argument is property
function isProperty(value)
    return ("table" == type(value)) and (1 == value.__property)
end


-- create new property table
function createProperty(value)
    if isProperty(value) then
        return value
    end

    local prop = { __property = 1 }
    if "function" == type(value) then
        prop.get = value
    else
        prop.value = value
    end
    return prop
end


-- component grabbed mouse focus (after mouse pressed)
focusedComponent = nil


-- component currently dragging
draggableComponent = nil


-- start component dragging
function dragStart(comp)
    draggableComponent = comp
end

-- stop component dragging
function dragStop(comp)
    draggableComponent = nil
end


-- default mouse down handler
function defaultOnMouseDown(comp, x, y, button, parentX, parentY)
    if (1 == button) and get(comp.movable) then
        local position = get(comp.position)
        comp.dragStartX = parentX
        comp.dragStartY = parentY
        comp.dragStartPosX = position[1]
        comp.dragStartPosY = position[2]
        comp.dragging = true
        return true
    end
    return false
end


-- default mouse up handler
function defaultOnMouseUp(comp, x, y, button, parentX, parentY)
    if 1 == button and get(comp.movable) then
        comp.dragging = false
        return true
    end
    return false
end


-- default mouse move handler
function defaultOnMouseMove(comp, x, y, button, parentX, parentY)
    if rawget(comp, "dragging") and get(comp.movable) then
        local position = get(comp.position)
        position[1] = comp.dragStartPosX + (parentX - comp.dragStartX)
        position[2] = comp.dragStartPosY + (parentY - comp.dragStartY)
        set(comp.position, position)
        return true
    else
        return false
    end
end

-- create basic component
function createComponent(name)
    local data = { 
        components = { },
        componentsByName = { },
        size = { 100, 100 },
        position = createProperty { 0, 0, 100, 100 },
        draw = function (comp) drawAll(comp.components); end,
        update = function (comp) updateAll(comp.components); end,
        name = name,
        visible = createProperty(true),
        movable = createProperty(false),
        onMouseUp = defaultOnMouseUp,
        onMouseDown = defaultOnMouseDown,
        onMouseMove = defaultOnMouseMove,
    }
    data._C = data
    addComponentFunc(data)
    return data
end


-- create component function
function addComponentFunc(component)
    component.component = function(name, tbl)
        if not tbl then -- anonymous subcomponent
            tbl = name
            name = nil
        end
        table.insert(component.components, tbl)
        if name then
            component.componentsByName[name] = tbl
        end
        return tbl
    end
end


-- add component to popup layer
function popup(name, tbl)
    return popups.component(name, tbl)
end


-- main panel
panel = createComponent("panel")

-- popups layer
popups = createComponent("popups")


-- returns simulator float property
function globalPropertyf(name)
    local ref = findProp(name, "float")
    return {
        __property = 1;
        get = function() return getPropf(ref); end;
        set = function(self, value) setPropf(ref, value); end;
    }
end

-- returns simulator int property
function globalPropertyi(name)
    local ref = findProp(name, "int")
    return {
        __property = 1;
        get = function(doNotCall) return getPropi(ref); end;
        set = function(self, value) setPropi(ref, value); end;
    }
end


-- returns value of property
-- traverse recursive properties
function get(property, doNotCall)
    if isProperty(property) then
        if property.get then
            return property:get(doNotCall)
        else
            if isProperty(property.value) then
                return get(property.value, doNotCall)
            else
                return property.value
            end
        end
    else
        if (not doNotCall) and ("function" == type(property)) then
            return property()
        else
            return property
        end
    end
end


-- set value of property
function set(property, value)
    if property.set then
        property:set(value)
    else
        if isProperty(property.value) then
            set(property.value, value)
        else
            property.value = value
        end
    end
end


-- deep copies values from source table to destination table
function mergeTables(dest, src)
    for k, v in pairs(src) do
        if "table" == type(v) then
            if not dest[k] then
                --dest[k] = { }
                dest[k] = v
            else
                mergeTables(dest[k], v)
            end
        else
            dest[k] = v
        end
    end
end


-- convert values from table to properties
function argumentsToProperties(arguments)
    local res = { }
    for k, v in pairs(arguments) do
        if "function" == type(v) then
            res[k] = v
        else
            if isProperty(v) then
                res[k] = v
            else
                res[k] = createProperty(v)
            end
        end
    end
    return res
end

-- update component
function updateComponent(v)
    if v and v.update then
        v:update()
    end
end

-- update all components from table
function updateAll(table)
    for _, v in pairs(table) do
        updateComponent(v)
    end
end

-- draw component
function drawComponent(v)
    if v and get(v.visible) then
        saveGraphicsContext()
        local pos = get(v.position)
        setTranslation(pos[1], pos[2], pos[3], pos[4], v.size[1], v.size[2])
        v:draw()
        restoreGraphicsContext()
    end
end


-- draw all components from table
function drawAll(table)
    for _, v in pairs(table) do
        drawComponent(v)
    end
end

-- try to find key in local table first.
-- look in global table if key doesn't exists in local table
-- try to load component from file if it doesn't exists in global table
function compIndex(table, key)
    local v = rawget(table, key)
    if v ~= nil then 
        return v 
    else
        v = _G[key]
        if nil == v then
            return loadComponent(key)
        else
            return v
        end
    end
end

-- return true if file exists
function isFileExists(fileName)
    local f = io.open(fileName)
    if nil == f then
        return false
    else
        io.close(f)
        return true
    end
end


-- remove extension from file name
function extractFileName(filePath)
    for v = string.len(filePath), -1, 1 do
        if string.sub(filePath, i, i) == '.' then
            return string.sub(filePath, 1, i-1)
        end
    end
    return filePath
end


-- try to find file on search paths
function openFile(fileName)
    local name = extractFileName(fileName)

    for _, v in ipairs(searchPath) do
        local fullName
        local subdir
        if 0 < string.len(v) then
            fullName = v .. '/' .. fileName
            subdir = v .. '/' .. name
        else
            fullName = fileName
            subdir = name
        end

        -- check if it is available at current path
        if isFileExists(fullName) then
            local f, errorMsg = loadfile(fullName)
            if f then
                return f
            else
                print(errorMsg)
            end
        end

        -- check subdir
        local subFullName = subdir .. '/' .. fileName
        if isFileExists(subFullName) then
            local f, errorMsg = loadfile(subFullName)
            if f then
                return f, subdir
            else
                print(errorMsg)
            end
        end
    end

    print("component not found", fileName)
    return nil
end


 -- make deep copy of table
function cloneTable(table)
    local newTable = {}
    for k, v in pairs(table) do
        if "table" == type(v) then
            newTable[k] = cloneTable(v)
        else
            newTable[k] = v
        end
    end
    return newTable
end


-- add properties to component
function setupComponent(component, args)
    mergeTables(component, argumentsToProperties(args))
    setmetatable(component, { __index = compIndex })
    
    component.defineProperty = function(name, dflt)
        if not rawget(component, name) then
            component[name] = createProperty(dflt)
        end
    end

    addComponentFunc(component)
end

-- load component from file and create constructor
function loadComponent(name, fileName)
    print("loading", name)

    if not fileName then
        fileName = name .. ".lua"
    end

    local f, subdir = openFile(fileName)
    if not f then
        print("can't load component", name)
        return nil
    end

    local constr = function(args)
        if subdir then
            addSearchPath(subdir)
        end
        local t = createComponent(name)
        setupComponent(t, args)
        setfenv(f, t)
        f()
        if subdir then
            popSearchPath()
        end
        return t
    end

    _G[name] = constr

    return constr
end


function resizePanel(width, height)
    set(panel.position, { 0, 0, width, height })
end

function resizePopup(width, height)
    set(popups.position, { 0, 0, width, height })
    popups.size[1] = width
    popups.size[2] = height
end


-- load panel from file
-- panel table will be stored in panel global variable
function loadPanel(fileName, panelWidth, panelHeight, popupWidth, popupHeight)
    popups = createComponent("popups")
    popups.position = createProperty { 0, 0, popupWidth, popupHeight }
    popups.size = { popupWidth, popupHeight }

    local c = loadComponent("panel", fileName)
    if not c then
        print("Error loading panel", fileName)
        return nil
    end
    panel = c({position = { 0, 0, panelWidth, panelHeight}})

    return panel
end


-- add path to search path
function addSearchPath(path)
    table.insert(searchPath, 1, path)
    table.insert(searchImagePath, 1, path)
end


function popSearchPath()
    table.remove(searchPath, 1)
    table.remove(searchImagePath, 1)
end


-- add path to images search path
function addSearchImagePath(path)
    table.insert(searchImagePath, 1, path)
end


-- cursor texture and position
cursor = {
    x = 0,
    y = 0,
    shape = nil
}


-- Draw panel on screen
function drawPanelLayer()
    drawComponent(panel)
end


-- draw popup panels
function drawPopupsLayer()
    drawComponent(popups)

    if cursor.shape then
        drawCursor()
    end
end


-- draw panel and popups layers
function drawPanel()
    drawPanelLayer()
    drawPopupsLayer()
end

-- update all component
function update()
    updateComponent(panel)
    updateComponent(popups)
end

-- load texture image
-- loads image and sets texture coords.  It can be called in forms of:
-- loadImage(fileName) -- sets texture coords to entire texture
-- loadImage(fileName, width, height) -- sets texture coords to show 
--    center part of image.  width and height sets size of image part
-- loadImage(fileName, x, y, width, height) - loads specified part of image
function loadImage(fileName, x, y, width, height)
    for _, v in ipairs(searchImagePath) do
        local t = getGLTexture(v .. '/' .. fileName, x, y, width, height)
        if t then
            return t
        end
    end

    local tex = getGLTexture(fileName, x, y, width, height)
    if not tex then
        print("Can't load texture", fileName)
    end
    return tex
end


-- check if coord lay inside rectangle.  rectangle is array of 
-- { x, y, width, height }
function isInRect(rect, x, y)
    local x1 = rect[1]
    local y1 = rect[2]
    local x2 = x1 + rect[3]
    local y2 = y1 + rect[4]
    return (x1 <= x) and (x2 > x) and (y1 <= y) and (y2 > y)
end

-- run handler of component
function runComponentHandler(component, name, mx, my, button, x, y)
    local handler = rawget(component, name)
    if handler then
        return handler(component, mx, my, button, x, y)
    else
        return false
    end
end


-- traverse components and finds best handler with specified name
function runHandler(component, name, x, y, button, path)
    local position = get(component.position)
    local size = component.size
    if (not (position and size)) then
        return false
    end
    local mx = (x - position[1]) * size[1] / position[3]
    local my = (y - position[2]) * size[2] / position[4]
    for _, v in pairs(component.components) do
        if get(v.visible) and isInRect(get(v.position), mx, my) then
            local res = runHandler(v, name, mx, my, button, path)
            if res then
                if path then
                    table.insert(path, component)
                end
                return true
            end
        end
    end
    local res = runComponentHandler(component, name, mx, my, button, x, y)
    if res then
        if path then
            table.insert(path, component)
        end
    end
    return res
end


-- run handler of focused component
function runFocusedHandler(path, name, x, y, button)
    local mx = x
    local my = y
    local px = x
    local py = y
    for i = #path, 1, -1 do
        local c = path[i]
        px = mx
        py = my
        local position = get(c.position)
        local size = get(c.size)
        mx = (mx - position[1]) * c.size[1] / position[3]
        my = (my - position[2]) * c.size[2] / position[4]
    end
    return runComponentHandler(path[1], name, mx, my, button, px, py)
end


-- traverse components and finds best handler with specified name
function runTopHandler(layer, name, x, y, button)
    local path = { }
    if (1 == layer) or (3 == layer) then
        local res = runHandler(popups, name, x, y, button, path)
        if res then
            return true, path
        end
    end
    if (2 == layer) or (3 == layer) then
        return runHandler(panel, name, x, y, button, path), path
    end
end

-- returns value of cursor property for specified layer
function getTopCursorShape(layer, x, y)
    if (1 == layer) or (3 == layer) then
        local cursor = getCursorShape(popups, x, y)
        if cursor then
            return cursor
        end
    end
    if (2 == layer) or (3 == layer) then
        return getCursorShape(panel, x, y)
    end
end

-- returns value of cursor property
function getCursorShape(component, x, y)
    local position = get(component.position)
    local size = component.size
    if (not (position and size)) then
        return nil
    end
    local mx = (x - position[1]) * size[1] / position[3]
    local my = (y - position[2]) * size[2] / position[4]
    for _, v in pairs(component.components) do
        if get(v.visible) and isInRect(get(v.position), mx, my) then
            local res = getCursorShape(v, mx, my)
            if res then
                return res
            end
        end
    end
    return rawget(component, "cursor")
end

-- set shape of cursor
function setCursor(x, y, shape, layer)
    if (2 ~= layer) then
        cursor.x = x
        cursor.y = y
    end
    cursor.shape = get(shape)
end


-- draw cursor shape
function drawCursor()
    if cursor.shape and cursor.shape.shape then
        drawTexture(cursor.shape.shape, 
                cursor.shape.x + cursor.x, cursor.y - cursor.shape.y,
                cursor.shape.width, cursor.shape.height,
                1, 1, 1, 1)
    end
end


-- pressed button number
local pressedButton = 0


-- Called when mouse button was pressed
function onMouseDown(x, y, button, layer)
    pressedButton = button
    local handled, path = runTopHandler(layer, "onMouseDown", x, y, button)
    if handled then
        focusedComponentPath = path
    end
    return handled
end


-- Called when mouse button was released
function onMouseUp(x, y, button, layer)
    if focusedComponentPath then
        local res = runFocusedHandler(focusedComponentPath, "onMouseUp", 
                x, y, button)
        pressedButton = 0
        focusedComponent = nil
        focusedComponentPath = nil
        return res
    else
        return runTopHandler(layer, "onMouseUp", x, y, button)
    end
end

-- Called when mouse click event was processed
function onMouseClick(x, y, button, layer)
    pressedButton = button
    local handled, path = runTopHandler(layer, "onMouseClick", x, y, button)
    if handled then
        focusedComponentPath = path
    end
    return handled
end

-- Called when mouse motion event was processed
function onMouseMove(x, y, layer)
    if focusedComponentPath then
        local res = runFocusedHandler(focusedComponentPath, "onMouseMove", 
                x, y, pressedButton)
        return res
    else
        local cursor = getTopCursorShape(layer, x, y)
        setCursor(x, y, cursor, layer)
        local res = runTopHandler(layer, "onMouseMove", x, y, pressedButton)
        return res or cursor
    end
end


-- create popup movable subpanel hidden by default
function subpanel(tbl)
    local c = createComponent('subpanel')
    set(c.position, tbl.position)
    c.onMouseClick = function() return true; end
    c.size = { tbl.position[3], tbl.position[4] }
    set(c.visible, false)
    set(c.movable, true)
    c.components = tbl.components

    if not rectangle then
        rectangle = loadComponent('rectangle')
    end
    
    if not button then
        button = loadComponent('button')
    end

    table.insert(c.components, 1,
        rectangle { position = { 0, 0, c.size[1], c.size[2] } } )

    c.component('closeButton', button { 
        position = { c.size[1] - 16, c.size[2] - 16, 16, 16};
        image = loadImage('close.png');
        onMouseClick = function()
            set(c.visible, false)
            return true
        end;
    })

    popup(c)

    return c
end

