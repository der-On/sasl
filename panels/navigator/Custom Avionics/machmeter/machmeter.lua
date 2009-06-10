size = { 256, 256 }

-- mach number
defineProperty("mach", globalPropertyf("sim/flightmodel/misc/machno"))

-- background image
defineProperty("background", loadImage("machmeter.png", size[1], size[2]))

-- meters needle image
defineProperty("needleImage", loadImage("needle.tga"))


-- machmeter components defnition
components = {
    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- mach number needle
    -- scale is combination of linear function
    needle {
        position = { 30, 30, size[1]-60, size[2]-60 },
        image = get(needleImage),
        angle = function()
            local angle
            local machNumber = get(mach)
            if 0.25 >= machNumber then
                return 180
            elseif 0.3 >= machNumber then
                return 180 + 70.0 * (machNumber - 0.25) * 20.0
            elseif 0.9 > machNumber then
                return 250 + 220 * (machNumber - 0.3) * 1.66666667
            else
                return 470
            end
        end
    },
}

