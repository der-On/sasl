-- two needles tachometer

size = { 160, 160 }


-- revolutions of first engine
defineProperty("n1_1", globalPropertyf("sim/cockpit2/engine/indicators/N1_percent[0]"))

-- revolutions of second engine
defineProperty("n1_2", globalPropertyf("sim/cockpit2/engine/indicators/N1_percent[1]"))

-- background image
defineProperty("background", loadImage("tacho.png", 0, 0, 160, 160))

-- needle 1 image
defineProperty("needle1", loadImage("tacho.png", 165, 14, 22, 128))

-- needle 2 image
defineProperty("needle2", loadImage("tacho.png", 181, 14, 22, 128))

-- convert revolutions to needle angle
function revolutionsToAngle(rev)
    return 190 + rev / 100.0 * 4700.0 / 5000.0 * 340.0;
end


components = {
    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background),
    },
    
    -- first engine needle
    needle {
        position = { 16, 17, 128, 128 },
        image = get(needle1),
        angle = function() 
             return revolutionsToAngle(get(n1_1));
        end,
    },  
    
    -- second engine needle
    needle {
        position = { 16, 17, 128, 128 },
        image = get(needle2),
        angle = function() 
             return revolutionsToAngle(get(n1_2));
        end,
    },  
}

