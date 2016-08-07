#include "door_board.hpp"
#include "lk_track.hpp"

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
                //adjust car position.
                adjust_car(cap, sclient, door_num);
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
/* adjust car to the center of the front of the door.
 * ---------------------------------------------------
 *  @cap: video capture to get the image.
 *  @sclient: socket port to the car adaptor.
 *  @door_board: written of the door.
 *  ---------------------------------------------------
 *  return: adjust result(success or else).
 * */
Adjust_Result adjust_car(cv::VideoCapture & cap, int sclient, std::string & door_board) {
    const int TRY_LIMIT = 10;
    const double ACCURATE_ENOUGH = 0.05;
    const double SPEED_PERC = 0.1;
    cout << "adjust car start..." << endl;
    // #1 get the init position of the car board.
    Mat img, img_copy, img_copy2;
    int try_count = 0;
    cv::Rect board_rect;
    while(true) {
        if(try_count++ > TRY_LIMIT) {
            return Adjust_Result::Fake_DoorBoard;
        }
        cap >> img;
        img.copyTo(img_copy);
        img.copyTo(img_copy2);
        vector<string> words;
        vector<float> confidences;
        vector<cv::Rect> boxes;
        words.clear();
        confidences.clear();
        boxes.clear();
        find_text(words, confidences, img, boxes);
        cv::imshow("video", img);
        if(words.size() != 0 ) {
            if(Check_Result::Perfect_Match == check(words, confidences, boxes, door_board, img, board_rect) ){
                cout << __FILE__ << ": " << __LINE__ << "adjust_car:verifyed the door board." << endl;
                break;
            }
        }
    }
    // #2 init tracker.
    vector<cv::Point2f> points_prev, points_now;
    deque<int> tracked_pt_ids;
    init_track(board_rect, points_prev, points_now,img_copy, tracked_pt_ids);
    // #3 calculate the max dist to go.
    const int MAX_DIST_To_GO = 100;//1 meter.
    int dist_limit = MAX_DIST_To_GO * abs(board_rect.x + board_rect.width /2 - img.cols /2) / img.cols;
    char move_dir = '\0';
    if(board_rect.x + board_rect.width /2  - img.cols /2  > ACCURATE_ENOUGH * img.cols) {
        /*go forward.*/
        move_dir = 'D';
    }else if(board_rect.x + board_rect.width /2  - img.cols /2 < -ACCURATE_ENOUGH * img.cols ){
        /*go backward.*/
        move_dir = 'B';
    }else{
        /* close enough to the center of img. Don't need to move.*/
        return Adjust_Result::Succeed;
    }
    // #4 decrease the speed.
    change_speed(SPEED_PERC, sclient);
    // #5 make the car go.
    send_dist_go_order('D', sclient, dist_limit);
    // #6 start tracking.
    Mat prevGray;
    cv::cvtColor(img_copy2, prevGray, cv::COLOR_BGR2GRAY);
    track_it(points_prev, points_now, img, board_rect, cap, prevGray, tracked_pt_ids, false);
    // #7 stop car.
    send_stop_order(sclient);
    return Adjust_Result::Succeed;
}

Check_Result check(vector<string> & words, vector<float>& confidences, vector<Rect> & boxes, string & door_num, Mat & img, Rect & match_rect) {
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
                match_rect = boxes[i];
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
    cv::Rect match_rect;
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
            if(Check_Result::Perfect_Match == check(words, confidences, boxes, door_num, frame, match_rect) ){
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
