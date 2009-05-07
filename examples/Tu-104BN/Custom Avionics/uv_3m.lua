-- UV-3M radio altimeter

size = { 256, 256 }


-- background image
defineProperty("background", loadImage("uv-3m.png", size[1], size[2]))

-- radio altitude
defineProperty("altitude", globalPropertyf("sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_pilot"))

-- needle image
defineProperty("needleImage", loadImage("uv-3m-needle.png"))

components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },
    
    -- meters needle
    needle {
        position = { 40, 40, size[1]-80, size[2]-80 },
        image = needleImage,
        angle = function() 
            local a = get(altitude) * 0.3048
            if 0 > a then
                return 0
            elseif 30 > a then
                return a / 30 * 55
            elseif 50 > a then
                return (a - 30) / 20 * 35 + 55
            elseif 100 > a then
                return (a - 50) / 50 * 40 + 90
            elseif 600 > a then
                return (a - 100) / 500 * 180 + 130
            else
                return 310
            end
        end
    },
}

