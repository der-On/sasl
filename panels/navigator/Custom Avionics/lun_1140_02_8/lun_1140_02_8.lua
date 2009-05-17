size = { 256, 256 }

-- initialize component property table
defineProperty("vvi", globalPropertyf("sim/cockpit2/gauges/indicators/vvi_fpm_pilot"))

-- background image
defineProperty("background", loadImage("lun-1140-02-08-bg.png", size[1], size[2]))

-- meters needle image
defineProperty("needleImage", loadImage("needle.tga"))

components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- vvi needle
    needle {
        position = { 30, 30, size[1]-60, size[2]-60 },
        image = get(needleImage),
        angle = function() 
            local v = get(vvi) * 0.00508
            if 30 < v then
                v = 30
            elseif -30 > v then
                v = -30
            end
            if 10 >= math.abs(v) then
                return 270 + v / 10 * 100
            else
                if 0 < v then
                    return (v - 10) / 20 * 80 + 10
                else
                    return (v + 10) / 20 * 80 - 190
                end
            end
        end
    },
}

