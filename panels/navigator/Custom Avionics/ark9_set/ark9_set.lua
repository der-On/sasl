size = {388, 284}

-- define component property table. 
defineProperty("left_freq", globalPropertyf("sim/cockpit2/radios/actuators/adf1_frequency_hz"))  -- left frequency
defineProperty("right_freq", globalPropertyf("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz"))  -- right frequency
defineProperty("active", globalPropertyf("sim/cockpit2/radios/actuators/adf1_right_is_selected"))  -- selector of active disk. 0 - left, 1 - right 
defineProperty("mode", globalPropertyf("sim/cockpit2/radios/actuators/adf1_power"))  -- adf mode. 0 = off, 1 = antenna, 2 = on, 3 = tone, 4 = test
defineProperty("fail", globalPropertyf("sim/operation/failures/rel_adf1"))
defineProperty("adf", globalPropertyf("sim/cockpit2/radios/indicators/adf1_relative_bearing_deg"))
defineProperty("gauge_power", 1)
defineProperty("world_green", globalPropertyf("sim/graphics/misc/cockpit_light_level_g"))  -- green component of natural light in cockpit



-- define image table
defineProperty("background", loadImage("ark9_set.png", 0, 0, 388, 284))
defineProperty("big_disk", loadImage("ark9_set.png", 406.5, 5, 100, 100))
defineProperty("small_disk", loadImage("ark9_set.png", 428, 112, 56, 56))
defineProperty("ones_disk", loadImage("ark9_set.png", 436, 191, 40, 40))
defineProperty("disk_handle", loadImage("ark9_set.png", 444, 234, 22, 68))
defineProperty("niddle", loadImage("ark9_set.png", 411, 214, 1, 58))
defineProperty("caps", loadImage("ark9_set.png", 0, 297, 296, 88))
defineProperty("mode_selector", loadImage("ark9_set.png", 434, 320, 43, 76))
defineProperty("signal_nd", loadImage("ark9_set.png", 410, 239, 2, 57)) 
defineProperty("led", loadImage("ark9_set.png", 394, 352, 12, 12))

defineProperty("tmb_left", loadImage("tumbler_left.png"))
defineProperty("tmb_right", loadImage("tumbler_right.png"))
defineProperty("background_lit", loadImage("ark9_set_lit.png", 0, 0, 388, 284))
defineProperty("background_digits", loadImage("ark9_set_digits.png", 0, 0, 388, 284))


-- define initial position for disks
set(active, 1) -- by default active disk is right
set(mode, 0) -- OFF by default



-- define local variables

local ones_left = 0
local decades_left = 0
local hundreds_left = 0
local ones_right  = 0
local decades_right = 0
local hundreds_right = 0

local freq_R 
local freq_L 
local act
local mode_sel
local signal
local power

local night = 0

function update()
    act = get(active)
    power = get(gauge_power)

    -- set limits for frequency that gauge can set (0100 - 1299)
    if get(left_freq) > 1299 then set(left_freq, 1299) end
    if get(left_freq) < 100 then set(left_freq, 100) end
    if get(right_freq) > 1299 then set(right_freq, 1299) end
    if get(right_freq) < 100 then set(right_freq, 100) end 
    
    -- set disks to their freq
    if get(active) == 0 then
       freq_R = get(right_freq)
       freq_L = get(left_freq)
    else
       freq_L = get(right_freq)
       freq_R = get(left_freq)       
    end
    
   -- calculate disks position
   ones_left = freq_L - math.floor(freq_L / 10) * 10
   decades_left = freq_L - math.floor(freq_L / 100) * 100 - ones_left
   hundreds_left = math.floor(freq_L / 100) * 100 

   ones_right = freq_R - math.floor(freq_R / 10) * 10
   decades_right = freq_R - math.floor(freq_R / 100) * 100 - ones_right
   hundreds_right = math.floor(freq_R / 100) * 100

   -- define position for mode selector
   local mod = get(mode)
   if mod == 0 then mode_sel = 0 end
   if mod == 1 then mode_sel = 2 end
   if mod == 2 then mode_sel = 1 end
   if mod == 3 or mod == 4 then mode_sel = 3 end
   
   -- calculate signal indicator's needle position
   local adf_bearing = get(adf)
   if mod > 0 and get(fail) < 6 and power == 1 then
      if adf_bearing == 90 then signal = math.random(0, 4) - 50
         else signal = math.random(0, 4) + 30 
      end
   else signal = -60
   end

   -- calculate night light
    if get(world_green) > 0.75 then night = 0
       else night = 1
    end 

end  



components = {

    -- background image
    texture { 
        position = { 0, 0, 388, 284 },
        image = get(background)
    },

    -- background image
    textureLit { 
        position = { 0, 0, 388, 284 },
        image = get(background_lit),
        visible = function()
           if get(mode) > 0 and get(fail) < 6 and power == 1 then return true
           else return false
           end 
           return a        
        end
    }, 

    -- background image
    texture { 
        position = { 0, 0, 388, 284 },
        image = get(background_digits),
        visible = function()
           return night == 0
        end        
    },
    
    -- left disk indicator
    textureLit { 
        position = { 61, 118, 10, 10 },
        image = get(led),
        visible = function()
           local a
           if act == 0 and get(mode) > 0 and get(fail) < 6 and power == 1 then a = true
           else a = false
           end 
           return a 
        end,
    },    
    
    -- right disk indicator
    textureLit { 
        position = { 246, 118, 10, 10 },
        image = get(led),
        visible = function()
           local a
           if act == 1 and get(mode) > 0 and get(fail) < 6 and power == 1 then a = true
           else a = false
           end 
           return a 
        end,
    },    
        

    -- signal needle
    needle {
        position = { 111, 205, 57, 57  },
        image = get(signal_nd),
        angle = function ()
           return signal
        end
    
    },    

    
    -- left ones disk
    needle {
        position = { 25, 48, 40, 40 },
        image = get(ones_disk),
        angle = function ()
           return -90 + ones_left * 30
        end
    
    },

    -- right ones disk
    needle {
        position = { 204, 48, 40, 40 },
        image = get(ones_disk),
        angle = function ()
           return -90 + ones_right * 30
        end
    
    },
    
    -- left big disk
    needle {
        position = { 83, 20, 100, 100 },
        image = get(big_disk),
        angle = function ()
           return hundreds_left * 30 / 100 - 30
        end
    
    },

    -- right big disk
    needle {
        position = { 262, 20, 100, 100 },
        image = get(big_disk),
        angle = function ()
           return hundreds_right * 30 / 100 - 30
        end
    
    },        

    -- left small disk
    needle {
        position = { 104, 42, 56, 56 },
        image = get(small_disk),
        angle = function ()
           return decades_left * 36 / 10
        end
    
    },
    
    -- right small disk
    needle {
        position = { 284, 42, 56, 56 },
        image = get(small_disk),
        angle = function ()
           return decades_right * 36 / 10
        end
    
    },
    
    -- caps image
    texture { 
        position = { 74, 23, 296, 88 },
        image = get(caps)
    },   

    -- left handle
    needle {
        position = { 98, 36, 68, 68 },
        image = get(disk_handle),
        angle = function ()
           return decades_left * 36 / 10
        end
    
    },
    
    -- right handle
    needle {
        position = { 278, 36, 68, 68 },
        image = get(disk_handle),
        angle = function ()
           return decades_right * 36 / 10
        end
    
    },

    -- mode handle
    needle {
        position = { 183, 171, 76, 76 },
        image = get(mode_selector),
        angle = function ()
           return mode_sel * 30 - 60
        end
    
    },    

    -- select left/right disk
    switch {
        position = { 242, 230, 64, 18 },
        state = function()
            return get(active) ~= 0
        end,
        btnOn = get(tmb_right),
        btnOff = get(tmb_left),
        onMouseClick = function()
            if get(active) == 1 then
                set(active, 0)
                set(left_freq, freq_L)
                set(right_freq, freq_R)
            else
                set(active, 1)
                set(left_freq, freq_R)
                set(right_freq, freq_L)
            end 
            return true
        end   
    },   

    -- click zones for set left ones
    clickable {
       position = { 14, 50, 25, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            if ones_left > 0 then
               ones_left = ones_left - 1
               freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            end
            return true
        end
    },

    clickable {
       position = { 42, 50, 25, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            if ones_left < 9 then
               ones_left = ones_left + 1
               freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            end
            return true
        end
    },    
    -- click zones for set right ones
    clickable {
       position = { 195, 50, 25, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            if ones_right > 0 then
               ones_right = ones_right - 1
               freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            end
            return true
        end
    },

    clickable {
       position = { 225, 50, 25, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            if ones_right < 9 then
               ones_right = ones_right + 1
               freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            end
            return true
        end
    },

    -- click zones for set left decades
    clickable {
       position = { 75, 50, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            decades_left = decades_left - 10
            if decades_left == -10 then decades_left = 90 end
            freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            return true
        end
    },

    clickable {
       position = { 158, 50, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            decades_left = decades_left + 10
            if decades_left == 100 then decades_left = 0 end
            freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            return true
        end
    },       

    -- click zones for set right decades
    clickable {
       position = { 255, 50, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            decades_right = decades_right - 10
            if decades_right == -10 then decades_right = 90 end
            freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            
            return true
        end
    },

    clickable {
       position = { 335, 50, 30, 40 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            decades_right = decades_right + 10
            if decades_right == 100 then decades_right = 0 end
            freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            return true
        end
    },       
    
    -- click zones for set left hundreds
    clickable {
       position = { 115, 30, 35, 25 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            hundreds_left = hundreds_left - 100
            if hundreds_left == 0 then hundreds_left = 1200 end
            freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            return true
        end
    },

    clickable {
       position = { 115, 90, 35, 25 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            hundreds_left = hundreds_left + 100
            if hundreds_left == 1300 then hundreds_left = 100 end
            freq = hundreds_left + decades_left + ones_left
               if act == 0 then
                  set(left_freq, freq)
               else set(right_freq, freq)
               end
            return true
        end
    },       

    -- click zones for set right hundreds
    clickable {
       position = { 290, 30, 35, 25 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            hundreds_right = hundreds_right - 100
            if hundreds_right == 0 then hundreds_right = 1200 end
            freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            return true
        end
    },

    clickable {
       position = { 290, 90, 35, 25 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local freq
            hundreds_right = hundreds_right + 100
            if hundreds_right == 1300 then hundreds_right = 100 end
            freq = hundreds_right + decades_right + ones_right
               if act == 0 then
                  set(right_freq, freq)
               else set(left_freq, freq)
               end
            return true
        end
    },       
 
    -- click zones for set left ones
    clickable {
       position = { 183, 175, 30, 60 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateleft.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local mod
            if mode_sel > 0 then mode_sel = mode_sel - 1 end
            
            if mode_sel == 0 then mod = 0 end
            if mode_sel == 1 then mod = 2 end
            if mode_sel == 2 then mod = 1 end
            if mode_sel == 3 then mod = 3 end
            
            set(mode, mod)
            return true
        end
    },

    clickable {
       position = { 220, 175, 25, 60 },
        
       cursor = { 
            x = 10, 
            y = 28, 
            width = 16, 
            height = 16, 
            shape = loadImage("rotateright.png")
        },  
        
        onMouseClick = function(x, y, button) 
            local mod
            if mode_sel < 3 then mode_sel = mode_sel + 1 end
            
            if mode_sel == 0 then mod = 0 end
            if mode_sel == 1 then mod = 2 end
            if mode_sel == 2 then mod = 1 end
            if mode_sel == 3 then mod = 3 end
            
            set(mode, mod)
            return true
        end
    }, 
 

}

