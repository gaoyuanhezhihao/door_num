#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include <vector>
#include <string>
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace cv;
using namespace cv::text;
int find_text(vector<string>& words, vector<float>  &confidences, Mat & image,vector<Rect>&  boxes);
#endif// SCENE_TEXT_H
