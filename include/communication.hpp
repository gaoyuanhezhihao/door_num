#ifndef COMMUNICATION_HPP
#define	COMMUNICATION_HPP 

#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "netinet/in.h" 
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#define BUFFSIZE 32
using namespace std;
enum class Dist_Go_State{
		Completed,
		Obstacle,
		Unkown_Err,
		Going
};
Dist_Go_State peek_dist_go_result(int sock, char direction);
int init_commu(string server_ip, int port);
bool send_forward_order(int sock);
bool send_stop_order(int sock);
bool make_a_turn(char side, int degree, int sclient);
bool send_dist_go_order(char direction, int sclient, int dist); 

#endif //COMMUNICATION_HPP
