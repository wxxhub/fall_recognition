#include <stdio.h>

#include "fall_recognition/bind_id.h"

using namespace std;

using namespace fall_recognition;

BindID::BindID(int num)
    : if_match_id_(false)
{
    id_pools_.resize(num);

    for (int i = 0; i < id_pools_.size(); ++i)
    {
        BindTarget *taget = new BindTarget();

        taget->empty = true;
        taget->origin_id = INIT_ID;
        taget->residual_capacity = capacity_;
        taget->rectangle.up_x = 0;
        taget->rectangle.up_y = 0;
        taget->rectangle.down_x = 0;
        taget->rectangle.down_y = 0;

        id_pools_[i] = taget;
    }
}

BindID::~BindID()
{
    for (int i = 0; i < id_pools_.size(); ++i)
    {
        delete id_pools_[i];
    }
}

void BindID::add(Rectangle rectangle, int origin_id)
{
    if_match_id_ = false;

    BindTarget new_target;
    new_target.rectangle = rectangle;
    new_target.origin_id = origin_id;

    tagets_.push_back(new_target);
}

void BindID::add(const int up_x, const int up_y, const int down_x, const int down_y, const int origin_id)
{
    if_match_id_ = false;

    BindTarget new_target;
    new_target.rectangle.up_x = up_x;
    new_target.rectangle.up_y = up_y;
    new_target.rectangle.down_x = down_x;
    new_target.rectangle.down_y = down_y;
    new_target.origin_id = origin_id;

    tagets_.push_back(new_target);
}

void BindID::match()
{
    if_match_id_ = true;

    for (int i = 0; i < id_pools_.size(); ++i)
    {
        id_pools_[i]->origin_id = INIT_ID;
    }

    list<BindTarget>::iterator iter = tagets_.begin();

    while (iter != tagets_.end())
    {
        int flag = -1;
        list<BindTarget>::iterator erease_iter = iter;
        for (int i = 0; i < id_pools_.size(); ++i)
        {

            if (id_pools_[i]->empty)
            {
                flag = -1;
                continue;
            }

            if (!ifIntersection(id_pools_[i]->rectangle, iter->rectangle) && id_pools_[i]->empty)
            {
                flag = -1;
                continue;
            }

            const float area_error0 = areaError(id_pools_[i]->rectangle, iter->rectangle);
            const float area_error1 = areaError(iter->rectangle, id_pools_[i]->rectangle);

            // 如果相交情况符合， 则将目标进行面积判断， 否则丢进空的目标池。
            if ((area_error0 > must_area_error_ && area_error1 > min_area_error_) ||
                 (area_error1 > must_area_error_ && area_error0 > filter_area_error_))
            {
                // 如果id池中已经有目标， 比较大小， 没有目标则丢进id池
                if (id_pools_[i]->origin_id != INIT_ID && id_pools_[i]->origin_id != CLEANR_ID)
                {
                    int origion_area = (id_pools_[i]->rectangle.down_x - id_pools_[i]->rectangle.up_x)
                                       *(id_pools_[i]->rectangle.down_y - id_pools_[i]->rectangle.up_y);
                    
                    int present_area = (iter->rectangle.down_x - iter->rectangle.up_x)
                                       *(iter->rectangle.down_y - iter->rectangle.up_y);
                    
                    // 比较面积大小， 如果当前面积比之前大， 则更新。
                    if (origion_area < present_area)
                    {
                        setIdPool(i, iter);
                    }

                }
                else
                {
                    setIdPool(i, iter);
                }
                
                id_pools_[i]->residual_capacity = capacity_;
                id_pools_[i]->empty = false;

                flag = 0;

                // 放进id池后对下个目标判断
                break;
            }
        }  

        iter++;

        // 如果已经放入id池，则将目标从tagets_中擦除。
        if (flag == 0)
        {
            tagets_.erase(erease_iter);
        }
        
    }   

    // 如果还有没丢进id池的， 将剩余的丢进空池
    if (tagets_.size() > 0)
    {
        for (int i = 0; i < id_pools_.size(); ++i)
        {
            if (id_pools_[i]->empty)
            {
                list<BindTarget>::iterator iter = tagets_.begin();
                
                setIdPool(i, iter);
                id_pools_[i]->residual_capacity = capacity_;
                id_pools_[i]->empty = false;

                tagets_.erase(iter);

                if (tagets_.size() <= 0)
                    break;
            }
        }
    }

    tagets_.clear();
}

int BindID::getID(int origion_id)
{
    for (int i = 0; i < id_pools_.size(); ++i)
    {
        if (!id_pools_[i]->empty)
        {
            if (origion_id == id_pools_[i]->origin_id)
                return i;
        }
    }

    return -1;
}

int BindID::getResult(int pool_index, int& up_x, int& up_y, int& down_x, int& down_y)
{
    if (!if_match_id_)
        match();

    if (id_pools_[pool_index]->origin_id == CLEANR_ID)
        return CLEANR_ID;

    if (id_pools_[pool_index]->empty)
        return NO_MATCH_ID;

    if (id_pools_[pool_index]->origin_id == INIT_ID)
        return INIT_ID;
    
    up_x = id_pools_[pool_index]->rectangle.up_x;
    up_y = id_pools_[pool_index]->rectangle.up_y;
    down_x = id_pools_[pool_index]->rectangle.down_x;
    down_y = id_pools_[pool_index]->rectangle.down_y;

    return MATCH_ID;
}

bool BindID::ifIntersection(Rectangle rectangle, Rectangle judge_rectangle)
{
    if (rectangle.down_x < judge_rectangle.up_x||
        rectangle.down_y < judge_rectangle.up_y||
        judge_rectangle.down_x < rectangle.up_x||
        judge_rectangle.down_y < rectangle.up_y)
    {
        return false;   
    }
    else
    {
        return true;
    }
}

float BindID::areaError(Rectangle rectangle, Rectangle judge_rectangle)
{
    int up_x = rectangle.up_x > judge_rectangle.up_x ? rectangle.up_x : judge_rectangle.up_x;
    int up_y = rectangle.up_y > judge_rectangle.up_y ? rectangle.up_y : judge_rectangle.up_y;
    int down_x = rectangle.down_x < judge_rectangle.down_x ? rectangle.down_x : judge_rectangle.down_x;
    int down_y = rectangle.down_y < judge_rectangle.down_y ? rectangle.down_y : judge_rectangle.down_y;

    float result = 1.0 * (down_x - up_x) * (down_y - up_y) / ((rectangle.down_x - rectangle.up_x) * (rectangle.down_y - rectangle.up_y));
    return result;
}

void BindID::setIdPool(int index,  list<BindTarget>::iterator iter)
{
    id_pools_[index]->origin_id = iter->origin_id;
    id_pools_[index]->rectangle.up_x   = iter->rectangle.up_x;
    id_pools_[index]->rectangle.up_y   = iter->rectangle.up_y;
    id_pools_[index]->rectangle.down_x = iter->rectangle.down_x;
    id_pools_[index]->rectangle.down_y = iter->rectangle.down_y; 
}

void BindID::checkClean()
{
    for (int i = 0; i < id_pools_.size(); ++i)
    {
        if (id_pools_[i]->origin_id == INIT_ID)
        {
            id_pools_[i]->residual_capacity--;

            if (id_pools_[i]->residual_capacity <= 0)
                cleanPool(i);
        }
    }
}

void BindID::cleanPool(int index)
{
    id_pools_[index]->origin_id = CLEANR_ID;
    id_pools_[index]->rectangle.up_x = 0;
    id_pools_[index]->rectangle.up_y = 0;
    id_pools_[index]->rectangle.down_x = 0;
    id_pools_[index]->rectangle.down_y = 0;
    id_pools_[index]->residual_capacity = 0;
    id_pools_[index]->empty = true;
}