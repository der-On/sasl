size = { 149, 56 }

-- autopilot speed setting
defineProperty("autoSpeed", globalPropertyf("sim/cockpit/autopilot/airspeed"))

-- settings backgorund image
defineProperty("backgroundImage", loadImage("ap-settings-bg.png", 51, 20))

-- digits image
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

components = {
    rotary {
        position = { 74, 0, 56, 56 };
        image = loadImage("rotary.png");
        value = autoSpeed;
    };

    texture { 
        position = { 0, 15, 84, 28 };
        image = backgroundImage;
    };

    digitstape {
        position = { 0, 17, 80, 25 };
        image = digitsImage;
        digits = 5;
        
        value = function() 
            return math.floor(get(autoSpeed) * 1.852 + 0.5); 
        end;
    };
}

