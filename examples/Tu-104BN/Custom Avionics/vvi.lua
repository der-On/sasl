size = { 220, 220 }

-- initialize component property table
defineProperty("vvi", globalPropertyf("sim/flightmodel/position/vh_ind_fpm"))

-- background image
defineProperty("background", loadImage("variometer.png", size[1], size[2]))

-- needle image
defineProperty("longNeedleImage", loadImage("needle.tga"))

-- maximum vertical speed gauge can display (in meters per second)
defineProperty("maxVvi", 10)

-- variometer consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- variometer needle
    needle {
        position = { 0, 0, size[1], size[2] },
        image = get(longNeedleImage),
        angle = function() 
            local v = get(vvi) * 0.00508
            local max = get(maxVvi)
            if max < v then
                v = max
            elseif -max > v then
                v = -max
            end
            return v / max * 180.0 - 90.0
        end
    },

}

