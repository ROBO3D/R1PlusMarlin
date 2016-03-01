/*
 * WIFI_Manager.c
 *
 * Created: 23/4/2015 6:44:42 PM
 *  Author: Sherif
 */ 

#include "WIFI_Manager.h"
#include "SPI.h"
#include "WIFI_CC3000.h"
#include "WIFI_CC3000_Client.h"
#include "Marlin.h"

// ROBO_WIFI
void WifiManager_Init()
{
	SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
	SFE_CC3000_Client client = SFE_CC3000_Client(wifi);
	wifi_client = &client;
	MYSERIAL.wifi_client = &client;
	
	char ap_ssid[] = "RoBo3D!";                  // SSID of network
	char ap_password[] = "roborobo";          // Password of networks
	//char ap_ssid[] = "NETGEAR";                  // SSID of network
	//char ap_password[] = "1234abcd";          // Password of network
	unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
	unsigned int timeout = 30000;             // Milliseconds
	
	ConnectionInfo connection_info;
	
	SERIAL_ECHO_START;
	SERIAL_ECHOLNPGM("Wifi init");
	if (!wifi.init())
	{
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Wifi failed to initialize");
		return;
	}
	
	SERIAL_ECHO_START;
	SERIAL_ECHOLNPGM("Wifi initialized successfully");
	wifi.scanAccessPoints(1000);
	
	//InitFT843Screen();
	SERIAL_ECHO_START;
	SERIAL_ECHOPGM("Connecting to SSID: ");
	SERIAL_ECHOLN(ap_ssid);
	
	if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout)) {
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Error: Could not connect to AP");
	}
	
	if ( !wifi.getConnectionInfo(connection_info) )
	{
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Error: Could not obtain connection details");
	}
	else
	{
		SERIAL_ECHO_START;
		SERIAL_ECHOPGM("IP Address: ");
		for (int i = 0; i < IP_ADDR_LEN; i++)
		{
			MYSERIAL.print(connection_info.ip_address[i], DEC);
			if ( i < IP_ADDR_LEN - 1 )
			{
				SERIAL_ECHOPGM(".");
			}
		}
		
		SERIAL_ECHOLN("");
		
		client.initServer(2535);
		client.serverListen();
	}
}