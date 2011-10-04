#include "listener.h"
#include "xpsdk.h"
#include "math3d.h"

using namespace xap;


// aircraft position in local coords
static XPLMDataRef localXRef;
static XPLMDataRef localYRef;
static XPLMDataRef localZRef;

// aircraft orientation in local coords
static XPLMDataRef pitchRef;
static XPLMDataRef rollRef;
static XPLMDataRef yawRef;

// is view external or internal
static XPLMDataRef viewIsExternalRef;


void xap::initListenerTracking()
{
    localXRef = XPLMFindDataRef("sim/flightmodel/position/local_x");
    localYRef = XPLMFindDataRef("sim/flightmodel/position/local_y");
    localZRef = XPLMFindDataRef("sim/flightmodel/position/local_z");
    
    pitchRef = XPLMFindDataRef("sim/flightmodel/position/theta");
    rollRef = XPLMFindDataRef("sim/flightmodel/position/phi");
    yawRef = XPLMFindDataRef("sim/flightmodel/position/psi");
    
    viewIsExternalRef = XPLMFindDataRef("sim/graphics/view/view_is_external");
}


void xap::updateListenerPosition(SASL sasl)
{
    double localX = XPLMGetDatad(localXRef);
    double localY = XPLMGetDatad(localYRef);
    double localZ = XPLMGetDatad(localZRef);

    double roll = XPLMGetDataf(rollRef);
    double yaw = XPLMGetDataf(yawRef);
    double pitch = XPLMGetDataf(pitchRef);
    Quat acfQuat(roll, yaw, -pitch);

    Matrix acfMat(acfQuat.toMatrix() * 
        translate(Vector(localX, localY, localZ)));

    XPLMCameraPosition_t camPos;
    XPLMReadCameraPosition(&camPos);
    Quat camQuat(-camPos.roll, -camPos.heading, camPos.pitch);

    Matrix camMat(translate(Vector(-camPos.x, -camPos.y, -camPos.z)) * 
        camQuat.toMatrix()); 
    Matrix tarMat = acfMat * camMat;
    Quat tarQuat = acfQuat * camQuat;
    Matrix tarRot(tarQuat.toMatrix());

    Vector p = tarMat * Vector(0, 0, 0);
    Vector dir = tarRot * Vector(0, 0, -1);
    dir.normalize();
    Vector up = tarRot * Vector(0, 1, 0);
    up.normalize();

    sasl_listener_set_position(sasl, p.x, p.y, p.z);
    sasl_listener_set_orientation(sasl, dir.x, dir.y, dir.z,
            up.x, up.y, up.z);
    if (XPLMGetDatai(viewIsExternalRef))
        sasl_listener_set_env(sasl, SOUND_EXTERNAL);
    else
        sasl_listener_set_env(sasl, SOUND_INTERNAL);
}


