-- autopilot altitude setting
defineProperty("auto_alt", globalPropertyf("sim/cockpit/autopilot/altitude"))

-- handler image
defineProperty("image", loadImage("rotary.png"))

components = {

    rotary {
        image = image;
        value = auto_alt;
        step = 328.08399 / 2.0;
        position = { 0, 0, 100, 100 };

        -- round feets to hunred meters
        adjuster = function(value)
            return math.floor((value * 0.3048) / 50 + 0.5) * 50 * 3.2808399
        end
    };
}

