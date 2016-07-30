#include "communication.hpp"
#include "stdio.h"

int init_commu(std::string server_ip, int port) {
		int sock;
		struct sockaddr_in echoserver;
		if((sock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				std::cout << "Failed to create socket"<<std::endl;
				return -1;
		}
		memset(&echoserver, 0, sizeof(echoserver));/*clear struct*/
		echoserver.sin_family = AF_INET;/*Internet/IP*/	
		echoserver.sin_addr.s_addr = inet_addr(server_ip.c_str());
		echoserver.sin_port = htons(port);
		/*Establish Connection*/
		if(0 > connect(sock,(struct sockaddr *) &echoserver, sizeof(echoserver))) {
				std::cout<< "Error: connection failed"<<std::endl;
				return -2;
		}
		return sock;
}

bool send_dist_go_order(char direction, int sclient, int dist) {
		string cmd_2_car;
		cmd_2_car += direction;
		cmd_2_car += '\n';
		cmd_2_car += to_string(dist);
		cmd_2_car += '\n';
		
		string ack_msg;
		ack_msg += direction;
		ack_msg += "_ack\n";
	
		int sock_data_count = 0;
		char sock_recv_buf[100] = {0};

	do
	{
		sock_recv_buf[0] = 0;
		send(sclient, cmd_2_car.c_str(), cmd_2_car.size(), 0);
		cout << "waiting the ack completed..." << endl;
		sock_data_count = recv(sclient, sock_recv_buf, 100, 0);
		sock_recv_buf[sock_data_count] = 0;
		cout << sock_data_count << endl;
		printf("%s, %c", sock_recv_buf, sock_recv_buf[sock_data_count-1]);
	} while (0 !=strcmp(sock_recv_buf, ack_msg.c_str() ) );
	return true;
}

Dist_Go_State peek_dist_go_result(int sock, char direction) {
	char recv_buf[BUFFSIZE];
	int bytes_in_sock_buf = 0;
   	ioctl(sock, FIONREAD, &bytes_in_sock_buf);
	string ack_msg;
	ack_msg += direction;
	ack_msg += "_ok";
	if(bytes_in_sock_buf > 0) {
		bytes_in_sock_buf = recv(sock, recv_buf, BUFFSIZE, 0);
		recv_buf[bytes_in_sock_buf] = 0;
		printf("%d, %s",bytes_in_sock_buf, recv_buf);
		if(0 == strcmp(recv_buf, ack_msg.c_str())) {
			cout << "completed" << endl;
			return Dist_Go_State::Completed;
		}else {
			cout << "Unkown_Err" << endl;
			return Dist_Go_State::Unkown_Err;
		}
	} else {
		cout << "Going" << endl;
		return Dist_Go_State::Going;
	}
}

bool send_forward_order(int sock) {
	char recv_buf[BUFFSIZE];
	int recv_count;
	string cmd_2_car("f\n");
	int cmd_len = cmd_2_car.size();	
	while(strcmp(recv_buf, "f_ack\n") ){
		if(cmd_len != send(sock, cmd_2_car.c_str(), cmd_len, 0))	{
				std::cout<< "Warning: Miss match in number of sent bytes"<<std::endl;
				continue;	
		}
		recv_count = recv(sock, recv_buf, BUFFSIZE, 0);
		recv_buf[recv_count] = 0;	
	//	printf("%s", recv_buf);
	}
	return true;
}

bool send_stop_order(int sock) {
	char recv_buf[BUFFSIZE];
	int recv_count;
	string cmd_2_car("s\n");
	int cmd_len = cmd_2_car.size();	
	while(strcmp(recv_buf, "s_ack\n") ){
		if(cmd_len != send(sock, cmd_2_car.c_str(), cmd_len, 0))	{
				std::cout<< "Warning: Miss match in number of sent bytes"<<std::endl;
				continue;	
		}
		recv_count = recv(sock, recv_buf, BUFFSIZE, 0);
		recv_buf[recv_count] = 0;	
	}
	return true;
}

bool make_a_turn(char side, int degree, int sclient)
{
	string cmd_2_car;
	string complete_msg;
	complete_msg += side;
	complete_msg += "_ok\n";

	string ack_msg;
	ack_msg += side;
	ack_msg += "_ack\n";
	int sock_data_count = 0;
	char sock_recv_buf[100] = {0};
	cmd_2_car += side;
	cmd_2_car += '\n';
	cmd_2_car += to_string(degree);
	cmd_2_car += '\n';
	do
	{
		sock_recv_buf[0] = 0;
		send(sclient, cmd_2_car.c_str(), cmd_2_car.size(), 0);
#ifdef MY_DEBUG_SOCKET
		cout << "waiting the ack completed..." << endl;
#endif //MY_DEBUG_SOCKET
		sock_data_count = recv(sclient, sock_recv_buf, 100, 0);
		sock_recv_buf[sock_data_count] = 0;
	} while (0 != strcmp(sock_recv_buf, ack_msg.c_str()));
#ifdef MY_DEBUG_SOCKET
	cout << "acked" << endl;
#endif //MY_DEBUG_SOCKET
	sock_data_count = 0; 
	while (0 != strcmp(sock_recv_buf, complete_msg.c_str()))
	{
		//cout << "waiting the turning completed" << endl;
		sock_data_count = recv(sclient, sock_recv_buf, 100, 0);
		//cout << "waiting..." << endl;
		/*cap >> img_trush;*/
		if (sock_data_count > 0)
		{
			//sock_data_count = recv(sclient, sock_recv_buf, 100, 0);
			sock_recv_buf[sock_data_count] = '\0';
#ifdef MY_DEBUG_SOCKET
			cout << "recv:" << sock_recv_buf << endl;
#endif // MY_DEBUG_SOCKET
		}
	}
	return true;
}

