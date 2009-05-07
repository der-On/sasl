size = { 40, 10 }


-- autopilot altitude setting
defineProperty("auto_alt", globalPropertyf("sim/cockpit/autopilot/altitude"))

-- digits image
defineProperty("image", loadImage("digits-tape.png"))

-- overlay image
--defineProperty("overlayImage", loadImage("digits-tape-bg.png", 8, 10))
defineProperty("overlayImage")

-- background image
--defineProperty("backgroundImage", loadImage("ap-settings-bg.png", 51, 20))
defineProperty("backgroundImage")


components = {
    
    -- background image
    texture { 
        position = { 0, 0, 40, 10 };
        image = backgroundImage; 
    },

    digitstape {
        position = { 0, 0, 40, 10 };
        image = image;
        overlayImage = overlayImage;
        digits = 5;
        
        value = function() 
            return math.floor(get(auto_alt) * 0.3048 / 10 + 0.5) * 10; 
        end;
    };
}

