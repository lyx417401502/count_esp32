#include "OTA.hpp"
const char* updateIndex = R"DELIM(<!DOCTYPE html>
<html lang="zh">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>粤浆科技</title>
<style>
  body, h1 {
    margin: 0;
    padding: 0;
    color: #333;
  }
  body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    text-align: center;
    background-color: #f4f4f4;
    margin-top: 50px;
  }
  .container {
    display: inline-block;
    text-align: left;
  }
  h1 {
    font-size: 2.5em;
    color: #4a4a4a;
  }
  form {
    margin-top: 30px;
    background-color: #fff;
    padding: 30px;
    border-radius: 5px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
  }
  input[type=file], input[type=submit] {
    display: block;
    margin: 10px auto;
    padding: 10px;
    border: 1px solid #ddd;
    border-radius: 3px;
    width: 80%;
    box-sizing: border-box;
  }
  input[type=submit] {
    background-color: #5cb85c;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s ease;
  }
  input[type=submit]:hover {
    background-color: #4cae4c;
  }
</style>
</head>
<body>
<div class="container">
  <h1>粤浆设备无线升级模式</h1>
  <form method="POST" action="/update" enctype="multipart/form-data">
    <input type="file" name="update" accept=".bin">
    <input type="submit" value="上传固件">
  </form>
</div>
</body>
</html>)DELIM";

// void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//   switch(type) {
//     case WStype_DISCONNECTED:
//       Serial.printf("WebSocket client %u disconnected\n", num);
//       break;
//     case WStype_CONNECTED:
//       {
//         IPAddress ip = webSocket.remoteIP(num);
//         Serial.printf("WebSocket client %u connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
//       }
//       break;
//     case WStype_TEXT:
//       Serial.printf("WebSocket message from client %u: %s\n", num, payload);
//       break;
    
//   }
// }

String OTA::logData=" ";

IPAddress local_IP(192, 168, 4, 1); // 静态IP
IPAddress gateway(192, 168, 4, 1); // 网关IP

void OTA::handleRoot() {
  this->server.sendHeader("Location", "/update");
  server.send(302, "text/plain", "");
}

void OTA::addLogData(const char* data) {
  if(logData.length() > 1000) {
    logData = "";
  }
  //避免重复打印
  if(logData.indexOf(data) == -1) {
    //时间
    String time = String(millis());
    //将时间添加到日志数据中
    logData += time;
    logData += ": ";
    //将数据添加到日志数据中
    logData += data;
    logData += "\n";
    
  }

}

WebSocketsServer webSocket = WebSocketsServer(81);

OTA::OTA(const String ssid,const String password) {

  this->m_ssid=ssid;
  this->m_password=password;

  // 将 ESP32 设置为 AP 模式
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, IPAddress(255, 255, 255, 0));


  // 设置服务器处理函数


  server.on("/", HTTP_GET, [this]() {
        handleRoot(); // 使用this调用handleRoot成员函数
    });

  server.on("/update", HTTP_GET, [this]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", updateIndex);
  });

  server.on("/update", HTTP_POST, [this]() {
    server.sendHeader("Connection", "close");

    //动态显示结果
  String message = Update.hasError() ? "升级失败" : "升级成功，等待重新启动…";
  server.sendHeader("Content-Type", "text/html; charset=utf-8");
  server.send(200, "text/html", "<span style='font-size: 24px;'>" + message + "</span>");
    delay(1000);
    ESP.restart();
  }, [this]() {
    HTTPUpload& upload = server.upload(); //用于处理上传的文件数据
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // 以最大可用大小开始
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // 将接收到的数据写入Update对象
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { // 设置大小为当前大小
        Serial.printf("Update Success: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.on("/log", HTTP_GET, [this]() {
  server.send(200, "text/plain", logData.c_str());
});

  // webSocket.begin();
  // webSocket.onEvent(webSocketEvent);
  server.begin();

}

