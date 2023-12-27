#include <stdio.h>
#include <stdlib.h>
#include <utility>

#include "PID.h"

/* Controller parameters */
#define PID_KP	2.0f
#define PID_KI	0.05f
#define PID_KD	0.5f

#define PID_TAU 0.3f

#define PID_LIM_MIN -2.0f
#define PID_LIM_MAX  2.0f

#define PID_LIM_MIN_INT -1.5f
#define PID_LIM_MAX_INT	 1.5f

#define SAMPLE_TIME_S 1.0f

/* Maximum run-time of simulation */
#define SIMULATION_TIME_MAX 50.0f

/* Simulated dynamical system (first order) */
float Plant(float inp);

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

		.proportional {
			.val = 0.0f
		},
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

	controller ctrller {std::move(para), plant_ctrl_t {0.0f, 100.0f, 20.0f}};

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
		"Integral\t"
		"Derivative\n");

	for (float t = 0.0f; t <= SIMULATION_TIME_MAX; t += SAMPLE_TIME_S) {

		float pid_output {ctrller.output()};

		/* Get measurement from system */
		float measurement = Plant(ctrller.ctrl_signal());

		/* Compute new control signal */
		ctrller.routine(setpoint_DP_psi, measurement);

		fprintf(outfp, "%.0fs,%f,%f,%f\n", t, measurement, pid_output, ctrller.ctrl_signal());
		fprintf(pidfp, "%f = %f + %f + %f\n", ctrller.ctrlerpara.output.val, ctrller.ctrlerpara.proportional.val, ctrller.ctrlerpara.integral.val, ctrller.ctrlerpara.derivative.val);
	}

	fclose(outfp);
	fclose(pidfp);
	return 0;
}


/*
 * Input : PID controller output
 * Output: Plant output
 */
float
Plant(float g_1)
{
	static float g_0 {0.0f}; // Last input of controller

	float output {SAMPLE_TIME_S * (g_1 + g_0) / 2};
	g_0 = g_1;

	return output;
}
