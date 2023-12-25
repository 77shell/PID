#include <stdio.h>
#include <stdlib.h>
#include <utility>

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

using namespace pidcontrol;

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

	/* Initialise controller parameter */
	ctrler_para_t para {
		.gain {
			.Kp = PID_KP,
			.Ki = PID_KI,
			.Kd = PID_KD
		},
		.T = SAMPLE_TIME_S,
		.integral {
			.val = 0.0f,
			.prevError = 0.0f,
			.limit {
				.min = PID_LIM_MIN_INT,
				.max = PID_LIM_MAX_INT
			},
		},
		.derivative {
			.val = 0.0f,
			.tau = PID_TAU,
			.prevMeasurement = 0.0f
		},
		.output {
			.val = 0.0f,
			.limit {
				.min = PID_LIM_MIN,
				.max = PID_LIM_MAX
			}
		}
	};

	plant_ctrl_t plant_ctrl {0.0f, 100.0f, 20.0f};
	controller ctrller {std::move(para), plant_ctrl};

	/* Simulate response using test system */
	float setpoint_DP_psi = 50.0f;

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

		float pid_output {ctrller.output()};

		/* Get measurement from system */
		float measurement = TestSystem_Update(pid_output);

		/* Compute new control signal */
		ctrller.routine(setpoint_DP_psi, measurement);

		fprintf(outfp, "%f,%f,%f,%f\n", t, measurement, pid_output, ctrller.ctrl_signal());
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
