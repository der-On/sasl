
size = { 2048, 2048 }

components = { 

    --frame { position = { 214, 925, 256, 256 }, },

    -- airspeed indicator
    kus_730 {
        position = { 214, 925, 256, 256 }, 
    },
    
    -- altitude indicator
    vd_10 { 
        position = { 780, 888, 320, 320 },
    },
   
    -- artifical horizon
    agd_1 {
        position = { 465, 895, 320, 315 },
    },
    
    -- radio altimeter
    uv_3m { 
        position = { 810, 1245, 256, 256 },
    },
   
    -- vertical speed indicator
    lun_1140_02_8 {
        position = { 214, 595, 270, 270 }, 
    },

    -- Metric DME 1
    dme_simple {
        position = { 845, 558, 90, 36 }, 
    },

    -- Metric DME 2
    dme_simple {
        position = { 950, 558, 90, 36 }, 
        distance = globalPropertyf("sim/cockpit/radios/nav2_dme_dist_m"),
        hasDistance = globalPropertyf("sim/cockpit/radios/nav2_has_dme"),
    },

    -- metric autopilot altitude settings
    ap_alt {  position = { 1703, 0, 149, 56 } };
    
    -- metric autopilot airpeed settings
    ap_speed {  position = { 1703, 70, 149, 56 } };
    
    -- heading airpeed settings
    ap_hdg {  position = { 1860, 70, 149, 56 } };
   
    -- vertical speed settings
    ap_vvi {  position = { 1860, 0, 149, 56 } };

    -- pressure settings
    pressure { position = { 1137, 642, 200, 159 } }
}


