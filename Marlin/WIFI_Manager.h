/*
 * WIFI_Manager.h
 *
 * Created: 20/4/2015 1:48:42 PM
 *  Author: Sherif
 */ 


#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include "SPI.h"
#include "WIFI_CC3000.h"
#include "WIFI_CC3000_Client.h"
#include "Marlin.h"

#define CC3000_INT   21   // 3 on shield   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN    45   // 5  on shield // Can be any digital pin
#define CC3000_CS    47   // 10  // Preferred is pin 10 on Uno

// Connection info data lengths
#define IP_ADDR_LEN     4   // Length of IP address in bytes

// Constants

// Global Variables

//char* recvStr;

extern void WifiManager_Init();

#endif /* WIFI_MANAGER_H_ */