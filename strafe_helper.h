#ifndef STRAFE_HELPER_H
#define STRAFE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


struct StrafeHelperParams {
    bool center;
    bool center_marker;
    float scale;
    float height;
    float y;
};

void StrafeHelper_SetAccelerationValues(const float forward[3],
                                        const float velocity[3],
                                        const float wishdir[3],
                                        float wishspeed, float accel,
                                        float frametime);
void StrafeHelper_Draw(struct StrafeHelperParams *params,
                       float hud_width, float hud_height);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // STRAFE_HELPER_H
