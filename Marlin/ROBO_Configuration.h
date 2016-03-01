/*
 * RoboConfiguration.h
 *
 * Created: 18/12/2014 6:18:49 AM
 *  Author: Sherif
 */ 


#ifndef ROBOCONFIGURATION_H_
#define ROBOCONFIGURATION_H_

#define ROBO_NONE		0
#define ROBO_R1			1
#define ROBO_MINI		2
#define ROBO_R2			3
#define ROBO_R2_RAMPS	6
#define ROBO_MEGA		4
#define ROBO_R1_PLUS	5

#define SDSUPPORT
#define ROBO_VERSION	ROBO_R1_PLUS

// EEPROM_DEFINITION
#if ROBO_VERSION == ROBO_MINI
#define EEPROM_SETTINGS
#endif

#endif /* ROBOCONFIGURATION_H_ */