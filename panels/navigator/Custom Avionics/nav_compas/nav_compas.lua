size = {360, 360}

-- define property table

-- initialize component property table

defineProperty("gyro", globalPropertyf("sim/cockpit2/gauges/indicators/heading_electric_deg_mag_pilot"))
defineProperty("fail", globalPropertyf("sim/operation/failures/rel_ss_dgy"))
defineProperty("gauge_power", globalPropertyi("sim/custom/xap/gauge_power_avail"))

-- background image
defineProperty("background", loadImage("un1.png", 794, 320, 180, 180))

-- needle image
defineProperty("scale", loadImage("un1.png", 667, 0.5, 284, 284))




local power
local angle = get(gyro)

local notLoaded = true

function update()
  
           power = get(gauge_power)
           local v = get(gyro)
         
         if get(fail) < 6 and power == 1 then  -- check if indicator is working  
           
           -- calculate smooth move of adf needle
           local delta = v - angle
           if delta > 180 then delta = delta - 360 end
           if delta < -180 then delta = delta + 360 end
           
           angle = angle + 0.5 * delta 

           -- calculate circle of needle's move
           if angle > 180 then angle = angle - 360 end
           if angle < -180 then angle = angle + 360 end
    
        end   
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
        position = { 30, 30, 300, 300 },
        image = get(scale),
        angle = function() 
             return -angle
        end,
    },    
    

}
