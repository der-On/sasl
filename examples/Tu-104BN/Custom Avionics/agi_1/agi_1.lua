size = { 290, 290 }

-- background image
defineProperty("background", loadImage("agi1-bg.png", size[1], size[2]))

-- attitude tape
defineProperty("tapeImage", loadImage("agi1-tape.png", 80, 720))

-- aircraft pitch
defineProperty("pitch", globalPropertyf("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot"))

-- aircraft roll
defineProperty("roll", globalPropertyf("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot"))

-- height of visible window area
local winHeight = 80 / 720

-- height of one degrees in texture coord
local deg = 2 / 720

components = {

    -- attitude tape
    rotated_tape {
        position = { 58, 66, 174, 174 },
        image = get(tapeImage),
        window = { 1.0, 0.1111111111111 },
        angle = function() return -get(roll); end,

        -- calculate pitch level
        scrollY = function()
            return (0.5 - winHeight / 2) + deg * get(pitch);
        end,
    },

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- slip ball
    slip {
        position = { 81, 10, 128, 64 }
    }
}

