#ifndef CAN_HPP  
#define CAN_HPP  
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#define CAN_TX		5
#define CAN_RX		4

class Can
{
public:

    static CanFrame m_rxFrame;
    static CanFrame m_txFrame;

    void sendtxFrame(int len, String data);
    void showObdFrame(CanFrame frame);

    Can(uint32_t addr,int rxQueSize,int txQueSize,uint16_t speed);
    
};




#endif //CAN_HPP