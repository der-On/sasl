size = { 160, 160 }

-- initialize component property table
defineProperty("vor", globalPropertyf("sim/cockpit/radios/nav2_dir_degt"))
defineProperty("fail", globalPropertyf("sim/operation/failures/rel_nav2"))
defineProperty("gauge_power",globalPropertyi("sim/custom/xap/gauge_power_avail"))

-- background image
defineProperty("background", loadImage("rsbn.png", 0, 0, 160, 160))

-- needle image
defineProperty("bearing_needle", loadImage("rsbn.png", 160, 14, 32, 128))


-- radiocompass consists of several components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background),
    },
    
    -- DME
    dme_simple {
        position = { 58, 100, 45, 18 }, 
        distance = globalPropertyf("sim/cockpit/radios/nav2_dme_dist_m"),
        hasDistance = globalPropertyf("sim/cockpit/radios/nav2_has_dme"),
    },

    -- bearing needle
    needle {
        position = { 16, 17, 128, 128 },
        image = get(bearing_needle),
        angle = vor,
    },  

   
}


