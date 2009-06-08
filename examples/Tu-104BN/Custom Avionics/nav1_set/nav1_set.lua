size = {220, 140}

-- define property table
defineProperty("frequency", globalPropertyf("sim/cockpit2/radios/actuators/nav1_frequency_hz"))  -- set the frequency
defineProperty("ilsOnly", false)  -- true to work with ILS only


-- images table
defineProperty("background", loadImage("nav1_set.png", 21, 13, 220, 140))
defineProperty("big_knob", loadImage("nav1_set.png", 38, 206, 43, 43)) 
defineProperty("small_knob", loadImage("nav1_set.png", 160, 206, 43, 43)) 
defineProperty("glass_cap", loadImage("nav1_set.png", 21, 160, 70, 34))
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196)) 


-- variables for separate manipulations
local freq_100  
local freq_10 



function update()
   local freq = get(frequency)
   
   -- calculate separate digits
   freq_100 = math.floor(freq / 100)  -- cut off last two digits
   freq_10 = freq - freq_100 * 100  -- cut off first digits

end


-- device consist of several components

components = {

    
    -----------------
    -- images --
    -----------------

    -- background image
    texture { 
        position = { 0, 0, 220, 140 },
        image = get(background)
    },  
 

    -- hundreds digits
    digitstape {
        position = { 68, 90, 30, 20},
        image = digitsImage,
        digits = 3,
        showLeadingZeros = false,
        value = function()
           return freq_100 
        end
    }; 
    
    -- decimals digits
    digitstape {
        position = {109, 90, 20, 20},
        image = digitsImage,
        digits = 2,
        showLeadingZeros = true,
        value = function()
           return freq_10 
        end
    }; 
    
    -- glass cap image
    texture { 
        position = { 65, 83, 70, 34 },
        image = get(glass_cap)
    },
 

    -- left knob
    needle {
        position = { 17, 35, 43, 43 },
        image = get(big_knob),    
        angle = function()
           return (freq_100 - 108) * 40
        end,
    
    },
 
    -- right knob
    needle {
        position = { 140, 35, 43, 43 },
        image = get(big_knob),    
        angle = function()
           return freq_10 * 36 / 5
        end,
    
    },

    -- right small knob
    texture { 
        position = { 140, 35, 43, 43 },
        image = get(small_knob)
    }, 

    ---------------------
    -- click zones --
    ---------------------
    
    -- click zones for left knob
    clickable {
       position = { 7, 35, 30, 40 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_100 = freq_100 - 1
            if get(ilsOnly) then
                if 108 > freq_100 then 
                    freq_100 = 111
                end
            else
                if 108 > freq_100 then 
                    freq_100 = 117 
                end
            end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },
    
    clickable {
       position = { 37, 35, 30, 40 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_100 = freq_100 + 1
            if get(ilsOnly) then
                if 111 < freq_100 then 
                    freq_100 = 108
                end
            else
                if 117 < freq_100 then 
                    freq_100 = 108 
                end
            end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },    

    
    -- click zones for right knob
    clickable {
       position = { 135, 35, 30, 40 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_10 = freq_10 - 5
            if get(ilsOnly) then
                local v = math.modf(freq_10 / 10)
                if 1 ~= v % 2 then
                    freq_10 = freq_10 - 10
                end
                if 0 >= freq_10 then 
                    freq_10 = 95 
                end
            else
                if 0 > freq_10 then 
                    freq_10 = 95 
                end
            end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },
    
    clickable {
       position = { 165, 35, 30, 40 },
        
       cursor = { 
            x = 0, 
            y = 0, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_10 = freq_10 + 5
            if get(ilsOnly) then
                local v = math.modf(freq_10 / 10)
                if 1 ~= v % 2 then
                    freq_10 = freq_10 + 10
                end
                if 95 < freq_10 then 
                    freq_10 = 10
                end
            else
                if 95 < freq_10 then 
                    freq_10 = 0
                end
            end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },  

}
