#include "lk_track.hpp"

#define CENTER_RANGE_PERC 0.01
cv::Size const  subPixWinSize(10,10), winSize(31,31);
const cv::TermCriteria termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03);
bool init_track(cv::Rect & tracked_rect, std::vector<cv::Point2f> & points_prev, vector<cv::Point2f> &points_now, cv::Mat & frame, deque<int> &tracked_points_ids) {
    const int MAX_COUNT = 500;
    Mat gray;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    /* #1 extract sub image*/
    //cv::Mat subImg = frame(tracked_rect) ;
    //cv::Mat subImg_gray;
    //cv::cvtColor(subImg, subImg_gray, cv::COLOR_BGR2GRAY );
    /* #2 get feature points*/
    cv::goodFeaturesToTrack(gray, points_now, MAX_COUNT, 0.01, 10, cv::Mat(), 3, 0, 0.04 );
    cv::cornerSubPix(gray, points_now, subPixWinSize, cv::Size(-1, -1),termcrit);
    /* #3 convert to global image*/
    size_t i = 0;
    for(i = 0; i< points_now.size(); ++i)	{
        if(tracked_rect.tl().x  <=  points_now[i].x  && points_now[i].x  <= tracked_rect.br().x) {
            if(tracked_rect.tl().y <= points_now[i].y && points_now[i].y <= tracked_rect.br().y) {
                tracked_points_ids.push_back(i);
                circle( frame, points_now[i], 3, Scalar(0,0,255), -1, 8);
                continue;
            }
        }
        circle( frame, points_now[i], 3, Scalar(0,255,0), -1, 8);
    }
    /* #4 copy to previous frame's */
    std::swap(points_now, points_prev);
    //imshow("init track", frame);
    //waitKey(1);
    return true;	
} 

bool check_if_near_x_center(Rect& track_rect,  Size img_size) {
    int x_center_rect = (track_rect.tl().x + track_rect.br().x) /2;
    if(abs(x_center_rect - (int)(img_size.width/2)) < img_size.width * CENTER_RANGE_PERC ) {
        return true;
    }else {
        return false;
    }
}

TrackResult track_it(vector<Point2f> & points_prev, vector<Point2f> & points_now, Mat & frame,  Rect & track_rect, cv::VideoCapture & cap, Mat &prevGray,deque<int> &tracked_points_ids,  bool default_step_mode, std::function<bool(Rect&, Size)> check_if_arrrived) {
    char order;
    cv::Mat gray;
    vector<uchar> status;
    vector<float> err;
    bool if_arrived = false;
    bool step_mode = default_step_mode;
    bool go_next_frame = false;
    size_t i, k;
    deque<int> new_tracked_points_ids;
    while(false == if_arrived) {
        /* #4 show image*/
        cv::rectangle(frame, track_rect.tl(), track_rect.br(), Scalar(255, 0, 0), 2);
        imshow("track", frame);
        go_next_frame = false;
        while(step_mode && go_next_frame == false) {
            order = waitKey(1);
            switch(order) {
                case 's': //step mode.
                    step_mode = true;
                    break;
                case 'g': // continual mode.
                    step_mode = false;
                    break;
                case 'n': // next frame(in step mode).
                    go_next_frame = true;
                    break;
                default:
                    continue;
                    break;
            } 
        }
        /* #1 update frame*/
        cap >> frame;
        cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        /* #2 update tracked points*/
        calcOpticalFlowPyrLK(prevGray, gray, points_prev, points_now, status, err, winSize, 3, termcrit, 0, 0.001);
        size_t x_min = frame.cols, x_max = 0, y_min = frame.rows, y_max = 0;
        cout << "points_now.size(): "<< points_now.size() << endl;
        size_t j = 0;
        int id = 0;
/*        for(j=0;j< tracked_points_ids.size(); ++j) {*/
           //id = tracked_points_ids[j];
           //if( !status[id] ) {
               //continue;
           //}else {
               //new_tracked_points_ids.push_back(id);
           //}
        //}
        /*size_t good_id = 0;*/
        for( i = k = 0; i < points_now.size(); i++ )  {
            if( !status[i] ) {
                if(i == tracked_points_ids[0]) {
                    tracked_points_ids.pop_front();
                }
                continue;
            }

/*            if(good_id < new_tracked_points_ids.size() && i == new_tracked_points_ids[good_id]) {*/
                //new_tracked_points_ids[good_id] = k;
                //++ good_id;
                //circle(frame, points_now[k], 5, Scalar(0,255,255), -1, 8);
            /*}*/
            if(i == tracked_points_ids[0]) {
                new_tracked_points_ids.push_back(k);
                tracked_points_ids.pop_front();
            }
            points_now[k++] = points_now[i];
            //if(points_now[i].x < x_min) {
                //x_min = points_now[i].x;
            //}else if (points_now[i].x > x_max) {
                //x_max = points_now[i].x;
            //}
            //if(points_now[i].y < y_min) {
                //y_min = points_now[i].y;
            //} else if (points_now[i].y > y_max) {
                //y_max = points_now[i].y;
            //}
            circle( frame, points_now[i], 3, Scalar(0,255,0), -1, 8);
        }
        for(j=0;j<new_tracked_points_ids.size();++j){
            id = new_tracked_points_ids[j];
            circle(frame, points_now[id], 5, Scalar(0,255,255), -1, 8);
            if(points_now[id].x < x_min) {
                x_min = points_now[id].x;
            }else if (points_now[id].x > x_max) {
                x_max = points_now[id].x;
            }
            if(points_now[id].y < y_min) {
                y_min = points_now[id].y;
            } else if (points_now[id].y > y_max) {
                y_max = points_now[id].y;
            }
        }

        imshow("track", frame);
        waitKey(5);
        cout << "tracked points" << new_tracked_points_ids.size()<<endl;
        points_now.resize(k);
        
        /* #3 update tracked rect*/
        track_rect.x = x_min;
        track_rect.y = y_min;
        track_rect.width = x_max - x_min;
        track_rect.height = y_max - y_min;

        /* #5 check if arrived*/
        if_arrived = check_if_arrrived(track_rect, Size(frame.cols, frame.rows));
        /* #6 update previous points of interst*/
        std::swap(points_now, points_prev);
        std::swap(new_tracked_points_ids, tracked_points_ids);
        new_tracked_points_ids.clear();
        //cv::swap(prevGray, gray);
        gray.copyTo(prevGray);
    }
    if(true == if_arrived) {
        cout << "arriaved" << endl;
        return TrackResult::arrived_dest;
    }
}


