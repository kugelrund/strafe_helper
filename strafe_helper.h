#ifndef STRAFE_HELPER_H
#define STRAFE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


struct StrafeHelperParams {
    bool center;
    bool center_marker;
    double scale;
    double height;
    double y;
};

void StrafeHelper_SetAccelerationValues(const float forward[3],
                                        const float velocity[3],
                                        const float wishdir[3],
                                        const double wishspeed,
                                        const double accel,
                                        const double frametime);
void StrafeHelper_Draw(const struct StrafeHelperParams *params,
                       const double hud_width, const double hud_height);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // STRAFE_HELPER_H
