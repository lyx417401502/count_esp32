#include "Can.hpp"

CanFrame Can::m_rxFrame={0};
CanFrame Can::m_txFrame={0};




Can::Can(uint32_t addr,int rxQueSize=5,int txQueSize=5,uint16_t speed=500)
{
 
    this->m_txFrame.identifier = addr; 
    this->m_txFrame.extd = 0;
    this->m_txFrame.data_length_code = 8;

        // Set pins
	ESP32Can.setPins(CAN_TX, CAN_RX);

    ESP32Can.setRxQueueSize(rxQueSize);
	ESP32Can.setTxQueueSize(txQueSize);

    ESP32Can.setSpeed(ESP32Can.convertSpeed(speed));
    if(ESP32Can.begin()) {
        Serial.println("CAN bus started!");
    } else {
        Serial.println("CAN bus failed!");
    }

}



void Can::sendtxFrame(int len, String data) {
    for(int i=0;i<len;i++)
    {
        Can::m_txFrame.data[i] = data[i];
    }
    ESP32Can.writeFrame(Can::m_txFrame,5);  // timeout defaults to 1 ms
     vTaskDelay(100 / portTICK_PERIOD_MS ); 
}


void Can::showObdFrame(CanFrame frame) {
    int len=sizeof(frame.data)/sizeof(frame.data[0]);
    char buffer[len+1];
    for(int i = 0; i < frame.data_length_code; i++) {
        // Serial.printf("data:%02X ", frame.data[i]);
        buffer[i] = frame.data[i];
        buffer[i+1]='\0';
    }
            
    Serial.print(buffer);
    
    Serial.println();
}