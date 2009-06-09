size = { 235, 235 }

-- initialize component property table
defineProperty("v_plank", globalPropertyf("sim/cockpit2/radios/indicators/nav1_hdef_dots_pilot"))
defineProperty("h_plank", globalPropertyf("sim/cockpit2/radios/indicators/nav1_vdef_dots_pilot"))
defineProperty("obs", globalPropertyf("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot"))


-- define images
defineProperty("background", loadImage("nkp4.png", 0, 0, size[1], size[2])) 
defineProperty("v_plank_img", loadImage("nkp4.png", 309, 58, 4, 119))
defineProperty("h_plank_img", loadImage("nkp4.png", 349, 10, 119, 4)) 
defineProperty("gyro_scale", loadImage("nkp4.png", 324, 26, 181, 181))



-- NKP-4K consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- vertical plank
    free_texture {
        image = get(v_plank_img),
        position_x = function() 
             return 115.5 + get(v_plank) * 17.6
             end,
        position_y = 55,
        width = 4,
        height = 119 
    },
 
    
    -- horizontal plank
    free_texture {
        image = get(h_plank_img),
        position_x = 60,
        position_y = function() 
            return 115.5 - get(h_plank) * 17.6
        end,
        width = 119,
        height = 4 
    },  
    
    -- gyrocompass scale
   needle {
        position = { 27, 27, 181, 181 },
        image = get(gyro_scale),
        angle = function() 
            return -get(obs)
        end
   
    },  

     -- click zone for decrement OBS
     clickable {
        position = { 147, 0, 50, 50 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
       onMouseClick = function(x, y, button) 
            local x = get(obs) - 1
            set(obs, x)
            return true
       end
    },     

     -- click zone for icrement OBS
     clickable {
        position = { 197, 0, 50, 50 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
       onMouseClick = function(x, y, button) 
            local x = get(obs) + 1
            set(obs, x)
            return true
       end
    },   
    


}

