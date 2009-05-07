size = { 149, 56 }

defineProperty("altBackgroundImage", loadImage("ap-settings-bg.png", 51, 20))
defineProperty("digitsImage", loadImage("white-digits.png", 3, 0, 10, 196))

components = {
    ap_alt_rotary { position = { 74, 0, 56, 56 } };

    texture { 
        position = { 0, 15, 84, 28 };
        image = altBackgroundImage;
    };

    ap_alt_display { 
        position = { 0, 17, 80, 25 };
        image = digitsImage;
    };
}

