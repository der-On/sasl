size = {360, 360}

-- define property table

-- initialize component property table

defineProperty("gauge_power", globalPropertyi("sim/custom/xap/gauge_power_avail"))    -- local logic gauge power
defineProperty("adf1", globalPropertyf("sim/cockpit2/radios/indicators/adf1_relative_bearing_deg"))    -- bearing of ADF1
defineProperty("adf2", globalPropertyf("sim/cockpit2/radios/indicators/adf2_relative_bearing_deg"))    -- bearing of ADF2
defineProperty("fail_1", globalPropertyf("sim/operation/failures/rel_adf1"))
defineProperty("fail_2", globalPropertyf("sim/operation/failures/rel_adf2"))
defineProperty("adf1_mode", globalPropertyf("sim/cockpit2/radios/actuators/adf1_power")) 
defineProperty("adf2_mode", globalPropertyf("sim/cockpit2/radios/actuators/adf2_power")) 

defineProperty("flight_time", globalPropertyf("sim/time/total_flight_time_sec"))  -- local time since aircraft was loaded 
defineProperty("M", globalPropertyf("sim/flightmodel/position/M"))  -- some momentum of aircraft. it's remein one value, when sim paused

-- background image
defineProperty("background", loadImage("un1.png", 0, 0, 359, 359))

-- needle image
defineProperty("scale", loadImage("un1.png", 372.5, 0.5, 284, 284))
defineProperty("adf1_needle", loadImage("un1.png", 365, 380, 295, 57))
defineProperty("adf2_needle", loadImage("un1.png", 365, 442, 295, 57))
defineProperty("curs_handle", loadImage("un1.png", 150, 423, 60, 60))


local power = 1
local curs = 0
local adf1_angle = get(adf1)
local adf2_angle = get(adf2)

local time_last = get(flight_time)
local time = 0 
local lastM = get(M)

function update()

         time = get(flight_time) - time_last  
         if get(M) - lastM == 0 then time = 0 end

         
         -- adf 1 logic

           local v1 = get(adf1)
           local ind1 = 0

        if get(adf1_mode) > 0 and get(fail_1) < 6 and power == 1 then  -- check if indicator is working  
           if v1 == 90 then 
              v1 = math.random(0, 360) 
              ind1 = 1
           else
               v1 = v1 + 5 * (math.random() - 0.5)
               ind1 = 0
           end  
           
           -- calculate smooth move of adf needle
           local delta1 = v1 - adf1_angle
           if delta1 > 180 then delta1 = delta1 - 360 end
           if delta1 < -180 then delta1 = delta1 + 360 end
           
           if ind1 == 0 then
              adf1_angle = adf1_angle + 1 * delta1 * time
           else 
              adf1_angle = adf1_angle + (0.2 * delta1 + 1 * (math.random() - 0.5)) * time
           end
           -- calculate circle if needle's move
           if adf1_angle > 180 then adf1_angle = adf1_angle - 360 end
           if adf1_angle < -180 then adf1_angle = adf1_angle + 360 end         
        end 

         
         -- adf 2 logic

           local v2 = get(adf2)
           local ind2 = 0

        if get(adf2_mode) > 0 and get(fail_2) < 6 and power == 1 then  -- check if indicator is working  
           if v2 == 90 then 
              v2 = math.random(0, 360) 
              ind2 = 1
           else
               v2 = v2 + 5 * (math.random() - 0.5)
               ind2 = 0
           end  
           
           -- calculate smooth move of adf needle
           local delta2 = v2 - adf2_angle
           if delta2 > 180 then delta2 = delta2 - 360 end
           if delta2 < -180 then delta2 = delta2 + 360 end
           
           if ind2 == 0 then
              adf2_angle = adf2_angle + 1 * delta2 * time 
           else 
              adf2_angle = adf2_angle + (0.2 * delta2 + 1 * (math.random() - 0.5)) * time
           end
           -- calculate circle if needle's move
           if adf2_angle > 180 then adf2_angle = adf2_angle - 360 end
           if adf2_angle < -180 then adf2_angle = adf2_angle + 360 end           
        end 
        
        time_last = get(flight_time)
        lastM = get(M)

        return true
end                                                              


-- radiocompass consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, 360, 360 },
        image = get(background),
    },

    -- round scale
    needle {
        position = { 39, 39, 284, 284 },
        image = get(scale),
        angle = function() 
             return curs
        end,
    },    
 
    -- adf1 needle
    needle {
        position = { 34, 32.5, 295, 295 },
        image = get(adf1_needle),
        angle = function() 
             return adf1_angle + 90
        end,
    }, 

    -- adf2 needle
    needle {
        position = { 34, 32.5, 295, 295 },
        image = get(adf2_needle),
        angle = function() 
             return adf2_angle + 90
        end,
    },    
    
    -- curs handle
    needle {
        position = { 290, 10, 60, 60 },
        image = get(curs_handle),
        angle = function() 
             return curs * 5
        end,
    },
    
    -- click zone for decrement curs
     clickable {
        position = { 290, 10, 30, 60 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
       onMouseClick = function(x, y, button) 
            curs = curs - 1
            if curs < -180 then curs = curs + 360 end
            return true
       end
    },

    -- click zone for increment curs
     clickable {
        position = { 320, 10, 30, 60 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
       onMouseClick = function(x, y, button) 
            curs = curs + 1
            if curs > 180 then curs = curs - 360 end
            return true
       end
    },

}
