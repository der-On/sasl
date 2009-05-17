
size = { 2048, 2048 }


-- Pop-up navigator panel
navigatorPanel = subpanel {
    position = { 50, 100, 1024*0.8, 768*0.8 };
    noBackground = true;

    components = {
        -- navigator panel body
        navpanel {  position = { 0, 0, 1024*0.8, 768*0.8 } };
    };
};


-- 3D cockpit
components = { 
    
    -- airspeed indicator
    kus_730 {
        position = { 370, 1450, 180, 180 }, 
    },
    
    -- altitude indicator
    vd_10 { 
        position = { 370, 1630, 180, 180 },
    },
   
    -- artifical horizon
    agd_1 {
        position = { 735, 1630, 180, 180 },
    },
    
    -- radio altimeter
    uv_3m { 
        position = { 210, 1500, 165, 165 },
    },
   
    -- vertical speed indicator
    lun_1140_02_8 {
        position = { 550, 1445, 185, 185 }, 
    },

    -- Metric DME 1
    dme_simple {
        position = { 1030, 1830, 90*0.7, 36*0.7 }, 
    },

    -- Metric DME 2
    dme_simple {
        position = { 1030, 1645, 90*0.7, 36*0.7 }, 
        distance = globalPropertyf("sim/cockpit/radios/nav2_dme_dist_m"),
        hasDistance = globalPropertyf("sim/cockpit/radios/nav2_has_dme"),
    },

    -- metric autopilot altitude settings
    ap_alt {  position = { 990, 1550, 149*0.80, 56*0.80 } };
    
    -- metric autopilot airpeed settings
    ap_speed {  position = { 990, 1589, 149*0.80, 56*0.80 } };
    
    -- heading airpeed settings
    ap_hdg {  position = { 990, 1475, 149*0.80, 56*0.80 } };
   
    -- vertical speed settings
    ap_vvi {  position = { 990, 1512, 149*0.8, 56*0.8 } };

    -- pressure settings
    pressure { position = { 227, 1242, 200*0.76, 159*0.76 } };

    -- show navigator panel
    togglePanelButton {
        position = { 320, 1690, 25, 25 };
        panel = navigatorPanel;
        image = loadImage("navigation.png");
    };
}


