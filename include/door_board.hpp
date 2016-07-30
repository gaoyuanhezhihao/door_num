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

class PathInfo {
public:
		// describing one path to next position.
		vector<pair<char, unsigned int> > way_to_go;	
		// feature of the destination.
		string door_num;
};


state_next_door go_to_next_door(PathInfo & path, string door_num, int sclient, cv::VideoCapture & cap);
Check_Result check(vector<string> & words, vector<float>& confidences, vector<Rect> & boxes, string & door_num, Mat & img);
Detect_Result detect_in_move(int sclient, char move_directiion, cv::VideoCapture &cap, string & door_num);
#endif // DOOR_BOARD_HPP
