#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Wheeled base instructions

SET_OPENLOOP_VELOCITIES_OPCODE  = 0x04

GET_CODEWHEELS_COUNTERS_OPCODE  = 0x0D

SET_VELOCITIES_OPCODE           = 0x06

START_PUREPURSUIT_OPCODE        = 0x07
START_TURNONTHESPOT_OPCODE      = 0x09

POSITION_REACHED_OPCODE         = 0x08

SET_POSITION_OPCODE	            = 0x0A
GET_POSITION_OPCODE	            = 0x0B
GET_VELOCITIES_OPCODE           = 0x0C

SET_PARAMETER_VALUE_OPCODE      = 0x0E
GET_PARAMETER_VALUE_OPCODE      = 0x0F

RESET_PUREPURSUIT_OPCODE        = 0x10
ADD_PUREPURSUIT_WAYPOINT_OPCODE = 0x11

LEFTWHEEL_RADIUS_ID	            = 0x10
LEFTWHEEL_CONSTANT_ID           = 0x11
RIGHTWHEEL_RADIUS_ID            = 0x20
RIGHTWHEEL_CONSTANT_ID          = 0x21
LEFTCODEWHEEL_RADIUS_ID	        = 0x40
LEFTCODEWHEEL_COUNTSPERREV_ID   = 0x41
RIGHTCODEWHEEL_RADIUS_ID        = 0x50
RIGHTCODEWHEEL_COUNTSPERREV_ID  = 0x51
ODOMETRY_AXLETRACK_ID           = 0x60
VELOCITYCONTROL_AXLETRACK_ID    = 0x80
VELOCITYCONTROL_MAXLINACC_ID    = 0x81
VELOCITYCONTROL_MAXLINDEC_ID    = 0x82
VELOCITYCONTROL_MAXANGACC_ID    = 0x83
VELOCITYCONTROL_MAXANGDEC_ID    = 0x84
LINVELPID_KP_ID                 = 0xA0
LINVELPID_KI_ID                 = 0xA1
LINVELPID_KD_ID                 = 0xA2
LINVELPID_MINOUTPUT_ID          = 0xA3
LINVELPID_MAXOUTPUT_ID          = 0xA4
ANGVELPID_KP_ID                 = 0xB0
ANGVELPID_KI_ID                 = 0xB1
ANGVELPID_KD_ID                 = 0xB2
ANGVELPID_MINOUTPUT_ID	        = 0xB3
ANGVELPID_MAXOUTPUT_ID	        = 0xB4
POSITIONCONTROL_LINVELKP_ID     = 0xD0
POSITIONCONTROL_ANGVELKP_ID     = 0xD1
POSITIONCONTROL_LINVELMAX_ID    = 0xD2
POSITIONCONTROL_ANGVELMAX_ID    = 0xD3
POSITIONCONTROL_LINPOSTHRESHOLD_ID  = 0xD4
POSITIONCONTROL_ANGPOSTHRESHOLD_ID  = 0xD5
PUREPURSUIT_LOOKAHEAD_ID        = 0xE0
SMOOTHTRAJECTORY_THRESHOLDRADIUS_ID = 0xF0

# Modules collector instructions

_WRITE_DISPENSER_OPCODE    	= 0x04
_WRITE_GRIP_OPCODE      	= 0X05
_OPEN_GRIP_OPCODE			= 0x06
_SET_GRIP_VELOCITY_OPCODE	= 0X07
_IS_UP_OPCODE               = 0x08
_IS_DOWN_OPCODE             = 0x09
_GET_MOTOR_VELOCITY_OPCODE	= 0x0B
_SET_MOTOR_VELOCITY_OPCODE  = 0x0C

# Ultrasonic sensors instructions

_GET_MESURE_SENSOR_OPCODE   = 0x06


class WheeledBase(SerialTalksProxy):
	def __init__(self, parent, uuid='wheeledbase'):
		SerialTalksProxy.__init__(self, parent, uuid)

	def set_openloop_velocities(self, left, right):
		self.send(SET_OPENLOOP_VELOCITIES_OPCODE, FLOAT(left), FLOAT(right))

	def get_codewheels_counter(self, **kwargs):
		output = self.execute(GET_CODEWHEELS_COUNTERS_OPCODE, **kwargs)
		left, right = output.read(LONG, LONG)
		return left, right

	def set_velocities(self, linear_velocity, angular_velocity):
		self.send(SET_VELOCITIES_OPCODE, FLOAT(linear_velocity), FLOAT(angular_velocity))

	def purepursuit(self, waypoints, direction='forward'):
		self.send(RESET_PUREPURSUIT_OPCODE)
		for x, y in waypoints:
			self.send(ADD_PUREPURSUIT_WAYPOINT_OPCODE, FLOAT(x), FLOAT(y))
		self.send(START_PUREPURSUIT_OPCODE, BYTE({'forward':0, 'backward':1}[direction]))

	def turnonthespot(self, theta):
		self.send(START_TURNONTHESPOT_OPCODE, FLOAT(theta))

	def isarrived(self, **kwargs):
		output = self.execute(POSITION_REACHED_OPCODE, **kwargs)
		isarrived = output.read(BYTE)
		return bool(isarrived)

	def wait(self, timestep=0.1, **kwargs):
		while not self.isarrived(**kwargs):
			time.sleep(timestep)

	def goto(self, x, y, theta=None, direction=None, **kwargs):
		# Compute the preferred direction if not set
		if direction is None:
			x0, y0, theta0 = self.get_position()
			if math.cos(math.atan2(y - y0, x - x0) - theta0) >= 0:
				direction = 'forward'
			else:
				direction = 'backward'
		
		# Go to the setpoint position
		self.purepursuit([(x, y)], direction)
		self.wait(**kwargs)
		
		# Get the setpoint orientation
		if theta is not None:
			self.turnonthespot(theta)
			self.wait(**kwargs)

	def stop(self):
		self.set_openloop_velocities(0, 0)

	def set_position(self, x, y, theta):
		self.send(SET_POSITION_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def reset(self):
		self.set_position(0, 0, 0)

	def get_position(self, **kwargs):
		output = self.execute(GET_POSITION_OPCODE, **kwargs)
		x, y, theta = output.read(FLOAT, FLOAT, FLOAT)
		return x, y, theta
	
	def get_velocities(self, **kwargs):
		output = self.execute(GET_VELOCITIES_OPCODE, **kwargs)
		linvel, angvel = output.read(FLOAT, FLOAT)
		return linvel, angvel

	def set_parameter_value(self, id, value, valuetype):
		self.send(SET_PARAMETER_VALUE_OPCODE, BYTE(id), valuetype(value))
	
	def get_parameter_value(self, id, valuetype):
		output = self.execute(GET_PARAMETER_VALUE_OPCODE, BYTE(id))
		value = output.read(valuetype)
		return value





class ModulesGripper(SerialTalksProxy):	
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.high_open_angle = 142
		self.low_open_angle = 80
		self.close_angle = 5

	def set_position(self,a):
		self.send(_WRITE_GRIP_OPCODE, INT(a))
	
	def set_velocity(self, a):
		self.send(_SET_GRIP_VELOCITY_OPCODE, FLOAT(a))

	def open_up(self):
		self.send(_OPEN_GRIP_OPCODE, INT(self.high_open_angle))

	def open_low(self):
		self.set_position(self.low_open_angle)

	def close(self):
		self.set_position(self.close_angle)


class ModulesDispenser(SerialTalksProxy):
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.open_dispenser_angle = 180
		self.close_dispenser_angle = 0
	
	def set_position(self,a, t): 
		self.send(_WRITE_DISPENSER_OPCODE, INT(a), FLOAT(t))

	def open(self):
		self.set_position(self.open_dispenser_angle, 1.5)

	def close(self):
		self.set_position(self.close_dispenser_angle, 1.5)
		



class ModulesElevator(SerialTalksProxy):
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.climbing_velocity = -11.1 
		self.going_down_velocity = 8
	
	def isup(self):
		output = self.execute(_IS_UP_OPCODE)
		isup = output.read(BYTE)
		return bool(isup)

	def isdown(self):
		output = self.execute(_IS_DOWN_OPCODE)
		isdown = output.read(BYTE)
		return bool(isdown)
	
	def set_velocity(self, a):
		output = self.execute(_SET_MOTOR_VELOCITY_OPCODE, FLOAT(a))
		moving = output.read(BYTE)
		return bool(moving)
	
	def get_velocity(self):
		output = self.execute(_GET_MOTOR_VELOCITY_OPCODE)
		velo = output.read(FLOAT)
		return float(velo)

	def go_up(self):
		if not self.set_velocity(self.climbing_velocity):
			raise RuntimeError("gripper not closed")
		while not self.isup():
			time.sleep(0.1)

	def go_down(self):
		if not self.set_velocity(self.going_down_velocity):
			raise RuntimeError('gripper not closed')
		while not self.isdown():
			time.sleep(0.1)

class Sensors(SerialTalksProxy):
	def __init__(self, parent, uuid='sensors'):
		SerialTalksProxy.__init__(self, parent, uuid)

	def get_mesure(self,**kwargs):
		output = self.execute(_GET_MESURE_SENSOR_OPCODE, **kwargs)
		ar,av=output.read(INT,INT)
		return ar,av
