#include <string.h>
#include <ESP32-TWAI-CAN.hpp>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>

#include<freertos/semphr.h>

#include"Slave.hpp"
#include"Data.hpp"
#include"Can.hpp"
#include "main.hpp"

#include <U8g2lib.h>
#include <U8x8lib.h>
#include <OneButton.h>

#include <Arduino.h>
#include "OTA.hpp"

#define Myself "01"

String myself="01";



#define Green_button_pin 25
#define Blue_button_pin 26

#define Green_LED_PIN  33
#define Red_LED_PIN  32

#define BLEDA_PIN 35


OneButton Green_button(Green_button_pin, true);
OneButton Blue_button(Blue_button_pin, true);



bool Start_count=false;
bool Reset_count=false;

bool using_success=false;
bool target_display=false;

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0,14,12,13,27); 


#define LED_BUILTIN 2

void taskOne( void * parameter );
void taskTwo( void * parameter );

Can *can=nullptr;
Data *data=nullptr;
Master *master=nullptr;
Slave *slave=nullptr;
OTA *ota=nullptr;


String display_num="";
String displau_model="";



void setup() {
    pinMode(Green_button_pin, INPUT_PULLUP);
  pinMode(Blue_button_pin, INPUT_PULLUP);

 pinMode(Green_LED_PIN, OUTPUT); //设置管脚为输出模式
  pinMode(Red_LED_PIN, OUTPUT); //设置管脚为输出模式
  digitalWrite(Green_LED_PIN,LOW);
  digitalWrite(Red_LED_PIN,LOW);

  pinMode(BLEDA_PIN,OUTPUT);
  digitalWrite(BLEDA_PIN,HIGH);
  


  Green_button.reset();
  Blue_button.reset();


  Blue_button.attachLongPressStart([]{Serial.print("Blue_button attachLongPressStart\r\n");if(Start_count==false) Reset_count=true;});

  Green_button.attachClick([]{ Serial.print("green button clicked\r\n");Start_count=true;Reset_count=false;});
  Blue_button.attachClick([]{Serial.print("Blue_button clicked\r\n");Start_count=false;});

  //如果长时间Start_count是false或者using_success是false，通过定时判断状态，那么BLEDA_PIN是低电平，关闭背光



  

    Serial.begin(115200);
    Serial.print("receive\r\n");
    
    can=new Can(0x7Bf,5,5,1000);

    Serial.println("CAN init");
    data=new Data();
    master=new Master(can);
    slave=new Slave();

    u8g2.begin();
   u8g2.enableUTF8Print();
   u8g2.setFont(u8g2_font_wqy16_t_gb2312); 
  
 u8g2.clearBuffer();
 u8g2.firstPage();

    xTaskCreate(taskOne, "taskOne", 2048, NULL, 1, NULL); 
    xTaskCreate(taskTwo, "taskTwo", 4000, NULL, 2, NULL);

    ota=new OTA(myself,"12345689");

     ota->addLogData("debug...");


}

void taskOne( void * parameter ){
    while (1)
    { 
      if(Start_count==true)
      {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(Green_LED_PIN, HIGH);
        digitalWrite(Red_LED_PIN, LOW);
      }
        if(ESP32Can.readFrame(Can::m_rxFrame, 250))
         {
        //  Serial.printf("Received frame: %03X  \r\n", Can::m_rxFrame.identifier);
        if(Can::m_rxFrame.identifier == 0x7Bf  )
         {   // Standard OBD2 frame responce ID   
            slave->rece_Data(Can::m_rxFrame);
             Serial.print("m_NO:");
             Serial.print(slave->m_NO);
            

            if(strcmp(slave->m_NO, Myself) == 0)
            {
              target_display=true;
               if(strcmp(slave->m_comand, "stop") == 0) 
               {
                Start_count=false;
                using_success=false;
                goto out;
               }
              Start_count=true;
              using_success=true;
            }
            else  target_display=false;

          }

        }
        out:

      master->sendAlltoCan();
              
       data->Data_Dispose(); 
      
         
       vTaskDelay(100 / portTICK_PERIOD_MS ); 
      
    }
}

void taskTwo( void * parameter )
{
    while (1)
    {
      if(Start_count==false)
      {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(Green_LED_PIN, LOW);
        digitalWrite(Red_LED_PIN, HIGH);
      }
      if(using_success==false||Start_count==false)
      {
        if(Reset_count==false)
        {
            u8g2.clearBuffer();
            u8g2.firstPage();
            do {
             u8g2.setCursor(0,40);
             u8g2.printf("%s","该出料口暂停使用");
            } while ( u8g2.nextPage() );

        }



      }
      if(Start_count==false&&Reset_count==true&&using_success==true)
      {
            
            Data::m_outputString = "stop" + String(myself) + "000000";
            Data::m_stringComplete=true;

            memset(slave->m_comand,0,sizeof(slave->m_comand));
            memset(slave->m_num,0,sizeof(slave->m_num));
          

            display_num="000000";
            // displau_model="已清空计数";

            u8g2.clearBuffer();
            u8g2.firstPage();
            do {

             u8g2.setCursor(20,60);
             u8g2.printf("%s","已清空计数");
             
             u8g2.setCursor(0, 30);
             u8g2.printf("%s","数量：");
             
             u8g2.setCursor(50,30);
             u8g2.printf("%s",display_num);
            } while ( u8g2.nextPage() );

            Reset_count=false;
          
      }
      if(Start_count==true&&using_success==true&&target_display==true)
      {
            display_num=slave->m_num;
            displau_model=slave->m_comand;

            ota->addLogData((display_num+" "+displau_model).c_str());
            
         
            // Serial.print(display_num);
            u8g2.clearBuffer();
            u8g2.firstPage();
            do {
            u8g2.setCursor(0, 60);
            u8g2.printf("%s","模型：");

             u8g2.setCursor(50,60);
             u8g2.printf("%s",displau_model);
             
             u8g2.setCursor(0, 30);
             u8g2.printf("%s","数量：");
             
             u8g2.setCursor(50,30);
             u8g2.printf("%s",display_num);
            } while ( u8g2.nextPage() );
      
      }

      vTaskDelay(100 / portTICK_PERIOD_MS ); 
    }
}

void loop(){

  // server.handleClient();
  ota->server.handleClient();

  Green_button.tick();
  Blue_button.tick();
  vTaskDelay(60 / portTICK_PERIOD_MS);

}



// 串口接收中断
void serialEvent() {
  while (Serial.available()) 
  {
        char inChar  = (char)Serial.read();
        Data::m_inputString+= inChar; 
        
        
      if (inChar == '\n') 
      {
        int comIndex = Data::m_inputString.indexOf("com=");
        if (comIndex != -1) {
        comIndex += 4;
        int endOfLineIndex = Data::m_inputString.indexOf("\r", comIndex);
        if (endOfLineIndex != -1) {
      // 提取 "com=" 和 "\r\n" 之间的字符串
        Data::m_outputString = Data::m_inputString.substring(comIndex, endOfLineIndex);
        Data::m_inputString = "";  
        Data::m_stringComplete = true;
        }
       }
      }
      
    if(Data::m_inputString.length()>180)
    {
        Data::m_inputString="";

    }

  }
}


void app_main(void)
{
 

}

