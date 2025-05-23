#include "strafe_helper.h"
#include "strafe_helper_customization.h"
#include "../strafe_helper_includes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static float maxFloat(const float a, const float b)
{
	return a > b ? a : b;
}

static float minFloat(const float a, const float b)
{
	return a < b ? a : b;
}

static float square(const float value)
{
	return value * value;
}

static float sign(const float value)
{
	if (value < 0.0f)
	{
		return -1.0f;
	}
	else if (value > 0.0f)
	{
		return 1.0f;
	}
	return 0.0f;
}

static float crossProduct(const float v[2], const float w[2])
{
	return v[0] * w[1] - v[1] * w[0];
}

static float dotProduct(const float v[2], const float w[2])
{
	float dot_product = 0.0f;
	int i;
	for (i = 0; i < 2; i += 1) {
		dot_product += v[i] * w[i];
	}
	return dot_product;
}

static float angleBetweenVectors(const float v[2], const float w[2])
{
	return atan2f(crossProduct(v, w), dotProduct(v, w));
}

static float vectorAngleSign(const float v[2], const float w[2])
{
	return sign(crossProduct(v, w));
}

static float vectorNorm(const float v[2])
{
	return sqrtf(dotProduct(v, v));
}

struct HelperValues {
	float horizontal_norm;
	float z;
};

struct HelperValues computeHelperValues(const float vec[3])
{
	struct HelperValues helper;
	helper.horizontal_norm = vectorNorm(vec);
	helper.z = vec[2];
	return helper;
}

static float computeOptimalAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
	const struct HelperValues v, const struct HelperValues w,
	const float s, const float a, const float dt
)
{
	const float candidate1 = s * (1.0f - a * dt) - v.z * w.z;
	const float candidate2 = square(w.z) / (1.0f + square(w.z)) * (s - v.z * w.z);
	const float optimal_dot_product = maxFloat(candidate1, candidate2);
	const float optimal_scaled_dot_product =
		optimal_dot_product / (v.horizontal_norm * w.horizontal_norm);
	return acosf(minFloat(1.0f, optimal_scaled_dot_product));
}

static float computeSmallestAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
	const struct HelperValues v, const struct HelperValues w, const float s
)
{
	return acosf(minFloat(1.0f, (s - v.z * w.z) / (v.horizontal_norm * w.horizontal_norm)));
}

static float computeLargestAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
	const struct HelperValues v, const struct HelperValues w,
	const float s, const float a, const float dt
)
{
	const float candidate1 = -0.5f * s * a * dt;
	const float candidate2 = (v.z * w.z - s) / (2.0f - square(w.horizontal_norm));
	return acosf(maxFloat(candidate1, candidate2) * w.horizontal_norm / v.horizontal_norm);
}


/* The current angle between the players velocity vector and wishdir. */
static float angle_current;
/* The angle between the players velocity vector and wishdir that would have
 * resulted in the maximum amount of acceleration for the last reported
 * acceleration values. */
static float angle_optimal;
/* The (absolute) minimum and maximum angles between the players velocity
 * vector and wishdir that would give some acceleration. */
static float angle_minimum;
static float angle_maximum;

#ifndef STRAFE_HELPER_CUSTOMIZATION_DISABLE_DRAW_SPEED
/* The current player speed in the horizontal plane */
static float speed;
#endif

void StrafeHelper_SetAccelerationValues(const float velocity[3],
                                        const float wishdir[3],
                                        const float wishspeed,
                                        const float accel,
                                        const float frametime)
{
	const struct HelperValues velocity_helper = computeHelperValues(velocity);
	const struct HelperValues wishdir_helper = computeHelperValues(wishdir);

	const float angle_sign = vectorAngleSign(wishdir, velocity);
	const float two_pi = 2.0f * (float)M_PI;

	angle_optimal = angle_sign * computeOptimalAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
		velocity_helper, wishdir_helper, wishspeed, accel, frametime
	);

	angle_minimum = angle_sign * computeSmallestAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
		velocity_helper, wishdir_helper, wishspeed
	);

	angle_maximum = angle_sign * computeLargestAcceleratingHorizontalAngleBetweenVelocityAndWishdir(
		velocity_helper, wishdir_helper, wishspeed, accel, frametime
	);

	angle_current = angleBetweenVectors(wishdir, velocity);

	/* Make sure that angle_current fits well to the other angles. That is, try
	 * equivalent angles by adding or subtracting multiples of 2 * M_PI such
	 * that the angle values are closest to each other. That way we avoid
	 * differences greater than 2 * M_PI between the angles, which would break
	 * the drawing code. */
	angle_current += truncf((angle_minimum - angle_current) / two_pi) * two_pi;
	angle_current += truncf((angle_maximum - angle_current) / two_pi) * two_pi;

#ifndef STRAFE_HELPER_CUSTOMIZATION_DISABLE_DRAW_SPEED
	speed = velocity_helper.horizontal_norm;
#endif
}


static float angleDiffToPixelDiff(const float angle_difference, const float scale,
                                  const float hud_width)
{
	return angle_difference * (hud_width / 2.0f) * scale / (float)M_PI;
}

static float angleToPixel(const float angle, const float scale,
                          const float hud_width)
{
	return (hud_width / 2.0f) - 0.5f +
	       angleDiffToPixelDiff(angle, scale, hud_width);
}

void StrafeHelper_Draw(const struct StrafeHelperParams *params,
                       const float hud_width, const float hud_height)
{
	float angle_x;
	float angle_width;
#ifndef STRAFE_HELPER_CUSTOMIZATION_DISABLE_DRAW_SPEED
	char speed_string[8];  /* 7 digits should be more than enough for speed */
#endif

	const float upper_y = (hud_height - params->height) / 2.0f + params->y;

	float offset = 0.0f;
	if (params->center)	{
		offset = -angle_current;
	}

	if (angle_minimum < angle_maximum) {
		angle_x = angle_minimum + offset;
		angle_width = angle_maximum - angle_minimum;
	} else {
		angle_x = angle_maximum + offset;
		angle_width = angle_minimum - angle_maximum;
	}

	shc_drawFilledRectangle(
		angleToPixel(angle_x, params->scale, hud_width), upper_y,
		angleDiffToPixelDiff(angle_width, params->scale, hud_width),
		params->height, shc_ElementId_AcceleratingAngles);
	shc_drawFilledRectangle(
		angleToPixel(angle_optimal + offset, params->scale, hud_width) - 0.5f,
		upper_y, 2.0f, params->height, shc_ElementId_OptimalAngle);
	if (params->center_marker) {
		shc_drawFilledRectangle(
			angleToPixel(angle_current + offset, params->scale, hud_width) - 0.5f,
			upper_y + params->height / 2.0f, 2.0f, params->height / 2.0f,
			shc_ElementId_CenterMarker);
	}

#ifndef STRAFE_HELPER_CUSTOMIZATION_DISABLE_DRAW_SPEED
	if (params->speed_scale > 0.0f) {
		snprintf(speed_string, sizeof(speed_string), "%.0f", speed);
		shc_drawString(
			hud_width / 2.0f + params->speed_x,
			upper_y + params->height + params->speed_y,
			speed_string, params->speed_scale, shc_ElementId_SpeedText);
	}
#endif
}
