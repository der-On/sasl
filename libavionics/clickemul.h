#ifndef __CLICK_EMULATOR_H__
#define __CLICK_EMULATOR_H__


#include "rttimer.h"


namespace xa {

/// Emulates mouse click events
class ClickEmulator
{
    private:
        /// Timer for clicks synchronization
        RtTimer &timer;

        /// Delay before second mouse click event
        int clickDelay;
        
        /// Delay before third and other mouse click events
        int clickPeriod;

        /// Time of mouse button down event
        long mouseButtonDownTime;

        /// Number of clicks events generated after mouse button down event
        int clickCounter;

        /// Button pressed
        int button;

        /// X coord of click
        int mouseX;
        
        /// Y coord of click
        int mouseY;

        /// Type of layer
        int layer;

    public:
        /// Create click events emulator
        ClickEmulator(RtTimer &timer);

    public:
        /// Called on mouse down events
        void onMouseDown(int button, int x, int y, int layer);

        /// Returns button number of simulated click event button or zero
        /// If no click simulation required
        int update();

        /// Called on mouse up events
        void onMouseUp();

        /// Set click emulation parameters
        /// \param delay number of milliseconds before second click event
        /// \param period number of milliseconds before third and other click events
        void setClickParams(int delay, int period);

        /// Returns button pressed
        int getButton() const { return button; }

        /// Returns X coord of click
        int getX() const { return mouseX; }

        /// Returns Y coord of click
        int getY() const { return mouseY; }
        
        /// Returns current layer
        int getLayer() const { return layer; }
};

}

#endif

