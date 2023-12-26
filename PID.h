#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

namespace pidcontrol {

	struct plant_ctrl_t {
		float out;
		float max;
		float min;
	};

	struct ctrler_para_t {
		struct gain_t {
			float Kp;
			float Ki;
			float Kd;
		} gain;

		/* Sample time (in seconds) */
		float T;

		struct proportional_t {
			float val;
		} proportional;

		struct integral_t {
			float val;
			float prevError;

			struct limit_t {
				float min;
				float max;
			} limit;
		} integral;

		struct derivative_t {
			float val;
			float tau; /* Derivative low-pass filter time constant */
			float prevMeasurement;
		} derivative;

		/* The output is a difference of control signal/command to the plant,
		 * which doesn't include the complete control signal/command.
		 */
		struct output_t {
			float val;
			struct limit_t {
				float min;
				float max;
			} limit;
		} output;

		float compute_output(float setpoint, float measurement);
	};

	class controller {
	public:
		controller(ctrler_para_t&&, plant_ctrl_t&&);

		ctrler_para_t ctrlerpara;
		plant_ctrl_t plantctrl;

		/*
		 * Run routine to update controller's update.
		 *
		 * The period of execution is sample time of the controller 
		 * which is specified in ctrler_para_t.T.
		 */
		void routine(float setpoint, float measurement);

		/* 
		 * controller's output is a difference compare to last ctrl_signal. 
		 */
		constexpr float output() const
		{ return ctrlerpara.output.val; }

		/*
		 * A complete signal outputs to the plant
		 */
		float ctrl_signal() const
		{ return plantctrl.out; }
	};

} // namespace pidcontrol

#endif /* PID_Control */
