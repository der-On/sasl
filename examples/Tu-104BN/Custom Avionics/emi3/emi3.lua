size = { 155, 155 }

-- background image
defineProperty("background", loadImage("emi3.png", 0, 0, 155, 155))

-- foreground image
defineProperty("foreground", loadImage("emi3.png", 13, 170, 128, 64))

-- needle image
defineProperty("long_needle", loadImage("emi3.png", 175, 18, 13, 90)) 
defineProperty("short_needle", loadImage("emi3.png", 204, 18, 13, 90))

-- fuel pressure
defineProperty("fuel_p", globalPropertyf("sim/cockpit2/engine/indicators/fuel_pressure_psi[0]"))

-- oil pressure
defineProperty("oil_p", globalPropertyf("sim/cockpit2/engine/indicators/oil_pressure_psi[0]"))

-- oil temperature
defineProperty("oil_t", globalPropertyf("sim/cockpit2/engine/indicators/oil_temperature_deg_C[0]"))


-- 1 pound/square inch = 0.07031 kilogram/square centimeter

-- emi3 consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },
    
    -- fuel pressure needle
    needle {
        position = { 32.5, 47, 90 , 90 },
        image = get(long_needle),
        angle = function()
            local v = get(fuel_p)
            if v < 100 then
                return v * 120 / 100 - 60; 
            else 
                return 60   
            end
        end   
    },

 
    -- oil pressure needle
    needle {
        position = { -5, 17, 90 , 90 },
        image = get(short_needle),
        angle = function()
            local v = get(oil_p) * 0.07031 
            if v < 8 then
               return - v * 120 / 8 + 150;
            else return 30   
            end
        end   
    }, 

    -- oil temperature needle
    needle {
        position = { 70, 17, 90 , 90 },
        image = get(short_needle),
        angle = function()
            local t = get(oil_t) 
            if t < -50 then 
                 return -150
            elseif t > 150 then
                 return -30       
            else return  t * 120 / 200 - 120;
            end
        end   
    },   
 

    -- foreground image
    texture { 
        position = { 13, 43, 128, 64 },
        image = get(foreground)
    },    
         

}



