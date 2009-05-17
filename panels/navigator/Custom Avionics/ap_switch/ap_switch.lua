-- turns autopilot on or off

size = { 32, 64 }

-- autopilot mode
defineProperty("mode", globalPropertyi("sim/cockpit/autopilot/autopilot_mode"))

-- switch up
defineProperty("tmbUp", loadImage("tumbler_up.png"))

-- switch down
defineProperty("tmbDown", loadImage("tumbler_down.png"))

-- background
defineProperty("background", loadImage("ap-switch-bg.png"))

-- switch subcomponents
components = {

    -- background image
    texture {
        position = { 0, 0, size[1], size[2] };
        image = get(background);
    };

    -- autopilot on/off switch
    switch {
        position = { 4, 2, 24, 60 };

        -- button is on if autopilot enabled
        state = function () return 2 == get(mode); end;

        -- turn autopilot on or off
        onMouseDown = function ()
            if 2 == get(mode) then
                set(mode, 0)
            else
                set(mode, 2)
            end
            return true
        end;

        btnOn = get(tmbUp);
        btnOff = get(tmbDown);
    };

};

