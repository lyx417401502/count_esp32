#include "Master.hpp"

//
QueueHandle_t Master::message_queue=xQueueCreate(20, 20);


Master::Master(Can *can)
{
    this->m_can=can;
}


void Master:: sendAlltoCan()
{

    String sendbuffer="";
    
    
    if (xQueueReceive(Master::message_queue, &sendbuffer, ( TickType_t ) 0) == pdTRUE ) 
    {
       
        // digitalWrite(2, HIGH); 
        this->m_can->sendtxFrame(sendbuffer.length(), sendbuffer); 
        
    }
    
}