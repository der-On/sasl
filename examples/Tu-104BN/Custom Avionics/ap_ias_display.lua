size = { 24, 10 }

defineProperty("auto_ias", globalPropertyf("sim/cockpit/autopilot/airspeed"))

components = {

    digitstape {
        position = { 0, 0, 24, 10 };
        image = loadImage("digits-tape.png");
        overlayImage = loadImage("digits-tape-bg.png", 8, 10);
        value = function()
            return get(auto_ias) * 1.852
        end;
        digits = 3;
    };

}

