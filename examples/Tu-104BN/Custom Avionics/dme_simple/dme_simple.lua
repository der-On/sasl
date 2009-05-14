size = { 84, 32 }

-- background image
defineProperty("background", loadImage('dme-simple-bg.png'))

-- distance to dme
defineProperty("distance", globalPropertyf("sim/cockpit/radios/nav1_dme_dist_m"))

-- has dme or not
defineProperty("hasDistance", globalPropertyf("sim/cockpit/radios/nav1_has_dme"))

-- digits images
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

-- components
components = {

    -- background image
    texture { 
        position = { 0, 0, size[1], size[2] },
        image = get(background)
    },

    -- DME digits
    digitstape { 
        position = { 8, 4, size[1] - 16, size[2] - 8 };
        image = digitsImage;
        digits = 4;
        fractional = 1;

        value = function()
            return get(distance) * 1.852;
        end;

        valueEnabler = function()
            return (0 ~= get(hasDistance)) and (0 ~= get(distance));
        end;
    },
}

