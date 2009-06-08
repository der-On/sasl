size = { 160, 160 }

-- initialize component property table
defineProperty("v_plank", globalPropertyf("sim/cockpit2/radios/indicators/nav1_hdef_dots_pilot"))
defineProperty("h_plank", globalPropertyf("sim/cockpit2/radios/indicators/nav1_vdef_dots_pilot"))
defineProperty("nav_flag", globalPropertyf("sim/cockpit2/radios/indicators/nav1_flag_from_to_pilot"))
defineProperty("gs_flag", globalPropertyf("sim/cockpit/radios/nav1_CDI"))
defineProperty("frequency", globalPropertyf("sim/cockpit2/radios/actuators/nav1_frequency_hz"))

defineProperty("fail", globalPropertyf("sim/operation/failures/rel_nav1"))
defineProperty("gauge_power",globalPropertyi("sim/custom/xap/gauge_power_avail"))


-- background images
defineProperty("foreground", loadImage("psp48.png", 0, 0, 150, 150))
defineProperty("background", loadImage("psp48.png", 22, 154, 104, 98))

-- needle and flags images
defineProperty("plankNeedle", loadImage("psp48.png", 243, 76, 6, 167))
defineProperty("flag_nav_img", loadImage("psp48.png", 140, 216, 6, 12))
defineProperty("flag_gs_img", loadImage("psp48.png", 151, 191, 13, 7))


-- returns true if current beacon is ILS
function isIls()
    local freq = get(frequency)
    if (10810 > freq) or (11195 < freq) then
        return false
    end
    local v, f = math.modf(freq / 100)
    v = math.modf(f * 10)
    return 1 ~= (v % 2)
end

local vert
local horiz
local nav
local gs

function update()
    if not isIls() then
        vert = 0
        horiz = -90
        nav = true
        gs = true
    else
        -- calculate vertical and horizontal needle's positions
        vert = - get(v_plank) * 15  
        horiz = get(h_plank) * 15 - 90           
       
        -- flags logic
        nav = (get(fail) == 6 or get(gauge_power) == 0 or get(nav_flag) == 0)
        gs = (get(fail) == 6 or get(gauge_power) == 0 or get(gs_flag) == 0)
    end

    return true
end                                                              


-- ILS consists of several components
components = {

    -- background image
    texture { 
        position = { 27, 34, 104, 98 },
        image = background,
    },

    -- nav flag
    texture { 
        position = { 84, 57, 6, 12 },
        image = flag_nav_img,
        visible = function()
            return nav
        end
    }, 
    
    -- gs flag
    texture { 
        position = { 96, 88, 12, 7 },
        image = flag_gs_img,
        visible = function()
            return gs
        end
    },
   
   -- vertical plank
   needle {
        position = { -4, 45, 167, 167 },
        image = plankNeedle,
        angle = function() 
            return vert
        end
   
    }, 
    
   -- horizontal plank
   needle {
        position = { -50, -1, 167, 167 },
        image = plankNeedle,
        angle = function() 
            return horiz
        end
   
    }, 
 
    -- foreground image
    texture { 
        position = { 5, 5, 150, 150 },
        image = foreground,
    },    

   
}

