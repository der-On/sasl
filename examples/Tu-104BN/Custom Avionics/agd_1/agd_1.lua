size = { 290, 290 }

-- background image
defineProperty("background", loadImage("agd1-bg.png", size[1], size[2]))

-- attitude tape
defineProperty("tapeImage", loadImage("agd1-tape.png", 80, 720))

-- aircraft image
defineProperty("planeImage", loadImage("agd1-plane.png"))

-- aircraft pitch
defineProperty("pitch", globalPropertyf("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot"))
--defineProperty("pitch", globalPropertyf("sim/cockpit2/gauges/indicators/pitch_electric_deg_pilot"))

-- aircraft roll
defineProperty("roll", globalPropertyf("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot"))
--defineProperty("roll", globalPropertyf("sim/cockpit2/gauges/indicators/roll_electric_deg_pilot"))

-- height of visible window area
local winHeight = 80 / 720

-- height of one degrees in texture coord
local deg = 2 / 720

components = {

    -- attitude tape
    tape {
        position = { 65, 65, 160, 160 },
        image = get(tapeImage),
        window = { 1.0, 0.1111111111111 },

        -- calculate pitch level
        scrollY = function()
            return (0.5 - winHeight / 2) - deg * get(pitch);
        end,
    },

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- aircraft image
    needle {
        position = { 16, 16, 256, 256 },
        image = get(planeImage),
        angle = roll,
    },

    -- slip ball
    slip {
        position = { 81, 17, 128, 64 }
    }
}

