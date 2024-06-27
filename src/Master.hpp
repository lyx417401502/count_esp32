#ifndef  MASTER_HPP
#define  MASTER_HPP

#include <Arduino.h>
#include<freertos/FreeRTOS.h>
#include<freertos/queue.h>
#include <Data.hpp>
#include "Can.hpp"


class Master {
  public:

    Can *m_can;   
    
    String command;

    String num;

   static QueueHandle_t message_queue;

   void sendAlltoCan();

   Master( Can *can);

  private:
    
};

#endif