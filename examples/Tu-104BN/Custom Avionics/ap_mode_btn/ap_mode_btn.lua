-- general autopilot two-state indicator and button

-- image used when button in "ON" state
defineProperty("lightOn", loadImage("light-on.png"))

-- image used when button in "OFF" state
defineProperty("lightOff", loadImage("light-off.png"))

-- function called to get button state
defineProperty("bit")

-- autopilot state
defineProperty("state", globalPropertyi("sim/cockpit/autopilot/autopilot_state"))

components = {

    -- "on" state texture
    textureLit {
        image = lightOn,
        visible = function() return 0 ~= bitand(get(state), get(bit)); end,
    };
    
    -- "off" state texture
    textureLit {
        image = lightOff,
        visible = function() return 0 == bitand(get(state), get(bit)); end,
    };

    -- toggle state button
    button {
        onMouseDown = function ()
            set(state, get(bit))
            return true
        end
    }

}


