#include <stdio.h>
#include <stdlib.h>

#include "PID.h"

/* Controller parameters */
#define PID_KP	0.9f
#define PID_KI	0.01f
#define PID_KD	1.0f

#define PID_TAU 0.2f

#define PID_LIM_MIN -10.0f
#define PID_LIM_MAX  10.0f

#define PID_LIM_MIN_INT -6.0f
#define PID_LIM_MAX_INT	 6.0f

#define SAMPLE_TIME_S 1.0f

/* Maximum run-time of simulation */
#define SIMULATION_TIME_MAX 120.0f

/* Simulated dynamical system (first order) */
float TestSystem_Update(float inp);

int
main()
{
	const char *outfile_name = "output.csv";
	const char *pidout_name = "pid.csv";
	FILE *outfp = fopen(outfile_name, "w+");
	FILE *pidfp = fopen(pidout_name, "w+");
	if (!outfp || !pidfp) {
		perror(outfile_name);
		exit(EXIT_FAILURE);
	}

	/* Initialise PID controller */
	PIDController pid = { PID_KP, PID_KI, PID_KD,
			      PID_TAU,
			      PID_LIM_MIN, PID_LIM_MAX,
			      PID_LIM_MIN_INT, PID_LIM_MAX_INT,
			      SAMPLE_TIME_S };

	PIDController_Init(&pid);

	/* Simulate response using test system */
	float setpoint_DP_psi = 50.0f;
	control_signal_t ctrl = {0.0f, 100.0f, 20.0f};

	fprintf(outfp,
		"Time (sec),"
		"System Output (Differential pressure PSI),"
		"PID output (Delta part of control signal),"
		"Control signal (Pump duty %%)\n");

	fprintf(pidfp,
		"PID out  \t"
		"Proportional\t"
		"Integrator\t"
		"Differentiator\n");

	for (float t = 0.0f; t <= SIMULATION_TIME_MAX; t += SAMPLE_TIME_S) {

		/* Get measurement from system */
		float measurement = TestSystem_Update(pid.out);

		/* Compute new control signal */
		PIDController_Update(&pid, setpoint_DP_psi, measurement, pidfp);

		/* Update control signal */
		ctrl.out += pid.out;
		if (ctrl.out > ctrl.max)
			ctrl.out = ctrl.max;

		else if (ctrl.out < ctrl.min)
			ctrl.out = ctrl.min;

		fprintf(outfp, "%f,%f,%f,%f\n", t, measurement, pid.out, ctrl.out);
	}

	fclose(outfp);
	fclose(pidfp);
	return 0;
}

float
TestSystem_Update(float inp)
{
	static float output_DP = 0.0f;
	static const float alpha = 0.2f;

	output_DP = (SAMPLE_TIME_S * inp + output_DP) / (1.0f + alpha * SAMPLE_TIME_S);

	return output_DP;
}
