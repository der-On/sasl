size = { 128, 64 }

-- location of slip ball, in deegrees
defineProperty("slipDeg", globalPropertyf("sim/cockpit2/gauges/indicators/slip_deg"))

-- image of sleep ball
defineProperty("ballImage", loadImage('slip-ball.png'))

-- draw slip ball
function draw()
    local deg = get(slipDeg)
    local posX = (128 - 16)/2 - deg * 6
    if 32 > posX then
        posX = 32
    elseif 128-32 < posX then
        posX = 128-32
    end
    local posY = 16 + math.abs(deg) * 0.2
    drawTexture(get(ballImage), posX, posY, 16, 16)
end

