-- navigator panel

size = { 1024, 768 }

-- panel components
components = {

    -- background
    rectangle { 
        position = { 0, 0, size[1], size[2] };
        color = { 0.05, 0.05, 0.05, 1.0 };
    };

    -- ADF 1
    ark_set {
        position = { 20, 320, 388*1.0, 284*1.0 };
    };
    
    -- ADF 2
    ark_set {
        position = { 20, 20, 388*1.0, 284*1.0 };
        left_freq = globalPropertyf("sim/cockpit2/radios/actuators/adf2_frequency_hz");
        right_freq = globalPropertyf("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
        active = globalPropertyf("sim/cockpit2/radios/actuators/adf2_right_is_selected");
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
}

