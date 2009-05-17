-- navigator panel

createProp("sim/custom/xap/gauge_power_avail", 'int');
set(globalPropertyi('sim/custom/xap/gauge_power_avail'), 1);

size = { 1024, 768 }

-- panel components
components = {

    -- background
    rectangle { 
        position = { 0, 0, size[1], size[2] };
        color = { 0.05, 0.05, 0.05, 1.0 };
    };

    -- ADF 1
    ark9_set {
        position = { 20, 320, 388*1.0, 284*1.0 };
    };
    
    -- ADF 2
    ark9_set {
        position = { 20, 20, 388*1.0, 284*1.0 };
        left_freq = globalPropertyf("sim/cockpit2/radios/actuators/adf2_frequency_hz");
        right_freq = globalPropertyf("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
        active = globalPropertyf("sim/cockpit2/radios/actuators/adf2_right_is_selected");
        mode = globalPropertyf("sim/cockpit2/radios/actuators/adf2_power"); 
        fail = globalPropertyf("sim/operation/failures/rel_adf2");
        adf = globalPropertyf("sim/cockpit2/radios/indicators/adf2_relative_bearing_deg");
    };

    -- clock
    achs1 { position = { 875, 600, 135, 135 }; };
    
    -- vertical speed indicator
    lun_1140_02_8 { position = { 875, 455, 135, 135 }, },
    
    -- airspeed indicator
    kus_730 { position = { 875, 310, 135, 135 }, },
    
    -- altitude indicator
    vd_10 { position = { 875, 165, 135, 135 }, },
    
    -- radio altimeter
    uv_3m { position = { 875, 20, 135, 135 }, },
    
    -- artifical horizon
    agd_1 { position = { 735, 20, 135, 135 }, },
    
    -- mach meter
    machmeter { position = { 595, 20, 135, 135 }, },
    
    -- NAV1 receiver
    nav1_set { position = { 20, 600, 220, 140 }, },
    
    -- NAV2 receiver
    nav2_set { position = { 230, 600, 220, 140 }, },
    
    -- COM1 receiver
    com_set { position = { 440, 600, 220, 140 }, },
    
    -- COM2 receiver
    com_set { 
        position = { 650, 600, 220, 140 }, 
        frequency = globalPropertyf("sim/cockpit2/radios/actuators/com2_frequency_hz");
    };

    -- large bloody ADF
    un1 { position = { 420, 320, 280, 280 }, },
 
    -- directional gyro
    nav_compas { position = { 720, 470, 140, 140 }, },
    
    -- VOR 1
    vor { position = { 720, 315, 140, 140 }, },
    
    -- VOR 2
    vor { 
        position = { 720, 160, 140, 140 }, 
        v_plank = globalPropertyf("sim/cockpit2/radios/indicators/nav2_hdef_dots_pilot"),
        h_plank = globalPropertyf("sim/cockpit2/radios/indicators/nav2_vdef_dots_pilot"),
        obs = globalPropertyf("sim/cockpit2/radios/actuators/nav2_obs_deg_mag_pilot"),
    },
    
    -- DME 1
    dme { position = { 490, 180, 100, 100 }, },
    
    -- DME 2
    dme { 
        position = { 600, 180, 100, 100 }, 
        distance = globalPropertyf("sim/cockpit/radios/nav2_dme_dist_m");
        hasDistance = globalPropertyf("sim/cockpit/radios/nav2_has_dme");
    },
    
    -- heading airpeed settings
    ap_hdg {  position = { 510, 20, 149*0.6, 56*0.6 } };
       
    -- vertical speed settings
    ap_vvi {  position = { 510, 50, 149*0.6, 56*0.6 } };

    -- metric autopilot altitude settings
    ap_alt {  position = { 510, 80, 149*0.6, 56*0.6 } };
    
    -- metric autopilot airpeed settings
    ap_speed {  position = { 510, 110, 149*0.6, 56*0.6 } };

    -- autopilot on/off switch
    ap_switch {  position = { 430, 195, 32, 64 } };
    
    -- autopilot hdg mode button
    ap_mode_btn {  
        bit = 2;
        position = { 485, 28, 16, 16 } 
    };
    
    -- autopilot vvi mode button
    ap_mode_btn {  
        bit = 16;
        position = { 485, 58, 16, 16 } 
    };
    
    -- autopilot alt hold mode button
    ap_mode_btn {  
        bit = 16384;
        position = { 485, 88, 16, 16 } 
    };
    
    -- autothrottle mode button
    ap_mode_btn {  
        bit = 1;
        position = { 485, 118, 16, 16 } 
    };

    -- autopilote labels
    texture {
        position = { 440, 2, 32, 155 };
        image = loadImage('aplabels.png');
    };
    
}

