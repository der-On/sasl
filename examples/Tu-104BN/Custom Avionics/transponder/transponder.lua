size = {250, 60}


-- define peoperty table
defineProperty("xpdr_code", globalPropertyf("sim/cockpit/radios/transponder_code"))
defineProperty("xpdr_mode", globalPropertyf("sim/cockpit/radios/transponder_mode")) 
defineProperty("xpdr_led", globalPropertyf("sim/cockpit/radios/transponder_light"))
ident = findCommand("sim/transponder/transponder_ident")  -- comand of transponder ident

defineProperty("gauge_power", globalPropertyi("sim/custom/xap/gauge_power_avail"))
defineProperty("xpdr_fail", globalPropertyi("sim/operation/failures/rel_g_xpndr"))

defineProperty("flight_time", globalPropertyf("sim/time/total_flight_time_sec"))  -- local time since aircraft was loaded 


-- define image table
defineProperty("background", loadImage("transponder.png", 3, 4, 260, 60))
defineProperty("id_btn", loadImage("transponder.png", 149, 98, 28, 20))  

defineProperty("mode_knob_1", loadImage("transponder.png", 1, 96, 29, 29))
defineProperty("mode_knob_2", loadImage("transponder.png", 34, 96, 29, 29))
defineProperty("mode_knob_3", loadImage("transponder.png", 67, 96, 29, 29))
defineProperty("mode_knob_4", loadImage("transponder.png", 101, 96, 29, 29)) 

defineProperty("led", loadImage("transponder.png", 190, 104, 8, 8)) 

defineProperty("digitsImage", loadImage("red_digits.png", 3, 0, 10, 196))


local power = false       -- transponder's power


function getDigits(squawk)
    local d1 = math.floor(squawk / 1000)
    squawk = squawk - d1 * 1000
    local d2 = math.floor(squawk / 100)
    squawk = squawk - d2 * 100
    local d3 = math.floor(squawk / 10)
    local d4 = squawk - d3 * 10
    return d1, d2, d3, d4
end

-- set transponder code
function setSquawk(d1, d2, d3, d4)
   set(xpdr_code, d1 * 1000 + d2 * 100 + d3 * 10 + d4)
end

-- first digit of squawk code
defineProperty("code_1", 
        function()
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            return d1
        end)

-- second digit of squawk code
defineProperty("code_2", 
        function(self, value)
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            return d2
        end)

-- third digit of squawk code
defineProperty("code_3", 
        function(self, value)
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            return d3
        end)

-- last digit of squawk code
defineProperty("code_4", 
        function(self, value)
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            return d4
        end)



function update()
   power = get(xpdr_mode) > 0 and get(gauge_power) > 0 and get(xpdr_fail) < 6
end  


-- transponder cosist of several components

components = {

   -- background
   texture {
       position = {0, 0, 250, 60},
       image = get(background),
   },

   -- mode knob
   texture {
       position = {197.5, 15, 29, 29},
       image = get(mode_knob_1),
       visible = function()
          return get(xpdr_mode) == 0
       end,
   },
   texture {
       position = {197, 15, 29, 29},
       image = get(mode_knob_2),
       visible = function()
          return get(xpdr_mode) == 1
       end,
   },
   texture {
       position = {196.5, 15, 29, 29},
       image = get(mode_knob_3),
       visible = function()
          return get(xpdr_mode) == 2
       end,
   },
   texture {
       position = {197, 15, 29, 29},
       image = get(mode_knob_4),
       visible = function()
          return get(xpdr_mode) == 3
       end,
   },

   -- mode knob rotary
   rotary {
       position = {197, 15, 29, 29},
       value = xpdr_mode,
       step = 1,
       adjuster = function(v)
          if v > 3 then v = 3 end
          if v < 0 then v = 0 end
          return v
       end,
   },
   
   -- digits
   digitstape {
        position = { 63, 35, 10, 15},
        image = digitsImage,
        digits = 1,
        showLeadingZeros = false,
        value = code_1,
        visible = function()
           return power
        end,
    },
   
   digitstape {
        position = { 97, 35, 10, 15},
        image = digitsImage,
        digits = 1,
        showLeadingZeros = false,
        value = code_2,
        visible = function()
           return power
        end,
    },   

   digitstape {
        position = { 130, 35, 10, 15},
        image = digitsImage,
        digits = 1,
        showLeadingZeros = false,
        value = code_3,
        visible = function()
           return power
        end,
    },  

   digitstape {
        position = { 163, 35, 10, 15},
        image = digitsImage,
        digits = 1,
        showLeadingZeros = false,
        value = code_4,
        visible = function()
           return power
        end,
    }, 

    -- digit rotaries
    -- digit 1
    rotary {
        position = { 55, 1, 25, 25 },
        value = code_1;
        adjuster = function(v)
            if 0 > v then
                v = 7;
            elseif 7 < v then
                v = 0
            end
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            setSquawk(v, d2, d3, d4)
        end;
        visible = function()
           return power
        end,
    },
    
    -- digit 2
    rotary {
        position = { 89, 1, 25, 25 },
        value = code_2;
        adjuster = function(v)
            if 0 > v then
                v = 7;
            elseif 7 < v then
                v = 0
            end
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            setSquawk(d1, v, d3, d4)
        end;
        visible = function()
           return power
        end,
    },

    -- digit 3
    rotary {
        position = { 122, 1, 25, 25 },
        value = code_3;
        adjuster = function(v)
            if 0 > v then
                v = 7;
            elseif 7 < v then
                v = 0
            end
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            setSquawk(d1, d2, v, d4)
        end;
        visible = function()
           return power
        end,
    },

    -- digit 4
    rotary {
        position = { 155, 1, 25, 25 },
        value = code_4;
        adjuster = function(v)
            if 0 > v then
                v = 7;
            elseif 7 < v then
                v = 0
            end
            local d1, d2, d3, d4 = getDigits(get(xpdr_code))
            setSquawk(d1, d2, d3, v)
        end;
        visible = function()
           return power
        end,
    },


    -- ident button
    texture {
        position = {11, 33, 28, 20},
        image = get(id_btn),
        visible = function()
            return true -- get(xpdr_id) == 1
        end,
    },

    -- transponder light
    textureLit {
        position = { 19, 16, 8, 8},
        image = get(led),
        visible = function()
           return get(xpdr_led) == 1 and power
        end,
    
    },


    clickable {
       position = { 10, 25, 30, 30 },
        
       cursor = { 
            x = 8, 
            y = 26, 
            width = 16, 
            height = 16, 
            shape = loadImage("clickable.png")
        },  
        
        onMouseClick = function()
            commandOnce(ident)
            return true
        end
    },  




}

