
size = { 2048, 2048 }

-- power avialability
createProp("sim/custom/xap/gauge_power_avail", "int");  -- power for gauges and systems is exist


-- Pop-up navigator panel
navigatorPanel = subpanel {

    -- deafult panel position
    position = { 50, 100, 1024*0.8, 768*0.8 };

    -- disable default background
    noBackground = true;

    -- command ID for shortcut binding
    command = "xap/panels/1";

    -- panel descriptions for shortcuts setup dialog
    description = "Toggle Tu-104 navigator panel";

    -- components of panel
    components = {
        -- navigator panel body
        navpanel {  position = { 0, 0, 1024*0.8, 768*0.8 } };
    };
};


-- 3D cockpit
components = { 
    
    -- altitude indicator
    vd_10 { 
        position = { 440, 1675, 150, 150 },
    },
   
    -- airspeed indicator
    kus_730 {
        position = { 440, 1520, 150, 150 }, 
    },
    
    -- autopilot on/off switch
    ap_switch {  position = { 140, 1465, 32, 64 } };

    -- heading airpeed settings
    ap_hdg {  position = { 500, 1393, 149*0.6, 56*0.6 } };
    
    -- vertical speed settings
    ap_vvi {  position = { 500, 1421, 149*0.6, 56*0.6 } };

    -- metric autopilot altitude settings
    ap_alt {  position = { 500, 1451, 149*0.6, 56*0.6 } };
    
    -- metric autopilot airpeed settings
    ap_speed {  position = { 500, 1480, 149*0.6, 56*0.6 } };
    
    -- autopilote labels
    texture {
        position = { 448, 1375, 32, 155 };
        image = loadImage('aplabels.png');
    };
    
    -- autopilot hdg mode button
    ap_mode_btn {  
        bit = 2;
        position = { 480, 1403, 16, 16 } 
    };
    
    -- autopilot vvi mode button
    ap_mode_btn {  
        bit = 16;
        position = { 480, 1431, 16, 16 } 
    };
    
    -- autopilot alt hold mode button
    ap_mode_btn {  
        bit = 16384;
        position = { 480, 1461, 16, 16 } 
    };
    
    -- autothrottle mode button
    ap_mode_btn {  
        bit = 1;
        position = { 480, 1490, 16, 16 } 
    };
    
    -- directional gyro
    uk_2 {
        position = { 760, 1800, 170, 170 },
    },

    -- bank angle alerter
    bank_alert {
        position = { 763, 1768, 160, 25 },
    },

    -- artifical horizon
    agd_1 {
        position = { 755, 1580, 180, 180 },
    },
    
    -- second artifical horizon
    agi_1 {
        position = { 755, 1390, 180, 180 },
    },
    
    -- ILS
    psp48 {
        position = { 590, 1725, 150, 150 },
    },
    
    -- vertical speed indicator
    lun_1140_02_8 {
        position = { 590, 1580, 150, 150 }, 
    },

    -- radio altimeter
    uv_3m { 
        position = { 290, 1590, 150, 150 },
    },
    
    -- mach number indicator
    machmeter { 
        position = { 290, 1440, 150, 150 },
    },
   
    -- pressure settings
    pressure { position = { 227, 1242, 200*0.76, 159*0.76 } };

    -- show navigator panel
    togglePanelButton {
        position = { 220, 1590, 25, 25 };
        panel = navigatorPanel;
        image = loadImage("navigation.png");
    };
    
    -- ADF 1
    ugr_1 {
        position = { 940, 1870, 150, 150 },
    },
    
    -- VOR-DME
    rsbn {
        position = { 940, 1725, 150, 150 },
    },
    
    -- ADF 2
    ark_u2 {
        position = { 940, 1580, 150, 150 },
        adf = globalPropertyf("sim/cockpit2/radios/indicators/adf2_relative_bearing_deg"),
        fail = globalPropertyf("sim/operation/failures/rel_adf2"),
        mode = globalPropertyf("sim/cockpit2/radios/actuators/adf2_power"),
    },
    
    -- N1 at captain panel
    tacho {
        position = { 940, 1435, 150, 150 },
    },
    
    -- exhaust gas temperature
    eng_temp2 {
        position = { 940, 1290, 150, 150 },
    },

    -- three needle first engine indicator
    emi3 { 
        position = { 50, 195, 130, 130 },
    },
    
    -- three needle second engine indicator
    emi3 {
        position = { 690, 195, 130, 130 },
        fuel_p = globalPropertyf("sim/cockpit2/engine/indicators/fuel_pressure_psi[1]"),
        oil_p = globalPropertyf("sim/cockpit2/engine/indicators/oil_pressure_psi[1]"),
        oil_t = globalPropertyf("sim/cockpit2/engine/indicators/oil_temperature_deg_C[1]"),
    },

    -- update aircraft logic
    logic { },
}


