--
-- switch panel on or off
--

-- panel to toggle
defineProperty("panel")

-- no image by default
defineProperty("image")

-- switch consists of single button with very simple logic
components = {
    button {
        image = image;

        onMouseClick = function()
            local p = get(panel)
            set(p.visible, not get(p.visible))
            return true
        end;
    }
}

