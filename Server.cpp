#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

// Set Default values
RunServer::RunServer()
{

}

// Return Port Number
u_int16_t RunServer::get_port_number(struct sockaddr *s)
{

}

//Return Ip address
void * RunServer::get_ip_address(sockaddr *s)
{

}

// This function will create the socket for all the incoming client connections & update the client structure and pass
// this structure to Parse class
void RunServer::accept_connection()
{
	
}