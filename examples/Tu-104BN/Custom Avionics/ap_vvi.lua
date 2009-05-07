size = { 149, 56 }

-- autopilot speed setting
defineProperty("autoVvi", globalPropertyf("sim/cockpit/autopilot/vertical_velocity"))

-- settings backgorund image
defineProperty("backgroundImage", loadImage("ap-settings-bg.png", 51, 20))

-- digits image
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

components = {
    rotary {
        position = { 74, 0, 56, 56 };
        image = loadImage("rotary.png");
        value = autoVvi;
        step = 196.850394;
        
        -- round feets per minute to meters per second
        adjuster = function(value)
            return math.floor((value * 0.00508) + 0.5) * 196.850394
        end
    };

    texture { 
        position = { 0, 15, 84, 28 };
        image = backgroundImage;
    };

    digitstape {
        position = { 32, 17, 48, 25 };
        image = digitsImage;
        digits = 3;
        showLeadingZeros = true;
        showSign = true;
        value = function ()
            return math.floor(get(autoVvi) * 0.00508 + 0.5);
        end
    };
}

