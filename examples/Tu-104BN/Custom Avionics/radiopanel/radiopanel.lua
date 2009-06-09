-- radio operatoe panel

size = { 340, 250 }

-- panel components
components = {

    -- background
    rectangle { 
        position = { 0, 0, size[1], size[2] };
        color = { 0.05, 0.05, 0.05, 1.0 };
    };

    -- COM1 receiver
    com_set { position = { 20, 10, 220*0.7, 140*0.7 }, },
    
    -- COM2 receiver
    com_set { 
        position = { 180, 10, 220*0.7, 140*0.7 }, 
        frequency = globalPropertyf("sim/cockpit2/radios/actuators/com2_frequency_hz");
    };

    -- squawk
    transponder {
        position = { 20, 140, 250*1.25, 60*1.25 }
    };
};

