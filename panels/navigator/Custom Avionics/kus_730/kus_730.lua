size = { 256, 256 }

-- background image
defineProperty("background", loadImage("kus730.png", size[1], size[2]))

-- needle image
defineProperty("needleImage", loadImage("needle.tga"))

-- ias variable
defineProperty("speed", globalPropertyf("sim/cockpit2/gauges/indicators/airspeed_kts_pilot"))

-- tas variable
defineProperty("tas", globalPropertyf("sim/flightmodel/position/true_airspeed"))


-- 50 km/h at 10 degrees
-- 350 km/h at 340 degrees

-- altimeter consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },
    
    -- ias needle
    needle {
        position = { 40, 40, size[1]-80, size[2]-80 },
        image = get(needleImage),
        angle = function()
            local v = get(speed) * 1.852
            if 750 < v then
                v = 750
            end
            if 50 > v then
                return 0
            else
                return (v - 50.0) / 700 * 335;
            end
        end
    },

    -- tas needle
    needle {
        position = { 80, 80, size[1]-160, size[2]-160 },
        image = get(needleImage),
        angle = function()
            local v = get(tas) * 3.6
            if 1100 < v then
                v = 1100
            end
            if 350 > v then
                return 180
            elseif 400 > v then
                return (v - 350) / 50 * 10 + 180
            else
                return 190 + (v - 400) / 700 * 340;
            end
        end
    },
}



