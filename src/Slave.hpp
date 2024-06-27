#ifndef SLAVE_HPP
#define SLAVE_HPP

#include<Arduino.h>
#include"Can.hpp"
#include"Data.hpp"



//从机类

class Slave
{
private:


    


public:

    Data *data;

    char m_data[8];

    uint8_t m_addr;
    char m_comand[5];// aaa2
    char m_NO[3];// 01

    char m_num[7];// 999999
    Slave();
    ~Slave();

    void rece_Data(CanFrame frame);

    void print_Data();

    void saveAll(int len);




};






#endif//SLAVE_HPP