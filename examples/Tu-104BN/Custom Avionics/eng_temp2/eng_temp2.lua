-- two needles engine temperature indicator

size = { 160, 160 }


-- revolutions of first engine
defineProperty("t1", globalPropertyf("sim/cockpit2/engine/indicators/EGT_deg_C[0]"))

-- revolutions of second engine
defineProperty("t2", globalPropertyf("sim/cockpit2/engine/indicators/EGT_deg_C[1]"))

-- background image
defineProperty("background", loadImage("eng_temp2.png", 0, 0, 160, 160))

-- needle 1 image
defineProperty("needleImage", loadImage("eng_temp2.png", 165.5, 14, 22, 128))


components = {
    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background),
    },
    
    -- first engine needle
    needle {
        position = { -24, 17, 128, 128 },
        image = get(needleImage),
        angle = function() 
            local temp = get(t1)
            if 250 > temp then
                return 205
            elseif 300 > temp then
                return 205 - (300 - temp) / 50.0 * 5.0
            elseif 900 > temp then
                return 200 - (temp - 300) / 600 * 220;
            else
                return -20
            end
        end,
    },  
    
    -- second engine needle
    needle {
        position = { 54, 17, 128, 128 },
        image = get(needleImage),
        angle = function() 
            local temp = get(t2)
            if 250 > temp then
                return 155
            elseif 300 > temp then
                return 155 + (300 - temp) / 50.0 * 5.0
            elseif 900 > temp then
                return 160 + (temp - 300) / 600 * 220;
            else
                return 20
            end
        end,
    },  
}


