#include "strafe_helper.h"
#include "../strafe_helper_includes.h"
#include <math.h>
#include <stdbool.h>


static double sign(const double value)
{
	return copysign(1.0, value);
}

static double vectorAngleSign(const double v[2], const double w[2])
{
	return sign(v[0] * w[1] - v[1] * w[0]);
}

static double dotProduct(const double v[2], const double w[2])
{
	double dot_product = 0.0;
	for (int i = 0; i < 2; i += 1) {
		dot_product += v[i] * w[i];
	}
	return dot_product;
}

static double vectorNorm(const double v[2])
{
	return sqrt(dotProduct(v, v));
}


/* The current angle between the players velocity vector and forward-looking
 * vector. */
static double angle_current;
/* The angle between the players velocity vector and forward-looking vector
 * that would have resulted in the maximum amount of acceleration for the
 * last reported acceleration values. */
static double angle_optimal;
/* The (absolute) minimum and maximum angles between the players velocity
 * vector and forward-looking vector that would give some acceleration. */
static double angle_minimum;
static double angle_maximum;

void StrafeHelper_SetAccelerationValues(const float in_forward[3],
                                        const float in_velocity[3],
                                        const float in_wishdir[3],
                                        const double wishspeed,
                                        const double accel,
                                        const double frametime)
{
	const double forward[2] = { in_forward[0], in_forward[1] };
	const double velocity[2] = { in_velocity[0], in_velocity[1] };
	const double wishdir[2] = { in_wishdir[0], in_wishdir[1] };
	const double v_z = in_velocity[2];
	const double w_z = in_wishdir[2];

	const double forward_norm = vectorNorm(forward);
	const double velocity_norm = vectorNorm(velocity);
	const double wishdir_norm = vectorNorm(wishdir);

	const double forward_velocity_angle = acos(dotProduct(forward, wishdir)
		/ (forward_norm * wishdir_norm)) * vectorAngleSign(wishdir, forward);

	const double angle_sign = vectorAngleSign(wishdir, velocity);

	angle_optimal = (wishspeed * (1.0 - accel * frametime) - v_z * w_z)
	                / (velocity_norm * wishdir_norm);
	angle_optimal = acos(angle_optimal);
	angle_optimal = angle_sign * angle_optimal - forward_velocity_angle;

	angle_minimum = (wishspeed - v_z * w_z) / (2.0 - wishdir_norm * wishdir_norm)
	                * wishdir_norm / velocity_norm;
	angle_minimum = acos(fmin(1.0, angle_minimum));
	angle_minimum = angle_sign * angle_minimum - forward_velocity_angle;

	angle_maximum = -0.5 * accel * frametime * wishspeed * wishdir_norm
	                / velocity_norm;
	angle_maximum = acos(angle_maximum);
	angle_maximum = angle_sign * angle_maximum - forward_velocity_angle;

	angle_current = dotProduct(velocity, forward) / (velocity_norm * forward_norm);
	angle_current = acos(angle_current);
	angle_current = vectorAngleSign(forward, velocity) * angle_current;
}


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double angleDiffToPixelDiff(const double angle_difference,
                                   const double scale,
                                   const double hud_width)
{
	return angle_difference * (hud_width / 2.0) * scale / M_PI;
}

static double angleToPixel(const double angle, const double scale,
                           const double hud_width)
{
	return (hud_width / 2.0) - 0.5 +
	       angleDiffToPixelDiff(angle, scale, hud_width);
}

void StrafeHelper_Draw(const struct StrafeHelperParams *params,
                       const double hud_width, const double hud_height)
{
	const double upper_y = (hud_height - params->height) / 2.0 + params->y;

	double offset = 0.0;
	if (params->center)	{
		offset = -angle_current;
	}

	double angle_x;
	double angle_width;
	if (angle_minimum < angle_maximum) {
		angle_x = angle_minimum + offset;
		angle_width = angle_maximum - angle_minimum;
	} else {
		angle_x = angle_maximum + offset;
		angle_width = angle_minimum - angle_maximum;
	}

	shi_drawFilledRectangle(
		angleToPixel(angle_x, params->scale, hud_width), upper_y,
		angleDiffToPixelDiff(angle_width, params->scale, hud_width),
		params->height, shi_color_accelerating);
	shi_drawFilledRectangle(
		angleToPixel(angle_optimal + offset, params->scale, hud_width) - 0.5,
		upper_y, 2.0, params->height, shi_color_optimal);
	if (params->center_marker) {
		shi_drawFilledRectangle(
			angleToPixel(angle_current + offset, params->scale, hud_width) - 0.5,
			upper_y + params->height / 2.0, 2.0, params->height / 2.0,
			shi_color_center_marker);
	}
}
