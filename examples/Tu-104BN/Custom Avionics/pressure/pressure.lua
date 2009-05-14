size = { 212, 164 }

-- digits texture
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

-- desired barometric altitude of cabin
defineProperty("cabinAlt", globalPropertyf("sim/cockpit2/pressurization/actuators/cabin_altitude_ft"))

-- altitude of aircraft
defineProperty("acfAlt", globalPropertyf("sim/cockpit2/pressurization/actuators/max_allowable_altitude_ft"))

-- handler image
defineProperty("rotaryImage", loadImage("rotary.png"))

-- background image
defineProperty("backgroundImage", loadImage("pressure-bg.png", 212, 164))

components = {

    texture { 
        position = { 0, 0, 212, 164 };
        image = backgroundImage;
    };

    -- cabin barometric altiture
    digitstape {
        position = { 80, 112, 65, 22 };
        image = digitsImage;
        digits = 5;
        
        value = function() 
            return math.floor(get(cabinAlt) * 0.3084 / 100 + 0.5) * 100; 
        end;
    };
    
    -- maximum allowed aircraft altiture
    digitstape {
        position = { 80, 75, 65, 22 };
        image = digitsImage;
        digits = 5;
        
        value = function() 
            return math.floor(get(acfAlt) * 0.3084 / 10 + 0.5) * 10; 
        end;
    };
    
    -- pressure rotary
    rotary {
        image = rotaryImage;
        value = cabinAlt;
        step = 100 / 0.3084;
        position = { 76, 14, 60, 60 };

        -- round inches hg to millimeters hg
        adjuster = function(v)
            return math.floor(v * 0.3084 / 100 + 0.5) * 100 / 0.3084
        end;
    };
}    

