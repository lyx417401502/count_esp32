#ifndef DATA_HPP
#define DATA_HPP

#include<Arduino.h>
#include "Can.hpp"
#include "Master.hpp"
#include<freertos/queue.h>

class Data
{
private:
    String m_Rece_Dat;
    
    int     m_stringLength;
public:
    static String  m_inputString;
    static boolean m_stringComplete;    // 数据标志位

     static String  m_outputString;




    Data();
    ~Data();

    void Data_Dispose();

    unsigned char encodeTwoDigits(unsigned char digit1, unsigned char digit2) {
    return ((digit1 & 0x0F) << 4) | (digit2 & 0x0F);
}

//编码函数,低四位第一个数字，高四位第二个数字，合并成一个字符
unsigned char encodeCharPair(const char *pair);



//解码函数
void decodeByteToCharPair(unsigned char encodedByte, char *pair)  ;
    

};





#endif // DATA_HPP