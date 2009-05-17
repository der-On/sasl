size = { 210, 210 }

-- initialize component property table
defineProperty("utc_time", globalPropertyf("sim/time/zulu_time_sec"))
defineProperty("sim_run_time", globalPropertyf("sim/time/total_running_time_sec"))
defineProperty("world_green", globalPropertyf("sim/graphics/misc/cockpit_light_level_g"))  -- green component of natural light in cockpit



-- background image
defineProperty("background", loadImage("achs1.png", 0, 0, size[1], size[2]))
defineProperty("background_lit", loadImage("achs1_lit.png", 0, 0, size[1], size[2]))


-- needle images
defineProperty("time_hour_nd", loadImage("achs1.png", 241, 10, 14, 160))
defineProperty("time_min_nd", loadImage("achs1.png", 226, 10, 14, 160))
defineProperty("time_sec_nd", loadImage("achs1.png", 216, 10, 14, 160))
defineProperty("flight_hour_nd", loadImage("achs1.png", 25, 210, 7, 72))
defineProperty("flight_min_nd", loadImage("achs1.png", 38, 210, 7, 72))
defineProperty("chrono_sec_nd", loadImage("achs1.png", 50, 210, 8, 72))
defineProperty("chrono_min_nd", loadImage("achs1.png", 62, 210, 8, 72))

defineProperty("time_hour_nd_lit", loadImage("achs1_lit.png", 241, 10, 14, 160))
defineProperty("time_min_nd_lit", loadImage("achs1_lit.png", 226, 10, 14, 160))
defineProperty("time_sec_nd_lit", loadImage("achs1_lit.png", 216, 10, 14, 160))
defineProperty("flight_hour_nd_lit", loadImage("achs1_lit.png", 25, 210, 7, 72))
defineProperty("flight_min_nd_lit", loadImage("achs1_lit.png", 38, 210, 7, 72))
defineProperty("chrono_sec_nd_lit", loadImage("achs1_lit.png", 50, 210, 8, 72))
defineProperty("chrono_min_nd_lit", loadImage("achs1_lit.png", 62, 210, 8, 72))


-- flag images
defineProperty("flag_0", loadImage("achs1.png", 7, 221, 8, 6))  -- white
defineProperty("flag_1", loadImage("achs1.png", 7, 238, 8, 6))  -- red
defineProperty("flag_2", loadImage("achs1.png", 7, 230, 8, 6))  -- red/white



-- define needed variables
local sec_mode = 0  -- mode of sec-timer
local flight_mode = 0  -- mode of chronometer of flight time
local sec_time = 0  -- time of chronometer of sec
local flight_time = 0  -- time of flight
local start_sec  -- time of sec chrono start
local start_flight  -- time of flight chrono start

local night = 0

function update()
    -- local a = get(world_green)
    -- print(a)
    
    if get(world_green) > 0.75 then night = 0
       else night = 1
    end 
    

end



-- clock consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },
-----------------------------------
    -- chronometer --
-----------------------------------
    -- minutes needle
    needle {
        position = { 67, 26, 72, 72 },
        image = get(chrono_min_nd),
        angle = function()
            if sec_mode == 0 then 
               sec_time = 0
               return sec_time 
            elseif sec_mode == 1 then 
               sec_time = get(sim_run_time) - start_sec
               return sec_time * 360 / (60 * 60)
            else
               return sec_time * 360 / (60 * 60)
            end
        end
    },
    
    -- secondes needle
    needle {
        position = { 67, 26, 72, 72 },
        image = get(chrono_sec_nd),
        angle = function()
            if sec_mode == 0 then 
               sec_time = 0
               return sec_time 
            elseif sec_mode == 1 then 
               sec_time = get(sim_run_time) - start_sec
               return sec_time * 360 / 60
            else
               return sec_time * 360 / 60
            end
        end
    },  
    
 
    
    clickable {
        position = {165, 17, 30, 30 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("clickable.png")
        },  
        
       onMouseDown = function(x, y, button) 
            if sec_mode == 0 then
               sec_mode = 1
               start_sec = get(sim_run_time)
            elseif sec_mode == 1 then
               sec_mode = 2
            else sec_mode = 0
            end
            return true
       end 
    }, 
-----------------------------------
    -- flight timer --
-----------------------------------
    -- flight mode flag
    texture { 
        position = { 99, 136, 8, 6 },
        image = function()
            if flight_mode == 1 then return get(flag_1)
            elseif flight_mode == 2 then return get(flag_2)
            else return get(flag_0)
            end
        end
    },
    
      
    -- hours needle
    needle {
        position = { 67, 118, 72, 72 },
        image = get(flight_hour_nd),
        angle = function()
            if flight_mode == 0 then 
               flight_time = 0
               return flight_time 
            elseif flight_mode == 1 then 
               flight_time = get(sim_run_time) - start_flight
               return flight_time * 360 / (60 * 60 * 12)
            else
               return flight_time * 360 / (60 * 60 * 12)
            end
        end
    },
    
    -- minutes needle
    needle {
        position = { 67, 118, 72, 72 },
        image = get(flight_min_nd),
        angle = function()
            if flight_mode == 0 then 
               flight_time = 0
               return flight_time 
            elseif flight_mode == 1 then 
               flight_time = get(sim_run_time) - start_flight
               return flight_time * 360 / (60 * 60)
            else
               return flight_time * 360 / (60 * 60)
            end
        end
    },  
    

    
    clickable {
        position = {17, 20, 30, 30 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("clickable.png")
        },  
        
       onMouseDown = function(x, y, button) 
            if flight_mode == 0 then
               flight_mode = 1
               start_flight = get(sim_run_time)
            elseif flight_mode == 1 then
               flight_mode = 2
            else flight_mode = 0
            end
            return true
       end 
    },   
    
-----------------------------------    
    -- clock -- 
-----------------------------------
    -- hours needle
    needle {
        position = { 22.5, 28, 160, 160 },
        image = get(time_hour_nd),
        angle = function() 
            return get(utc_time) * 360 / (60 * 60 * 12)
        end
    },  

    -- minutes needle
    needle {
        position = { 22.5, 28, 160, 160 },
        image = get(time_min_nd),
        angle = function() 
            return get(utc_time) * 360 / (60 * 60)
        end
    },  
    
    -- secondes needle
    needle {
        position = { 22.5, 28, 160, 160 },
        image = get(time_sec_nd),
        angle = function() 
            return math.floor(get(utc_time)) * 360 / 60
        end
    },  


------------------------------------
   -- lit textures --
------------------------------------ 
    
    textureLit { 
        position = { 0, 0, size[1], size[2] },
        image = get(background_lit),
        visible = function()
           return night == 1
        end
    },    

    -- minutes needle
    needleLit {
        position = { 67, 26, 72, 72 },
        image = get(chrono_min_nd_lit),
        angle = function()
            if sec_mode == 0 then 
               sec_time = 0
               return sec_time 
            elseif sec_mode == 1 then 
               sec_time = get(sim_run_time) - start_sec
               return sec_time * 360 / (60 * 60)
            else
               return sec_time * 360 / (60 * 60)
            end
        end,
        visible = function()
           return night == 1
        end
    },
    
    -- secondes needle
    needleLit {
        position = { 67, 26, 72, 72 },
        image = get(chrono_sec_nd_lit),
        angle = function()
            if sec_mode == 0 then 
               sec_time = 0
               return sec_time 
            elseif sec_mode == 1 then 
               sec_time = get(sim_run_time) - start_sec
               return sec_time * 360 / 60
            else
               return sec_time * 360 / 60
            end
        end,
        visible = function()
           return night == 1
        end
    },     
    
    -- hours needle
    needleLit {
        position = { 67, 118, 72, 72 },
        image = get(flight_hour_nd_lit),
        angle = function()
            if flight_mode == 0 then 
               flight_time = 0
               return flight_time 
            elseif flight_mode == 1 then 
               flight_time = get(sim_run_time) - start_flight
               return flight_time * 360 / (60 * 60 * 12)
            else
               return flight_time * 360 / (60 * 60 * 12)
            end
        end,
        visible = function()
           return night == 1
        end
    },
    
    -- minutes needle
    needleLit {
        position = { 67, 118, 72, 72 },
        image = get(flight_min_nd_lit),
        angle = function()
            if flight_mode == 0 then 
               flight_time = 0
               return flight_time 
            elseif flight_mode == 1 then 
               flight_time = get(sim_run_time) - start_flight
               return flight_time * 360 / (60 * 60)
            else
               return flight_time * 360 / (60 * 60)
            end
        end,
        visible = function()
           return night == 1
        end
    }, 


    -- hours needle
    needleLit {
        position = { 22.5, 28, 160, 160 },
        image = get(time_hour_nd_lit),
        angle = function() 
            return get(utc_time) * 360 / (60 * 60 * 12)
        end,
        visible = function()
           return night == 1
        end
    },  

    -- minutes needle
    needleLit {
        position = { 22.5, 28, 160, 160 },
        image = get(time_min_nd_lit),
        angle = function() 
            return get(utc_time) * 360 / (60 * 60)
        end,
        visible = function()
           return night == 1
        end
    },  
    
    -- secondes needle
    needleLit {
        position = { 22.5, 28, 160, 160 },
        image = get(time_sec_nd_lit),
        angle = function() 
            return math.floor(get(utc_time)) * 360 / 60
        end,
        visible = function()
           return night == 1
        end
    }, 


}

