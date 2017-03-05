#include "instructions.h"

#include "addresses.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/PID.h"
#include "../../common/Codewheel.h"
#include "../../common/Odometry.h"
#include "../../common/TrajectoryPlanner.h"

// Global variables

extern DCMotorsDriver driver;

extern DCMotor leftWheel;
extern DCMotor rightWheel;

extern DifferentialController positionController;
extern DifferentialController velocityController;

extern PID linearVelocityController;
extern PID angularVelocityController;

extern Codewheel leftCodewheel;
extern Codewheel rightCodewheel;

extern Odometry odometry;

extern TrajectoryPlanner trajectory;

// Instructions

void SET_OPENLOOP_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float leftVelocity  = input.read<float>();
	float rightVelocity = input.read<float>();

	velocityController.disable();
	trajectory.disable();
	leftWheel .setVelocity(leftVelocity);
	rightWheel.setVelocity(rightVelocity);
}

void SET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float linearVelocity  = input.read<float>();
	float angularVelocity = input.read<float>();
	trajectory.disable();
	velocityController.enable();
	velocityController.setSetpoints(linearVelocity, angularVelocity);
}

void START_TRAJECTORY(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float x     = input.read<float>();
	float y     = input.read<float>();
	float theta = input.read<float>();

	velocityController.enable();
	trajectory.reset();
	trajectory.addWaypoint(Position(x, y, theta));
	trajectory.enable();
}

void TRAJECTORY_ENDED(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	output.write<byte>(trajectory.hasReachedItsTarget());
}

void SET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float x     = input.read<float>();
	float y     = input.read<float>();
	float theta = input.read<float>();

	odometry.calibrateXAxis(x);
	odometry.calibrateYAxis(y);
	odometry.calibrateOrientation(theta);
}

void GET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	const Position& position = odometry.getPosition();
	
	output.write<float>(position.x);
	output.write<float>(position.y);
	output.write<float>(position.theta);
}

void GET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	const float linearVelocity  = odometry.getLinearVelocity ();
	const float angularVelocity = odometry.getAngularVelocity();
	
	output.write<float>(linearVelocity);
	output.write<float>(angularVelocity);
}

void SET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte  id = input.read<byte>();
	float Kp = input.read<float>();
	float Ki = input.read<float>();
	float Kd = input.read<float>();
	
	PID linearPositionToVelocityController;
	PID angularPositionToVelocityController;

	switch (id)
	{
	case LINEAR_POSITION_TO_VELOCITY_PID_IDENTIFIER:
		linearPositionToVelocityController.setTunings(Kp, Ki, Kd);
		linearPositionToVelocityController.saveTunings(LINEAR_POSITION_TO_VELOCITY_PID_ADDRESS);
		break;
	case ANGULAR_POSITION_TO_VELOCITY_PID_IDENTIFIER:
		angularPositionToVelocityController.setTunings(Kp, Ki, Kd);
		angularPositionToVelocityController.saveTunings(ANGULAR_POSITION_TO_VELOCITY_PID_ADDRESS);
		break;
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		linearVelocityController.setTunings(Kp, Ki, Kd);
		linearVelocityController.saveTunings(LINEAR_VELOCITY_PID_ADDRESS);
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		angularVelocityController.setTunings(Kp, Ki, Kd);
		angularVelocityController.saveTunings(ANGULAR_VELOCITY_PID_ADDRESS);
		break;
	default:
		talks.err << "SET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
	}
}

void GET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte id = input.read<byte>();
	float Kp, Ki, Kd;

	PID linearPositionToVelocityController;
	PID angularPositionToVelocityController;
	linearPositionToVelocityController .loadTunings(LINEAR_POSITION_TO_VELOCITY_PID_ADDRESS);
	angularPositionToVelocityController.loadTunings(ANGULAR_POSITION_TO_VELOCITY_PID_ADDRESS);

	switch (id)
	{
	case LINEAR_POSITION_TO_VELOCITY_PID_IDENTIFIER:
		Kp = linearPositionToVelocityController.getKp();
		Ki = linearPositionToVelocityController.getKi();
		Kd = linearPositionToVelocityController.getKd();
		break;
	case ANGULAR_POSITION_TO_VELOCITY_PID_IDENTIFIER:
		Kp = angularPositionToVelocityController.getKp();
		Ki = angularPositionToVelocityController.getKi();
		Kd = angularPositionToVelocityController.getKd();
		break;
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		Kp = linearVelocityController.getKp();
		Ki = linearVelocityController.getKi();
		Kd = linearVelocityController.getKd();
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		Kp = angularVelocityController.getKp();
		Ki = angularVelocityController.getKi();
		Kd = angularVelocityController.getKd();
		break;
	default:
		talks.err << "GET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
		return;
	}

	output.write<float>(Kp);
	output.write<float>(Ki);
	output.write<float>(Kd);
}

