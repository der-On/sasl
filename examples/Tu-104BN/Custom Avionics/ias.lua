size = { 220, 220 }

-- background image
defineProperty("background", loadImage("yk12ias.png", size[1], size[2]))

defineProperty("needleImage", loadImage("needle.tga"))

-- speed variable
defineProperty("speed", globalPropertyf("sim/flightmodel/position/indicated_airspeed"))


-- 50 km/h at 10 degrees
-- 350 km/h at 340 degrees

-- altimeter consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },
    
    -- speed needle
    needle {
        position = { 0, 0, size[1], size[2] },
        image = get(needleImage),
        angle = function()
            local v = get(speed) * 1.852
            if 350 < v then
                v = 350
            end
            if 25 > v then
                return 0
            elseif 50 >= v then
                return (v - 25) / 25 * 10.0
            else
                return 10.0 + (v - 50.0) / 300.0 * 330.0;
            end
        end
    },

}


