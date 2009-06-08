size = { 160, 160 }

-- initialize component property table
defineProperty("adf", globalPropertyf("sim/cockpit2/radios/indicators/adf1_relative_bearing_deg"))
defineProperty("fail", globalPropertyf("sim/operation/failures/rel_adf1"))
defineProperty("mode", globalPropertyf("sim/cockpit2/radios/actuators/adf1_power"))  -- adf mode. 0 = off, 1 = antenna, 2 = on, 3 = tone, 4 = test
defineProperty("gauge_power",globalPropertyi("sim/custom/xap/gauge_power_avail"))

defineProperty("flight_time", globalPropertyf("sim/time/total_flight_time_sec"))  -- local time since aircraft was loaded 
defineProperty("M", globalPropertyf("sim/flightmodel/position/M"))  -- some momentum of aircraft. it's remein one value, when sim paused

-- background image
defineProperty("background", loadImage("ark_u2.png", 0, 0, 160, 160))

-- needle image
defineProperty("bearing_needle", loadImage("ark_u2.png", 160, 14, 32, 128))

local angle = get(adf)
local power

local time_last = get(flight_time)
local time = 0 
local lastM = get(M)

function update()
           local v = get(adf)
           local ind = 0
           power = get(gauge_power)

         time = get(flight_time) - time_last  
         if get(M) - lastM == 0 then time = 0 end

       if get(mode) > 0 and get(fail) < 6 and power == 1 then  -- check if indicator is working    
           
           if v == 90 then 
              v = math.random(0, 360) 
              ind = 1
           else
               v = v + 5 * (math.random() - 0.5)
               ind = 0
           end  
           
           -- calculate smooth move of adf needle
           local delta = v - angle
           if delta > 180 then delta = delta - 360 end
           if delta < -180 then delta = delta + 360 end
           
           if ind == 0 then
              angle = angle + 1 * delta  * time
           else 
              angle = angle + (0.2 * delta + 1 * (math.random() - 0.5)) * time
           end
           -- calculate circle if needle's move
           if angle > 180 then angle = angle - 360 end
           if angle < -180 then angle = angle + 360 end
 
       end    
       
        time_last = get(flight_time)
        lastM = get(M)    

       return true
        

end                                                              


-- radiocompass consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background),
    },

    -- bearing needle
    needle {
        position = { 16, 17, 128, 128 },
        image = get(bearing_needle),
        angle = function() 
             return angle
        end,
    },  

    
   
}

