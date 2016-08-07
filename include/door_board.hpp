#ifndef DOOR_BOARD_HPP
#define DOOR_BOARD_HPP

#include <vector>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <limits>
#include "communication.hpp"
#include "sceneText.hpp"
using namespace std;
enum class state_next_door{
		SUCCESS,
		FOUNDED_NOTHING,
		FOUNDED_OTHER_DOOR
};

enum class Detect_Result{
		Detected_It,
		NOTHING,
		VIDEO_OVER
};

enum class Check_Result{
		Perfect_Match,
		Suspect,
		Negative
};

enum class Adjust_Result{
    Fake_DoorBoard,
    Succeed,
};

class PathInfo {
public:
		// describing one path to next position.
		vector<pair<char, unsigned int> > way_to_go;	
		// feature of the destination.
		string door_num;
};


state_next_door go_to_next_door(PathInfo & path, string door_num, int sclient, cv::VideoCapture & cap);
Check_Result check(vector<string> & words, vector<float>& confidences, vector<Rect> & boxes, string & door_num, Mat & img, Rect & match_rect);
Detect_Result detect_in_move(int sclient, char move_directiion, cv::VideoCapture &cap, string & door_num);
Adjust_Result adjust_car(cv::VideoCapture & cap, int sclient, std::string & door_board);
#endif // DOOR_BOARD_HPP
