/*
 * state_judge.h
 *
 *  Created on: 2019. 6. 11
 *      Author: wxx
 *      E-mail: wxx0520@foxmail.com
 */

#ifndef _STATE_JUDGE_
#define _STATE_JUDGE_

#include <list>

namespace fall_recognition
{

// state
const int FALL_DOWN  = -2;
const int FALL_ALARM = -1;
const int NORMAL     = 0;
const int STAND_UP   = 1;
const int NONE_STATE = 2;

// slope
const float STAND_UP_SLOPE   = -5;
const float FALL_DOWN_SLOPE  = 5;
const float HORIZONTAL_SLOPE = 1;

class StateJudge
{
public:
    StateJudge();
    StateJudge(int judge_size);
    ~StateJudge();

    void setFPS(int fps);
    void clean();
    void setSlopeRadio(float radio);
    void setWarnTime(int second);

    int getResult(int up_x, int up_y, int down_x, int down_y, bool show_result = false, int img_h = 680);

    float fittingSlope(std::list<int> data_list);
private:
    int state_;
    int judge_times_;
    int judge_size_;   

    int fall_frame_num_;
    int warn_frame_threshold_;

    int warn_time_;
    int fps_;

    std::list<int> up_sites_;
    std::list<int> down_sites_;
    std::list<int> vertical_sites_distance_;
    std::list<int> horizontal_sites_distance_;

    float n_average_x_;
    float x_data_;
    float slope_radio_;

    float up_slope_;
    float veritial_slop_;
    
    int judgeFallByTime(int state);
};

}
#endif /* _STATE_JUDGE_ */