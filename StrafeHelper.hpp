#pragma once

#include "../../game/q_shared.h"


class StrafeHelper {
public:
	static void setAccelerationValues(vec_t const in_forward[],
		vec_t const in_velocity[], vec_t const in_wishdir[],
		double wishspeed, double accel, double frametime);

	static void setScale(float new_scale);
	static void draw(bool center, bool center_marker, float height, float y);

private:
	// The current angle between the players velocity vector and forward-looking
	// vector.
	static double currentAngle;
	// The angle between the players velocity vector and forward-looking vector
	// that would have resulted in the maximum amount of acceleration for the
	// last reported acceleration values.
	static double optimalAngle;
	// The (absolute) minimum and maximum angles between the players velocity
	// vector and forward-looking vector.
	static double minimumAngle;
	static double maximumAngle;

	static float scale;

	static float angleDiffToPixelDiff(float angle_difference);
	static float angleToPixel(float angle);
};
