size = { 220, 220 }

-- initialize component property table
defineProperty("altitude", globalPropertyf("sim/cockpit2/gauges/indicators/altitude_ft_pilot"))

-- background image
defineProperty("background", loadImage("yk12altimeter.png", size[1], size[2]))

-- meters needle image
defineProperty("longNeedleImage", loadImage("needle.tga"))

-- hundreds of meters needle image
defineProperty("shortNeedleImage", loadImage("alt-hundreds.tga"))

-- altimeter consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- hundreds meters needle
    needle {
        position = { 0, 0, size[1], size[2] },
        image = get(longNeedleImage),
        angle = function() 
            return get(altitude) * 0.3048 * 0.001 * 360.0; 
        end
    },

    -- thousands meters needle
    needle {
        position = { 30, 30, size[1]-60, size[2]-60 },
        image = get(shortNeedleImage),
        angle = function() 
            return get(altitude) * 0.3048 * 0.0001 * 360.0; 
        end
    },
    
    --frame { position = { 0, 0, size[1]-10, size[2] } },
}

