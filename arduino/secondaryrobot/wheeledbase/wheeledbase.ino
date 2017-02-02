#include <Arduino.h>

#include "PIN.h"
#include "constants.h"
#include "instructions.h"
#include "addresses.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/PID.h"
#include "../../common/Codewheel.h"
#include "../../common/Odometry.h"
#include "../../common/TrajectoryPlanner.h"
#include "../../common/mathutils.h"

#define CONTROL_IN_POSITION 0

// Load the different modules

DCMotorsDriver driver;

DCMotor leftWheel;
DCMotor rightWheel;

DifferentialController positionController;
DifferentialController velocityController;

PID linearVelocityController;
PID angularVelocityController;

#if CONTROL_IN_POSITION
PID linearPositionController;
PID angularPositionController;
#else
PID linearPositionToVelocityController;
PID angularPositionToVelocityController;
#endif

Codewheel leftCodewheel;
Codewheel rightCodewheel;

Odometry odometry;

TrajectoryPlanner trajectory;

// Setup

void setup()
{
	// Communication
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(SET_OPENLOOP_VELOCITIES_OPCODE, SET_OPENLOOP_VELOCITIES);
	talks.bind(SET_VELOCITIES_OPCODE, SET_VELOCITIES);
	talks.bind(GOTO_OPCODE, GOTO);
	talks.bind(SET_POSITION_OPCODE, SET_POSITION);
	talks.bind(GET_POSITION_OPCODE, GET_POSITION);
	talks.bind(GET_VELOCITIES_OPCODE, GET_VELOCITIES);
	talks.bind(SET_PID_TUNINGS_OPCODE, SET_PID_TUNINGS);
	talks.bind(GET_PID_TUNINGS_OPCODE, GET_PID_TUNINGS);

	// DC motors wheels
	driver.attach(DRIVER_RESET, DRIVER_FAULT);
	driver.reset();

	leftWheel.attach(LEFT_MOTOR_EN, LEFT_MOTOR_PWM, LEFT_MOTOR_DIR);
	leftWheel.setConstants(DCMOTORS_VELOCITY_CONSTANT, DCMOTORS_REDUCTION_RATIO);
	leftWheel.setSuppliedVoltage(DCMOTORS_SUPPLIED_VOLTAGE);
	leftWheel.setRadius(LEFT_WHEEL_RADIUS);

	rightWheel.attach(RIGHT_MOTOR_EN, RIGHT_MOTOR_PWM, RIGHT_MOTOR_DIR);
	rightWheel.setConstants(-DCMOTORS_VELOCITY_CONSTANT, DCMOTORS_REDUCTION_RATIO);
	rightWheel.setSuppliedVoltage(DCMOTORS_SUPPLIED_VOLTAGE);
	rightWheel.setRadius(RIGHT_WHEEL_RADIUS);

	// Engineering control
	velocityController.setAxleTrack(WHEELS_AXLE_TRACK);
	velocityController.setWheels(leftWheel, rightWheel);
	velocityController.setControllers(linearVelocityController, angularVelocityController);
	velocityController.disable();

#if CONTROL_IN_POSITION
	positionController.setAxleTrack(WHEELS_AXLE_TRACK);
	positionController.setWheels(leftWheel, rightWheel);
	positionController.setControllers(linearPositionController, angularPositionController);
	positionController.disable();
#endif

	linearVelocityController .loadTunings(LINEAR_VELOCITY_PID_ADDRESS);
	angularVelocityController.loadTunings(ANGULAR_VELOCITY_PID_ADDRESS);
	
#if CONTROL_IN_POSITION
	linearPositionController .loadTunings(LINEAR_POSITION_PID_ADDRESS);
	angularPositionController.loadTunings(ANGULAR_POSITION_PID_ADDRESS);
	//TODO: set outputs limits
#else
	linearPositionToVelocityController .loadTunings(LINEAR_POSITION_TO_VELOCITY_PID_ADDRESS);
	angularPositionToVelocityController.loadTunings(ANGULAR_POSITION_TO_VELOCITY_PID_ADDRESS);
	linearPositionToVelocityController .setOutputLimits(-MAX_LINEAR_VELOCITY,  +MAX_LINEAR_VELOCITY);
	angularPositionToVelocityController.setOutputLimits(-MAX_ANGULAR_VELOCITY, +MAX_ANGULAR_VELOCITY);
#endif

	// Odometry
	leftCodewheel.attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_Y);
	leftCodewheel.attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	leftCodewheel.setAxis(Y);
	leftCodewheel.setCountsPerRevolution(-CODEWHEELS_COUNTS_PER_REVOLUTION); // negative -> backward
	leftCodewheel.setRadius(LEFT_CODEWHEEL_RADIUS);
	leftCodewheel.reset();

	rightCodewheel.attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_X);
	rightCodewheel.attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	rightCodewheel.setAxis(X);
	rightCodewheel.setCountsPerRevolution(CODEWHEELS_COUNTS_PER_REVOLUTION); // positive -> forward
	rightCodewheel.setRadius(RIGHT_CODEWHEEL_RADIUS);
	rightCodewheel.reset();

	odometry.setWheels(leftCodewheel, rightCodewheel);
	odometry.setAxleTrack(CODEWHEELS_AXLE_TRACK);
	odometry.setTimestep(ODOMETRY_TIMESTEP);
	odometry.enable();

	// Trajectories
	trajectory.setThresholdRadius(WHEELS_AXLE_TRACK);
	trajectory.setTimestep(TRAJECTORY_TIMESTEP);
	trajectory.disable();

	// Miscellanous
	TCCR2B = (TCCR2B & 0b11111000) | 1; // Set Timer2 frequency to 16MHz instead of 250kHz
}

// Loop

void loop()
{	
	talks.execute();

	// Update odometry
	if (odometry.update())
	{
		trajectory.setCartesianPositionInput(odometry.getPosition());
		velocityController.setInputs(odometry.getLinearVelocity(), odometry.getAngularVelocity());
	}

	// Compute trajectory
	if (trajectory.update())
	{
		float linearPositionSetpoint  = trajectory.getLinearPositionSetpoint();
		float angularPositionSetpoint = trajectory.getAngularPositionSetpoint();
#if CONTROL_IN_POSITION
		positionController.setSetpoints(linearPositionSetpoint, angularPositionSetpoint);
		positionController.setInputs(0, 0);
#else
		float linearVelocitySetpoint  = linearPositionToVelocityController .compute(linearPositionSetpoint,  0, trajectory.getTimestep());
		float angularVelocitySetpoint = angularPositionToVelocityController.compute(angularPositionSetpoint, 0, trajectory.getTimestep());
		talks.out << millis() << "\n";
		talks.out << linearPositionSetpoint << " -> " << linearVelocitySetpoint << "\n";
		talks.out << angularPositionSetpoint << " -> " << angularVelocitySetpoint << "\n";
		velocityController.setSetpoints(linearVelocitySetpoint, angularVelocitySetpoint);
#endif
	}

	// Integrate engineering control
	velocityController.update();
#if CONTROL_IN_POSITION
	positionController.update();
#endif
}
