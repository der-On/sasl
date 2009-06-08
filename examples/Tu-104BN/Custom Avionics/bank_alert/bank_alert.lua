size = { 160, 25 }

-- aircraft roll
defineProperty("roll", globalPropertyf("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot"))

-- bank angle to alert
defineProperty("maxBank", 45);

-- background image
defineProperty("background", loadImage("bank_alert.png", 4, 2, 160, 25))

-- lamp on image
defineProperty("lightOn", loadImage("bank_alert.png", 169, 4, 21, 21))


-- bank alert consists of background and lamps
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = background,
    },
    
    -- left bank angle too great
    textureLit {
        image = lightOn,
        position = { 2, 2, 21, 21 },
        visible = function() return -get(maxBank) > get(roll); end,
    };
    
    -- left bank angle too great
    textureLit {
        image = lightOn,
        position = { 137, 2, 21, 21 },
        visible = function() return get(maxBank) < get(roll); end,
    };
    
}

