size = { 149, 56 }

-- autopilot speed setting
defineProperty("autoHdg", globalPropertyf("sim/cockpit/autopilot/heading_mag"))

-- settings backgorund image
defineProperty("backgroundImage", loadImage("ap-settings-bg.png", 51, 20))

-- digits image
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

components = {
    rotary {
        position = { 74, 0, 56, 56 };
        image = loadImage("rotary.png");
        value = autoHdg;
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
        value = function ()
            return math.floor(get(autoHdg) + 0.5);
        end
    };
}

