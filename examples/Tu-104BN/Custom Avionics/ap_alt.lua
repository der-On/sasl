size = { 149, 56 }

defineProperty("altBackgroundImage", loadImage("ap-settings-bg.png", 51, 20))
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

-- autopilot altitude setting
defineProperty("auto_alt", globalPropertyf("sim/cockpit/autopilot/altitude"))

components = {
    rotary {
        image = loadImage("rotary.png");
        value = auto_alt;
        step = 328.08399 / 2.0;
        position = { 74, 0, 56, 56 };

        -- round feets to hunred meters
        adjuster = function(value)
            return math.floor((value * 0.3048) / 50 + 0.5) * 50 * 3.2808399
        end
    };

    texture { 
        position = { 0, 15, 84, 28 };
        image = altBackgroundImage;
    };

    ap_alt_display { 
        position = { 0, 17, 80, 25 };
        image = digitsImage;
    };
}

