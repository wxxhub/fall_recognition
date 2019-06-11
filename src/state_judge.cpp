#include <stdio.h>

#include "fall_recognition/state_judge.h"

using namespace fall_recognition;

StateJudge::StateJudge()
    : state_(NORMAL),
      judge_size_(20),
      judge_times_(0),
      n_average_x_(210),
      x_data_(665)
{

}

StateJudge::StateJudge(int judge_size)
{
    state_ = NORMAL;
    judge_size_ = judge_size;
    judge_times_ = 0;

    float averager_x = (1 + judge_size_) / 2.0;
    float n_square_average_x = judge_size_ * averager_x * averager_x;

    n_average_x_ =  averager_x * judge_size_;

    // 1*1+2*2+3*3+...+n*n = n * (n+1) * (2n+1) / 6
    // x_data_ = E(xi) - n_square_average_x
    x_data_ = judge_size_ * (judge_size_ + 1) * (2 * judge_size_ + 1) / 6.0 - n_square_average_x;
}

StateJudge::~StateJudge()
{
}

void StateJudge::setFPS(int fps)
{

}

int StateJudge::getResult(int up_x, int up_y, int down_x, int down_y, int img_h, bool show_result)
{
    ++judge_times_;

    // update list
    if (judge_times_ <= judge_size_)
    {
        up_sites_.push_back(up_x);
        down_sites_.push_back(up_y);
        vertical_sites_distance_.push_back(down_y - up_y);
        horizontal_sites_distance_.push_back(down_x - up_x);
        return NORMAL;
    }
    else
    {
        up_sites_.pop_front();
        down_sites_.pop_front();
        vertical_sites_distance_.pop_front();
        horizontal_sites_distance_.pop_front();

        up_sites_.push_back(up_x);
        down_sites_.push_back(up_y);
        vertical_sites_distance_.push_back(down_y - up_y);
        horizontal_sites_distance_.push_back(down_x - up_x);
    }
    
    const float up_slope = fittingSlope(up_sites_);
    const float veritial_slop = fittingSlope(vertical_sites_distance_);

    if (show_result)
    {
        printf("up_slope: %f\n", up_slope);
        printf("veritial_slop: %f\n", veritial_slop);
        printf("\n");
    }

    // judge current state
    int current_state = NORMAL;
    if (up_slope >= FALL_DOWN_SLOP && veritial_slop <= FALL_DOWN_SLOP)
    {
        current_state = FALL_DOWN;
    }
    else if (up_slope <= STAND_UP_SLOP)
    {
        current_state = STAND_UP;
    }

    // judge state
    if (current_state == FALL_DOWN)
        state_ = FALL_DOWN;
    else if (current_state == STAND_UP)
        state_ = NORMAL;

    state_ = judgeFallByTime(state_);

    return state_;
}

void StateJudge::clean()
{
    state_ = NORMAL;
    up_sites_.clear();
    down_sites_.clear();
    vertical_sites_distance_.clear();
    horizontal_sites_distance_.clear();
}

float StateJudge::fittingSlope(std::list<int> data_list)
{
    int sum_y = 0;
    int sum_xy = 0;
    double average_y;

    std::list<int>::iterator iter = data_list.begin();
    
    for (int i = 1; iter != data_list.end(); iter++)
    {
        sum_y += *iter;
        sum_xy += *iter * i;
        i++;
    }

    average_y = 1.0 * sum_y / judge_size_;

    /*          n
    *           E(xi*yi) - n * average_x * average_y
    *           i = 1
    *   slop =  -------------------------------------
    *           n
    *           E(xi*xi) - n * average_x_ * average_x_
    *           i = 1
    */       

    float slope = (sum_xy - n_average_x_ * average_y)/x_data_;
    return slope;
}

int StateJudge::judgeFallByTime(int state)
{
    return state;
}