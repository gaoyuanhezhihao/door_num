#include "sceneText.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include "communication.hpp"
#include "door_board.hpp"
using namespace std;
const string Server_IP("127.0.0.1");
const int Server_Port = 8888;
int main(int argc, char **argv) {
		cv::VideoCapture cap(argv[1]);
		int sock = init_commu(Server_IP, Server_Port);
		PathInfo tmp_path;
		tmp_path.way_to_go.push_back(pair<char, int> ('D', 500));
		go_to_next_door(tmp_path, string ("1002"), sock, cap);
}
