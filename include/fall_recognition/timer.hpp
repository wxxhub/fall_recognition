#ifndef _TIMER_HPP_
#define _TIMER_HPP_
 
#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

class timer
{
public:
  timer() : m_begin(std::chrono::high_resolution_clock::now()) {}

  void reset() 
  { 
    m_begin = std::chrono::high_resolution_clock::now();
  }

  void stop() 
  { 
    m_end = std::chrono::high_resolution_clock::now();
  }


  double elapsed() 
  {
    return (std::chrono::duration_cast<std::chrono::duration<double>>(m_end - m_begin)).count()*1000;
  }

  double show() 
  {
    std::cout << "Time elapsed: " << elapsed() << " ms" << std::endl;
  }

  void sleep(double ms) 
  {
    std::this_thread::sleep_for(std::chrono::duration<double>(ms/1000));
  }

  double wait(double ms)
  {
    stop(); sleep(ms - elapsed()); stop();
  }
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
};

#endif //_TIMER_HPP_