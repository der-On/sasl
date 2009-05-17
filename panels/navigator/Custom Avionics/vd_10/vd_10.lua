size = { 256, 256 }

-- initialize component property table
defineProperty("altitude", globalPropertyf("sim/cockpit2/gauges/indicators/altitude_ft_pilot"))

-- background image
defineProperty("background", loadImage("vd-10-bg.png", size[1], size[2]))

-- meters needle image
defineProperty("longNeedleImage", loadImage("needle.tga"))

-- hundreds of meters needle image
defineProperty("shortNeedleImage", loadImage("alt-hundreds.tga"))

-- thouthands of meters needle image
defineProperty("topNeedleImage", loadImage("needle-top.png"))

-- handler image
defineProperty("rotaryImage", loadImage("rotary.png"))

-- pressure value
defineProperty("pressure", globalPropertyf("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot"))

-- digits images
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))


-- altimeter consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- hundreds meters needle
    needle {
        position = { 50, 50, size[1]-100, size[2]-100 },
        image = get(longNeedleImage),
        angle = function() 
            return get(altitude) * 0.3048 * 0.001 * 360.0; 
        end
    },

    -- thousands meters needle
    needle {
        position = { 70, 70, size[1]-140, size[2]-140 },
        image = get(shortNeedleImage),
        angle = function() 
            return get(altitude) * 0.3048 * 0.0001 * 360.0; 
        end
    },
    
    -- real thousands meters needle
    needle {
        position = { 30, 30, size[1]-60, size[2]-60 },
        image = get(topNeedleImage),
        angle = function() 
            return get(altitude) * 0.3048 * 0.00001 * 360.0; 
        end
    },
    
    -- pressure rotary
    rotary {
        image = rotaryImage;
        value = pressure;
        step = 1 / 25.3970886;
        position = { 107, -6, 44, 44 };

        -- round inches hg to millimeters hg
        adjuster = function(v)
            return math.floor((v * 25.3970886) + 0.5) / 25.3970886
        end;
    };
    
    -- pressure in millimeters of mercury
    digitstape {
        position = { 178, 121, 30, 14 };
        image = digitsImage;
        digits = 3;
        
        value = function() 
            return get(pressure) * 25.3970886; 
        end;
    };
}

