#include "door_board.hpp"

const double RETRY_DIST_PERC = 0.3;
state_next_door go_to_next_door(PathInfo & path, string door_num, int sclient, cv::VideoCapture & cap) {
	int distance = path.way_to_go[0].second;
	char order = path.way_to_go[0].first;
	bool not_find_yet = true;
	bool it_is_retry = false;
	while(not_find_yet)	{
			send_dist_go_order(order, sclient, distance);
			switch(detect_in_move(sclient,order, cap, door_num)){
					case Detect_Result::Detected_It:
				   		not_find_yet = false;	
						cout << "found perfect match" <<endl;
						cout<< "press enter to continue"<<endl;
						cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
						return state_next_door::SUCCESS;
						break;
					case Detect_Result::NOTHING:
						if(it_is_retry) {
								// retry failed.
								cout << "failed" <<endl;
								cout<< "press enter to continue"<<endl;
								cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
								return state_next_door::FOUNDED_NOTHING;
						}
						distance *= RETRY_DIST_PERC; 
						it_is_retry = true;
						break;
					default:
						break;
			}
	}		
}

Check_Result check(vector<string> & words, vector<float>& confidences, vector<Rect> & boxes, string & door_num, Mat & img) {
		int i =0;
		int match_count = 0;
		int possible_match_num = 0;
		for(i=0;i<words.size();++i) {
				if(confidences[i] > 50.0) {
						int j = 0;
						for(match_count = 0, j= 0;j<door_num.length() && j< words[i].length(); ++j)	{
						
								if(words[i][j] == door_num[j]) {
										++match_count; 
								} else {
										break;
								}
						}
						if(match_count == door_num.length()) {
								cv::rectangle(img, boxes[i].tl(), boxes[i].br(), Scalar(255, 0,0), 3);
								cout << "found Perfect_Match" <<endl;
								cout << words[i]<<endl;
								cout<< "press enter to continue"<<endl;
								cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
								return Check_Result::Perfect_Match;
						}else if(match_count >= door_num.length()/2 && match_count > 0) {
								cout << "found possible match" <<endl;
								cout << words[i]<<endl;
								cout<< "press enter to continue"<<endl;
								cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
								possible_match_num ++;
						}
				}
		}
		if(possible_match_num > 0) {
				return Check_Result::Suspect;
		} else{
				return Check_Result::Negative;
		}
}

Detect_Result detect_in_move(int sclient, char move_directiion, cv::VideoCapture &cap, string & door_num) {
		vector<string> words;
		vector<float> confidences;
		vector<cv::Rect> boxes;
		cv::Mat frame;
		while(true) {
				cap >> frame;
				if(frame.empty()) {
						std::cout<<"video over"<<std::endl;
						return Detect_Result::VIDEO_OVER;
				}
				words.clear();
				confidences.clear();
				boxes.clear();
				find_text(words, confidences, frame, boxes);
				cv::imshow("video", frame);
				if(words.size() != 0 ) {
						if(Check_Result::Perfect_Match == check(words, confidences, boxes, door_num, frame) ){
								cv::imshow("detect_in_move", frame);
								cv::waitKey(1);
								send_stop_order(sclient);
								return Detect_Result::Detected_It;
						}
				}
				waitKey(1);
				if(Dist_Go_State::Completed == peek_dist_go_result(sclient, move_directiion)) {
						return Detect_Result::NOTHING;
				}
				cv::imshow("detect_in_move", frame);
				cv::waitKey(1);
		}
}
