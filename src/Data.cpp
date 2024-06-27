#include "Data.hpp"


String Data::m_inputString = "";
boolean Data::m_stringComplete = false;
String Data::m_outputString= "";
Data::Data()
{

}

Data::~Data()
{

}




void Data::Data_Dispose()
{
 
  if (Data::m_stringComplete) {
    Data::m_stringComplete = false;
 
    // Serial.printf("m_outputString:%s\r\n",Data::m_outputString.c_str());
    if(Data::m_outputString)
    {

      // Serial.printf("Data: %s",Data::m_outputString.c_str());
      // Data::m_outputString = Data::m_outputString.c_str();

      for (size_t i = 4; i < m_outputString.length() - 1; i += 2) {
            unsigned char encodedByte = encodeCharPair(Data::m_outputString.substring(i, i+2).c_str());
            Data::m_outputString[i/2 + 2] = encodedByte; 
        }


      
     if (xQueueSend(Master::message_queue, (void *)&Data::m_outputString, ( TickType_t ) 0) != pdTRUE) {
          Serial.print("Failed to enqueue message_queue. Queue full.");
        }
         Data::m_stringComplete = false;
         Data::m_outputString="";
    }


  }
}

unsigned char Data::encodeCharPair(const char *pair) {
    unsigned char firstDigit = pair[0] - '0';
    unsigned char secondDigit = pair[1] - '0';
    Serial.printf("%02X ", (firstDigit << 4) | secondDigit);
    return (firstDigit << 4) | secondDigit;
}


void Data::decodeByteToCharPair(unsigned char encodedByte, char *pair) {
    pair[0] = ((encodedByte >> 4) & 0x0F) + '0'; // 高4位转换为字符
    pair[1] = (encodedByte & 0x0F) + '0';        // 低4位转换为字符
    pair[2] = '\0'; // 添加字符串终止符
}

