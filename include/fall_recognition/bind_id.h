/*
 * bind_id.h
 *
 *  Created on: 2019. 6. 11
 *      Author: wxx
 *      E-mail: wxx0520@foxmail.com
 */

#ifndef _BIND_ID_
#define _BIND_ID_

#include <vector>
#include <list>

namespace fall_recognition
{

const int MATCH_ID    = 0;
const int INIT_ID     = -1;
const int CLEANR_ID   = -2;
const int NO_MATCH_ID = -2;

struct Rectangle
{
    int up_x;
    int up_y;
    int down_x;
    int down_y;
};

struct BindTarget
{
    bool empty;
    int origin_id;
    int residual_capacity;
    Rectangle rectangle;
};

class BindID
{
public:
    BindID(int num);
    ~BindID();

    void add(Rectangle rectangle, int origin_id);
    void add(const int up_x, const int up_y, const int down_x, const int down_y, const int origin_id);
    void match();

    int getID(int origion_id);
    int getResult(int pool_index, int& up_x, int& up_y, int& down_x, int& down_y);

    bool ifIntersection(Rectangle rectangle, Rectangle judge_rectangle);
    float areaError(Rectangle rectangle, Rectangle judge_rectangle);

private:
    bool if_match_id_;

    std::vector<BindTarget*> id_pools_;
    std::list<BindTarget> tagets_;

    void setIdPool(int index,  std::list<BindTarget>::iterator iter);
    void cleanPool();
    void checkClean();
    void cleanPool(int index);

    const int capacity_ = 20;

    // area_error
    const float filter_area_error_ = 0.4; // 避免出现突然变小的面积带走ID
    const float min_area_error_ = 0.2;
    const float must_area_error_ = 0.6;
};

}

#endif /* _BIND_ID_ */