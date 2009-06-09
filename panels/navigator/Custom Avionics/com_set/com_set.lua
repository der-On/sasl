size = {220, 140}

-- define property table
defineProperty("frequency", globalPropertyf("sim/cockpit2/radios/actuators/com1_frequency_hz"))  -- set the frequency


-- images table
defineProperty("background", loadImage("com_set.png", 21, 13, 220, 140))
defineProperty("big_knob", loadImage("com_set.png", 38, 206, 43, 43)) 
defineProperty("small_knob", loadImage("com_set.png", 160, 206, 43, 43)) 
defineProperty("glass_cap", loadImage("com_set.png", 17, 161, 78, 32))
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196)) 


-- variables for separate manipulations
local freq_100  -- digits before period
local freq_10   -- digits after period

local freq_10_show


function update()
   local freq = get(frequency)
   
   -- calculate separate digits
   freq_100 = math.floor(freq / 100)  -- cut off last two digits
   freq_10 = freq - freq_100 * 100  -- cut off first digits 
   
   local freq_last = freq_10 - math.floor(freq_10 / 10) * 10 
   
   if freq_last == 2 or freq_last == 7 
      then freq_last = 5
      else freq_last = 0
   end

   freq_10_show = freq_10 * 10 + freq_last
   
 
   

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
        position = { 65, 100, 30, 20},
        image = digitsImage,
        digits = 3,
        showLeadingZeros = false,
        value = function()
           return freq_100 
        end
    }; 
    
    -- decimals digits
    digitstape {
        position = {105, 100, 30, 20},
        image = digitsImage,
        digits = 3,
        showLeadingZeros = true,
        value = function()
           return freq_10_show 
        end
    }; 
    
    -- glass cap image
    texture { 
        position = { 61, 93, 78, 32 },
        image = get(glass_cap)
    },
 

    -- left knob
    needle {
        position = { 17, 49, 43, 43 },
        image = get(small_knob),    
        angle = function()
           return (freq_100 - 118) * 40
        end,
    
    },
 
    -- right knob
    needle {
        position = { 139, 49, 43, 43 },
        image = get(small_knob),    
        angle = function()
           return freq_10 * 36 / 5
        end,
    
    },


    ---------------------
    -- click zones --
    ---------------------
    
    -- click zones for left knob
    clickable {
       position = { 7, 45, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_100 = freq_100 - 1
            if freq_100 == 117 then freq_100 = 136 end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },
    
    clickable {
       position = { 37, 45, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_100 = freq_100 + 1
            if freq_100 == 137 then freq_100 = 118 end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },    

    
    -- click zones for right knob
    clickable {
       position = { 135, 45, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_10 = math.floor((freq_10_show - 25) / 10)
            
            if freq_10 < 0 then freq_10 = 97 end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },
    
    clickable {
       position = { 165, 45, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            -- calculate new frequency
            freq_10 = math.floor((freq_10_show + 25) / 10)

            if freq_10 > 97 then freq_10 = 0 end
            
            local fr = freq_100 * 100 + freq_10
            set(frequency, fr)
            
            return true
        end
    },  

}

