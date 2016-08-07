#ifndef LK_TRACK_HPP
#define LK_TRACK_HPP

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <deque>
#include <string>
using namespace std;
using namespace cv;
enum class TrackResult{
		arrived_dest,
		obstable, 
		passed_it,
};


bool init_track(cv::Rect & tracked_rect, std::vector<cv::Point2f> & points_prev, vector<cv::Point2f> &points_now, cv::Mat & frame, deque<int> &tracked_points_ids);
bool check_if_near_x_center(Rect& track_rect,  Size img_size);
TrackResult track_it(vector<Point2f> & points_prev, vector<Point2f> & points_now, Mat & frame,  Rect & track_rect, cv::VideoCapture & cap, Mat &prevGray,deque<int> &tracked_points_ids,bool default_step_mode=true,std::function<bool(Rect&, Size)> check_if_arrrived=check_if_near_x_center);

#endif // LK_TRACK_HPP
