#include "StrafeHelper.hpp"
#include "../../cgame/cg_local.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

#undef min


template <typename T>
int sign(T const value)
{
	return (T(0) < value) - (value < T(0));
}

template <typename T, std::size_t N>
int vectorAngleSign(std::array<T, N> const &v, std::array<T, N> const &w)
{
	static_assert(N >= 2, "Array too small!");
	static_assert(N <= 3, "Array too large!");
	return sign(v[0] * w[1] - v[1] * w[0]);
}

template <typename T, std::size_t N>
typename T dotProduct(std::array<T, N> const &v, std::array<T, N> const &w)
{
	double dot_product = 0.0;
	for (std::size_t i = 0; i < N; i += 1)
	{
		dot_product += v[i] * w[i];
	}
	return dot_product;
}

template <typename T, std::size_t N>
typename T vectorNorm(std::array<T, N> const &vector)
{
	return std::sqrt(dotProduct(vector, vector));
}


double StrafeHelper::currentAngle;
double StrafeHelper::optimalAngle;
double StrafeHelper::minimumAngle;
double StrafeHelper::maximumAngle;
float StrafeHelper::scale;

void StrafeHelper::setAccelerationValues(vec_t const in_forward[],
		vec_t const in_velocity[], vec_t const in_wishdir[],
		double const wishspeed, double const accel, double const frametime)
{
	std::array<double, 2> forward = { in_forward[0], in_forward[1] };
	std::array<double, 2> velocity = { in_velocity[0], in_velocity[1] };
	std::array<double, 2> wishdir = { in_wishdir[0], in_wishdir[1] };
	double const v_z = in_velocity[2];
	double const w_z = in_wishdir[2];

	double const forward_norm = vectorNorm(forward);
	double const velocity_norm = vectorNorm(velocity);
	double const wishdir_norm = vectorNorm(wishdir);

	double const forward_velocity_angle = std::acos(dotProduct(forward, wishdir)
		/ (forward_norm * wishdir_norm)) * vectorAngleSign(wishdir, forward);

	int const angle_sign = vectorAngleSign(wishdir, velocity);

	optimalAngle = (wishspeed * (1.0 - accel * frametime) - v_z * w_z)
	               / (velocity_norm * wishdir_norm);
	optimalAngle = std::acos(optimalAngle);
	optimalAngle = angle_sign * optimalAngle - forward_velocity_angle;

	minimumAngle = (wishspeed - v_z * w_z) / (2.0 - wishdir_norm * wishdir_norm)
	               * wishdir_norm / velocity_norm;
	minimumAngle = std::acos(std::min(1.0, minimumAngle));
	minimumAngle = angle_sign * minimumAngle - forward_velocity_angle;

	maximumAngle = -0.5 * accel * frametime * wishspeed * wishdir_norm
	               / velocity_norm;
	maximumAngle = std::acos(maximumAngle);
	maximumAngle = angle_sign * maximumAngle - forward_velocity_angle;

	currentAngle = dotProduct(velocity, forward) / velocity_norm;
	currentAngle = std::acos(currentAngle);
	currentAngle = vectorAngleSign(forward, velocity) * currentAngle;
}

void StrafeHelper::setScale(float const new_scale)
{
	scale = new_scale;
}


constexpr float screen_width = 640.0f;
constexpr float screen_height = 480.0f;

float StrafeHelper::angleDiffToPixelDiff(float const angle_difference)
{
	return angle_difference * (screen_width / 2.0f) * scale / M_PI;
}

float StrafeHelper::angleToPixel(float const angle)
{
	return (screen_width / 2.0f) - 0.5f + angleDiffToPixelDiff(angle);
}

void StrafeHelper::draw(bool const center, bool const center_marker,
                        float const height, float const y) {
	float const upper_y = (screen_height - height) / 2.0f + y;

	double offset = 0.0;
	if (center)
	{
		offset = -currentAngle;
	}

	constexpr vec4_t accelerating_color = { 0.0f, 0.5f, 0.125f, 0.375f };
	if (minimumAngle < maximumAngle)
	{
		CG_FillRect(angleToPixel(minimumAngle + offset), upper_y,
		            angleDiffToPixelDiff(maximumAngle - minimumAngle), height,
		            accelerating_color);
	}
	else
	{
		CG_FillRect(angleToPixel(maximumAngle + offset), upper_y,
		            angleDiffToPixelDiff(minimumAngle - maximumAngle), height,
		            accelerating_color);
	}

	constexpr vec4_t optimal_color = { 0.0f, 1.0f, 0.25f, 0.75f };
	CG_FillRect(angleToPixel(optimalAngle + offset) - 0.5f, upper_y,
	            2.0f, height, optimal_color);

	if (center_marker)
	{
		constexpr vec4_t center_marker_color = { 1.0f, 1.0f,  1.0f, 0.75f };
		CG_FillRect(angleToPixel(currentAngle + offset) - 0.5f,
		            upper_y + height / 2.0f,
		            2.0f, height / 2.0f, center_marker_color);
	}
}
