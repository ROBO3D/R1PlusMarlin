/**
 * @file    SFE_CC3000_Client.cpp
 * @brief   Library for the SparkFun CC3000 shield and breakout boards
 * @author  Shawn Hymel (SparkFun Electronics)
 * @author  Revisions by Jacob Rosenthal (https://github.com/jacobrosenthal)
 * 
 * @copyright   This code is public domain but you buy me a beer if you use
 * this and we meet someday (Beerware license).
 * 
 * The client library provides functions to connect to servers using sockets.
 */
 
#include "Arduino.h"
#include "SPI.h"
 
#include "WIFI_common.h"
#include "WIFI_CC3000.h"
#include "WIFI_CC3000_Client.h"
#include "WIFI_socket.h"
#include "WIFI_evnt_handler.h"

#include "Marlin.h"

 /**
  * @brief Constructor - Instantiates SFE_CC3000_Client object
  */
SFE_CC3000_Client::SFE_CC3000_Client(SFE_CC3000 &cc3000)
{
    cc3000_ = &cc3000;
    socket_ = -1;
}

/**
 * @brief Destructor
 */
SFE_CC3000_Client::~SFE_CC3000_Client()
{

}

/**
 * @brief Connects to a remote server using TCP
 *
 * @param[in] hostname the address of the remote server
 * @param[in] port the receiving port of the server (default: 80)
 * @return True if connected to remote server. False otherwise.
 */
int SFE_CC3000_Client::connect(    const char *hostname, 
                                    uint16_t port)
{
    IPAddress remote_ip;

    /* If CC3000 is not connected to a network, return false. */
    if (    !cc3000_->getInitStatus() || 
            !cc3000_->getConnectionStatus() || 
            !cc3000_->getDHCPStatus() ) {
        return false;
    }
    
    /* Perform a DNS lookup of the site */
    if (!cc3000_->dnsLookup(const_cast<char *>(hostname), &remote_ip)) {
        return false;
    }
    
    /* Connect to remote host using IP address */
    return connect(remote_ip, port);
}

/**
 * @brief Connects to a remote server using TCP
 *
 * @param[in] IP address of the remote server
 * @param[in] port the receiving port of the server (default: 80)
 * @return True if connected to remote server. False otherwise.
 */
int SFE_CC3000_Client::connect(    IPAddress ip_address, 
                                    uint16_t port)
{
    sockaddr dest_addr;
    int i;
                                    
    /* If CC3000 is not connected to a network, return false. */
    if (    !cc3000_->getInitStatus() || 
            !cc3000_->getConnectionStatus() || 
            !cc3000_->getDHCPStatus() ) {
        return false;
    }
    
    /* Create a socket */
    socket_ = socket(AF_INET, SOCK_STREAM, TCP);
    if (socket_ == -1) {
        return false;
    }
    
    /* Set address family to AF_INET (only one that works right now) */
    dest_addr.sa_family = AF_INET;
    
    /* Fill out the destination port */
    dest_addr.sa_data[0] = (port & 0xFF00) >> 8;
    dest_addr.sa_data[1] = (port & 0x00FF);
    
    /* Fill out the destination IP address */
    for (i = 0; i < 4; i++) {
        dest_addr.sa_data[i + 2] = ip_address[i];
    }
    
    /* Set the rest of the dest_addr struct to 0 */
    for (i = 6; i < 14; i++) {
        dest_addr.sa_data[i] = 0;
    }
    
    /* Attempt to make a connection with a remote socket */
    if (connect_to_socket(socket_, &dest_addr, sizeof(dest_addr)) != 
                                                        CC3000_SUCCESS) {
        close();
        return false;
    }

    return true;
}

/**
 * @brief Writes a single character to the socket
 *
 * @param[in] c the character to be written
 * @return the amount of data (in bytes) written
 */
size_t SFE_CC3000_Client::write(uint8_t c)
{
    return write(&c, 1);
}

/**
 * @brief Writes a string of characters to the socket
 *
 * @param[in] buf buffer of characters
 * @param[in] size the size (in bytes) of the buffer
 * @return the amount of data (in bytes) written
 */
size_t SFE_CC3000_Client::write(const uint8_t *buf, size_t size)
{
    /* If socket does not have a connection, return 0 */
    if (!connected()) {
        return 0;
    }
    
    /* Send buffer. Last parameter (flags) is not yet implemented by TI. */
    return send(socket_, buf, size, 0);
}

/**
 * @brief Connects to a remote server using UDP
 *
 * @param[in] hostname the address of the remote server
 * @param[in] port the receiving port of the server (default: 80)
 * @return True if connected to remote server. False otherwise.
 */
int SFE_CC3000_Client::connectUDP(    const char *hostname, 
                                    uint16_t port)
{
    IPAddress remote_ip;

    /* If CC3000 is not connected to a network, return false. */
    if (    !cc3000_->getInitStatus() || 
            !cc3000_->getConnectionStatus() || 
            !cc3000_->getDHCPStatus() ) {
        return false;
    }
    
    /* Perform a DNS lookup of the site */
    if (!cc3000_->dnsLookup(const_cast<char *>(hostname), &remote_ip)) {
        return false;
    }

    //Serial.println(remote_ip);
    
    /* Connect to remote host using IP address */
    return connectUDP(remote_ip, port);
}

/**
 * @brief Connects to a remote server using UDP
 *
 * @param[in] IP address of the remote server
 * @param[in] port the receiving port of the server (default: 80)
 * @return True if connected to remote server. False otherwise.
 */
int SFE_CC3000_Client::connectUDP(    IPAddress ip_address, 
                                    uint16_t port)
{
    sockaddr dest_addr;
    int i;
                                    
    /* If CC3000 is not connected to a network, return false. */
    if (    !cc3000_->getInitStatus() || 
            !cc3000_->getConnectionStatus() || 
            !cc3000_->getDHCPStatus() ) {
        return false;
    }
    
    /* Create a socket */
    socket_ = socket(AF_INET, SOCK_STREAM, UDP);
    if (socket_ == -1) {
        return false;
    }
    
    /* Set address family to AF_INET (only one that works right now) */
    dest_addr.sa_family = AF_INET;
    
    /* Fill out the destination port */
    dest_addr.sa_data[0] = (port & 0xFF00) >> 8;
    dest_addr.sa_data[1] = (port & 0x00FF);
    
    /* Fill out the destination IP address */
    for (i = 0; i < 4; i++) {
        dest_addr.sa_data[i + 2] = ip_address[i];
    }
    
    /* Set the rest of the dest_addr struct to 0 */
    for (i = 6; i < 14; i++) {
        dest_addr.sa_data[i] = 0;
    }
    
    /* Attempt to make a connection with a remote socket */
    if (connect_to_socket(socket_, &dest_addr, sizeof(dest_addr)) != 
                                                        CC3000_SUCCESS) {
        close();
        return false;
    }

    return true;
}

/**
 * @brief Determines if data is available for reading
 *
 * @return True if socket contains data to be read. False otherwise.
 */
int SFE_CC3000_Client::available()
{
    fd_set readsds;
    timeval timeout;

    /* We need something in readsds to tell select() to watch read sockets */
    memset(&readsds, 1, sizeof(readsds));
    
    /* Minimum timeout for select() is 5ms */
    timeout.tv_sec = 0;
    timeout.tv_usec = 5000;
    
    /* Call select() to see if there is any data waiting */
    int ret = select(socket_ + 1, &readsds, NULL, NULL, &timeout);
    
    /* If select() returned anything greater than 0, there's data for us */
    if (ret > 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief reads 1 byte from the socket
 *
 * @return received data. -1 if no data is available
 */
int SFE_CC3000_Client::read()
{
    uint8_t b;
    
    if (recv(socket_, &b, 1, 0) > 0) {
        return b;
    } else {
        return -1;
    }
}

/**
 * @brief reads data from the socket
 *
 * @param[out] buf the buffer onto which the data is written
 * @param[in] the length (in bytes) of the buffer
 * @return number of bytes received. -1 if an error occurred
 */
int SFE_CC3000_Client::read(uint8_t *buf, size_t size)
{
    /* Note: Flags arg is not yet supported by TI library */
    return recv(socket_, buf, size, 0);
}

/**
 * @brief Closes the socket
 *
 * @return True if socket closed successfully. False otherwise.
 */
bool SFE_CC3000_Client::close()
{
    long ret;

    /* Attempt to close the socket and set connected state to false */
    ret = closesocket(socket_);
    socket_ = -1;
    g_socket_connected = false;
    return (ret == CC3000_SUCCESS) ? true : false;
}

/**
 * @brief Gets the status of the socket's connection
 *
 * @return True if socket is connected. False otherwise.
 */
uint8_t SFE_CC3000_Client::connected()
{
    /* If there is no socket, return false */
    if (socket_ == -1) {
        return false;
    }
    
    /* If the connection was closed and there is no data, return false */
    if ((!g_socket_connected) && (!available())) {
        return false;
    }
    
    return true;
}

/**
 * @brief NOT AVAILABLE IN CC3000 recv currently, necessary for Arduino Client implementation.
 *
 * @return -1 always
 */
 int SFE_CC3000_Client::peek(){
  return -1;
}

/**
 * @brief Discard any bytes that have been written to the client but not yet read.
 *
 * @return none
 */
void SFE_CC3000_Client::flush(){
  while(available())
    read();
}

/**
 * @brief Alias for close necessary for Arduino Client implementation.
 *
 * @return none
 */
void SFE_CC3000_Client::stop(){
  close();
}

/**
 * @brief Inits server socket to start listening for incoming connections
 * 
 * @param[in] The port number that the server listens on
 * 
 * @return none
 */
void SFE_CC3000_Client::initServer(int port)
{
	unsigned long aucDHCP       = 14400;
	unsigned long aucARP        = 3600;
	unsigned long aucKeepalive  = 30;
	unsigned long aucInactivity = 0;
	
	if (netapp_timeout_values(&aucDHCP, &aucARP, &aucKeepalive, &aucInactivity) != 0) {
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Error setting inactivity timeout!");
		return;
	}
	
	sockaddr serverSocketAddr;
	
	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket_ == -1)
	{
		// Failed Creating Socket
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Failed on creating socket");
		return;
	}
	
	serverSocketAddr.sa_family = AF_INET;
	serverSocketAddr.sa_data[0] = (port & 0xFF00) >> 8;
	serverSocketAddr.sa_data[1] = (port & 0x00FF);
	memset(&serverSocketAddr.sa_data[2], 0, 4);
	
	if (bind(serverSocket_, &serverSocketAddr, sizeof(sockaddr)) != 0)
	{
		// Failed when binding socket to address
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Failed on binding");
		return;
	}
	
	if (listen(serverSocket_, MAX_CLIENTS) != 0)
	{
		// Failed on socket listening
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Failed on listening on socket");
		return;
	}
	
	if (setsockopt(serverSocket_, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, SOCK_ON, 1) != 0)
	{
		// Failed on non-blocking accept
		SERIAL_ECHO_START;
		SERIAL_ECHOLNPGM("Failed on non-blocking accept");
		return;
	}
	
	SERIAL_ECHO_START;
	SERIAL_ECHOLNPGM("Socket created and port bound");
}

/**
 * @brief Inits server socket to start listening for incoming connections
 * 
 * @param[in] The port number that the server listens on
 * 
 * @return none
 */

uint8_t currentClient = 0;
uint8_t clientConnected = 0;

void SFE_CC3000_Client::serverListen()
{
	char recvBuffer[70];
	sockaddr clientaddr;
	socklen_t addrlen;
	//uint8_t currentClient = 0;
	//uint8_t clientConnected = 0;
	uint8_t flag = 0;
	
	memset(recvBuffer, 0, 70);
	
	sendStringCount = 0;
	sendString = (char*)calloc(101, 1);
	
	int recvLength = 0;
	
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		clientsList[i] = -1;
	}
	
	//if (listen(serverSocket_, MAX_CLIENTS) != 0)
	//{
		//// Failed on socket listening
		//Serial.println(F("Failed on listening on socket"));
		//return;
	//}
	
	while (1)
	{
		addrlen = sizeof(clientaddr);
		if (!clientConnected)
		{
			clientsList[currentClient] = accept(serverSocket_, (sockaddr*)&clientaddr, &addrlen);			
		}
		
		if (clientsList[currentClient] >= 1 && clientsList[currentClient] <= 6) // Connected
		{
			// Client connected?
			//send(serverSocket_, "Hello world", 11, 0);
			//Serial.print("Client connected! Value: ");
			//Serial.println(clientsList[currentClient]);
			//sendFlag = 1;
			clientConnected = 1;
			MYSERIAL.wifi_serialPrint = 0;
			unsigned long recvTimeout = 1;
			if (setsockopt(clientsList[currentClient], SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &recvTimeout, sizeof(recvTimeout)) != 0)
			{
				// Failed on non-blocking RECV
				closesocket(clientsList[currentClient]);
				closesocket(serverSocket_);
				initServer(2535);
				clientConnected = 0;
				MYSERIAL.wifi_serialPrint = 1;
				sendFlag = 0;
				continue;
			}
		}
		else if (clientsList[currentClient] == -57)
		{
			// BUG: Socket inactive so reopen socket
			// Inactive Socket, close and reopen it
			closesocket(serverSocket_);
			initServer(2535);
			clientConnected = 0;
			MYSERIAL.wifi_serialPrint = 1;
			sendFlag = 0;
			continue;
		}
		else if (clientsList[currentClient] == 255) // Disconnected
		{
			closesocket(serverSocket_);
			initServer(2535);
			clientConnected = 0;
			MYSERIAL.wifi_serialPrint = 1;
			sendFlag = 0;
			continue;
		}
		else if (clientsList[currentClient] == 254) // Returned from accept without anything happening.
		{
			//Serial.println("Code 254");
		}
		else
		{
			// New Code not programmed yet
			//Serial.print(F("New Code. Value: "));
			//Serial.println(clientsList[currentClient]);
		}
		
		
		//hci_unsolicited_event_handler();
		
		if (clientConnected)
		{
			//Serial.println("Before Recv");
			recvLength = recv(clientsList[currentClient], recvBuffer, 100, 0);
			//Serial.print("recv Ret: ");
			//Serial.println(recvLength);
			if (recvLength == -57)
			{
				closesocket(serverSocket_);
				initServer(2535);
				clientConnected = 0;
				MYSERIAL.wifi_serialPrint = 1;
				sendFlag = 0;
				continue;
			}
				
			//uint16_t ret = send(clientsList[currentClient], "Hello world!", 13, NULL);
			
			if (recvLength > 0)
			{
				//SERIAL_ECHO_START;
				//MYSERIAL.print("Received: ");
				//MYSERIAL.print(recvBuffer);
				//MYSERIAL.print(". \t\tC: ");
				//MYSERIAL.println(recvLength);
				enquecommand(recvBuffer);
				memset(recvBuffer, 0, 70);
			}
		}
		
		loop();
	}
	
	//Serial.println(F("Listening finished"));
	
}

void SFE_CC3000_Client::sendStr(char* str, uint8_t count)
{
	if (clientConnected)
	{
		sendFlag = 0;
		SERIAL_ECHO_START;
		MYSERIAL.print("Sending: ");
		MYSERIAL.print(str);
		MYSERIAL.print(". \t\tC: ");
		MYSERIAL.println(count, DEC);
		uint16_t ret = send(clientsList[currentClient], str, count, NULL);
		//sendFlag = 1;
	}
}

void SFE_CC3000_Client::sendStr()
{
	if (clientConnected)
	{
		MYSERIAL.wifi_serialPrint = 1;
		sendFlag = 0;
		SERIAL_ECHO_START;
		MYSERIAL.print("Sending: ");
		MYSERIAL.print(sendString);
		MYSERIAL.print(". \t\tC: ");
		MYSERIAL.println(sendStringCount, DEC);
		uint16_t ret = send(clientsList[currentClient], sendString, sendStringCount, NULL);
		MYSERIAL.wifi_serialPrint = 0;
		sendStringCount = 0;
		memset(sendString, 0, 100);
		//sendFlag = 1;
	}
}

void SFE_CC3000_Client::enqueueChar(char c)
{
	if (clientConnected)
	{
		sendString[sendStringCount++] = c;
		if (sendStringCount == 100)
		{
			sendStr();
			
		}
		
	}
}

SFE_CC3000_Client::operator bool()
{
  return (cc3000_->getInitStatus() || 
            cc3000_->getConnectionStatus() || 
            cc3000_->getDHCPStatus());
}
    