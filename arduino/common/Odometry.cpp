#include <Arduino.h>
#include <EEPROM.h>

#include "Odometry.h"
#include "SerialTalks.h"

#include <math.h>


void Odometry::process(float timestep)
{
	const float dL = m_leftCodewheel ->restart();
	const float dR = m_rightCodewheel->restart();

	const float deltaLinPos = (dL + dR) / 2;
	const float deltaAngPos = (dR - dL) / m_axleTrack;

	const float avgTheta = m_pos.theta + deltaAngPos / 2;
	m_pos.x     += deltaLinPos * cos(avgTheta);
	m_pos.y     += deltaLinPos * sin(avgTheta);
	m_pos.theta += deltaAngPos;

	m_linVel = deltaLinPos / timestep;
	m_angVel = deltaAngPos / timestep;
}

void Odometry::load(int address)
{
	EEPROM.get(address, m_axleTrack); address += sizeof(m_axleTrack);
}

void Odometry::save(int address) const
{
	EEPROM.put(address, m_axleTrack); address += sizeof(m_axleTrack);
}
