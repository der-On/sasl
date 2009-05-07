#include "clickemul.h"
#include <stdio.h>


using namespace xa;


ClickEmulator::ClickEmulator(RtTimer &timer): timer(timer)
{
    clickDelay = 150;
    clickPeriod = 50;
    button = 0;
}


void ClickEmulator::setClickParams(int delay, int period)
{
    clickDelay = delay;
    clickPeriod = period;
}

void ClickEmulator::onMouseDown(int button, int x, int y, int l)
{
    this->button = button;
    mouseButtonDownTime = timer.getTime();
    clickCounter = 0;
    mouseX = x;
    mouseY = y;
    layer = l;
}

void ClickEmulator::onMouseUp()
{
    button = 0;
}

int ClickEmulator::update()
{
    if (button) {
        long time = timer.getTime();
        if (! clickCounter) {
            if (time - mouseButtonDownTime >= clickDelay) {
                mouseButtonDownTime = time;
                clickCounter = 1;
                return button;
            }
        } else {
            if (time - mouseButtonDownTime >= clickPeriod) {
                mouseButtonDownTime = time;
                return button;
            }
        }
    }

    return 0;
}

