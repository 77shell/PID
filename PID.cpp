#include "PID.h"

using namespace pidcontrol;

controller::controller(ctrler_para_t &&p, plant_ctrl_t &c)
	: ctrlerpara {p}, plantctrl {c}
{}

void
controller::routine(float setpoint, float measurement)
{
	float out {plantctrl.out};
	out += ctrlerpara.compute_output(setpoint, measurement);

	if (out > plantctrl.max)
		plantctrl.out = plantctrl.max;

	else if (out < plantctrl.min)
		plantctrl.out = plantctrl.min;

	else
		plantctrl.out = out;
}

float
ctrler_para_t::compute_output(float setpoint, float measurement)
{
	/*
	 * Error signal
	 */
	const float error {setpoint - measurement};

	/*
	 * Proportional
	 */
	const float proportional {gain.Kp * error};

	/*
	 * Integral
	 */
	integral.val = integral.val + 0.5f * gain.Ki * T * (error + integral.prevError);

	/* Integrator Anti-windup via dynamic integrator clamping
	 *
	 * Compute integrator limits */
	if (output.limit.max > proportional)
		integral.limit.max = output.limit.max - proportional;
	else
		integral.limit.max = 0.0f;

	if (output.limit.min < proportional)
		integral.limit.min = output.limit.min - proportional;
	else
		integral.limit.min = 0.0f;

	/* Anti-wind-up via integrator clamping */
	if (integral.val > integral.limit.max)
		integral.val = integral.limit.max;

	else if (integral.val < integral.limit.min)
		integral.val = integral.limit.min;

	/*
	 * Derivative (band-limited differentiator)
	 */
	derivative.val = -(2.0f * gain.Kd * (measurement - derivative.prevMeasurement) /* Note: derivative on measurement, therefore minus sign in front of equation! */
				+ (2.0f * derivative.tau - T) * derivative.val)
				/ (2.0f * derivative.tau + T);

	/*
	 * Compute output and apply limits
	 */
	float out = proportional + integral.val + derivative.val;

	if (out > output.limit.max)
		out = output.limit.max;

	else if (out < output.limit.min)
		out = output.limit.min;

	output.val = out;


	/* Store error and measurement for later use */
	integral.prevError = error;
	derivative.prevMeasurement = measurement;

	return out;
}
