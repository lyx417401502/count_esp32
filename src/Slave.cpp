#include "Slave.hpp"


Slave::Slave(){}

Slave::~Slave() {}


void Slave::rece_Data(CanFrame frame) {
    int len=sizeof(frame.data)/sizeof(frame.data[0]);   
    for(int i = 0; i < frame.data_length_code; i++) {
         Serial.printf("data:%02X ", frame.data[i]);
        this->m_data[i] = frame.data[i];
    }
    

    saveAll(sizeof(m_data)/sizeof(m_data[0]));
    
}

void Slave::print_Data()
{
    Serial.printf("m_comand:%s\r\n",this->m_comand);
    Serial.printf("m_NO:%s\r\n",this->m_NO);
    Serial.printf("m_num:%s\r\n",this->m_num);
    Serial.printf("m_data:%s\r\n",this->m_data);
}

void Slave::saveAll(int len) {
    data=new Data();

    memcpy(m_comand, m_data, 4);
    m_comand[4] = '\0'; // 添加字符串终止符

    data->decodeByteToCharPair(m_data[4], m_NO);
    
    // 初始化m_num的索引
    int numIndex = 0;

    
    for (int i = 5; i < 8; ++i) {
         data->decodeByteToCharPair(m_data[i], &m_num[numIndex]);
        numIndex += 2; // 每次解码两个字节到m_num
    }
    


    // if (len >= 8) { // 确保len足够大，以便能够赋值给m_data数组
    //     for (int i = 0; i < len; i++) {
    //         if (i < 1) {
    //             this->m_comand[i] = this->m_data[i];
    //         } else if (i >= 1 && i < 3) {
    //             this->m_NO[i - 1] = this->m_data[i]; // 从m_data的第2个元素开始赋值
    //         }
    //     }
    //     // 为m_comand和m_NO添加字符串终止符
    //     this->m_comand[1] = '\0'; // m_comand只需要1个字符加上终止符
    //     this->m_NO[2] = '\0'; // m_NO需要2个字符加上终止符
    //     // m_num的赋值不需要在循环中处理，因为它们是从m_data的第4个元素开始的
    //     this->m_num[0] = this->m_data[3];
    //     this->m_num[1] = this->m_data[4];
    //     this->m_num[2] = this->m_data[5];
    //     this->m_num[3] = this->m_data[6];
    //     this->m_num[4] = this->m_data[7];
    //     this->m_num[5] = '\0'; // 为m_num添加字符串终止符
    // } else {
    //     // 如果len小于8，可以在这里处理错误情况
    //     Serial.println("Length is less than required.");
    // }
}