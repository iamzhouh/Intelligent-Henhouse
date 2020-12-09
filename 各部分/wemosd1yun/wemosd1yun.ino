#include <stdlib.h>

#include <ESP8266WiFi.h>
#include <DHTesp.h>

#define _DEBUG  //设置是否调试，注释该行取消调试 

#ifdef _DEBUG   //判断是否处于调试模式
  #define dprint(x)   sprint(x)
  #define dprintln(x) sprintln(x)
#else
  #define dprint(x)
  #define dprintln(x)
#endif

/*
* 我觉得用#define可以节约内存，将Serial.x替换成sx写起来也方便，
* 函数名一律小写不用经常大小写切换更好
*/

//函数名缩写
#define pmod(a,b)   pinMode(a,b)
#define dout(a,b)   digitalWrite(a,b)
#define aout(a,b)   analogWrite(a,b)
#define din(x)      digitalRead(x)
#define ain(x)      analogRead(x)
#define sprint(x)   Serial.print(x)
#define sprintln(x) Serial.println(x) 

//OneNET平台设置,从OneNet开发者中心→设备列表→设备详情页面获取
#define DEV_ID      "571541226"                       //设备Id项
#define API_KEY     "CWTI3qiS=7FYnT1=fLFVhu=CluE="    //APIKey项
#define SER_ADDR    "api.heclouds.com"                //统一设置

//无线网络设置
#define NET_SSID    "Z"   //无线网络的名称
#define NET_PWD     "ZHOUOUOU" //无线网络密码
#define NET_LED     2           //这是WeMos板子中ESP8266模块上的LED灯引脚，共阳极，低电平亮

// 设置上传数据阀值，避免无效数据上传,DHT11可测温度范围0~50℃,湿度20~90%RH
#define TEMP_MAX    50  //温度
#define TEMP_MIN    0
#define HUMI_MAX    100  //湿度
#define HUMI_MIN    0

//DHT11引脚
#define DHT_PIN     D8

//全局变量
char    json_data[200];
char    packet[400];
char    json_len[4]; 
char    temp[6];
char    humi[6];
float   var;
DHTesp dht;

/*
* 此函数用于连接无线网络，连接过程中NET_LED闪烁
*/
void netconnect() {
  sprint("网络信息：\r\n"
        "\t名称:"NET_SSID"\r\n"
        "\t密码:"NET_PWD"\r\n"
        "连接中，请稍后");
  WiFi.begin(NET_SSID, NET_PWD);
  while(WiFi.status() != WL_CONNECTED)
  {
    sprint('.');
    dout(NET_LED, !din(NET_LED));   //逻辑取反，因返回值为有符号数，LOW(0)按位却反(~)所得结果为-1,不为HIGH(1)
    delay(500);
  }
  sprint("\r\n连接成功");
  dout(NET_LED, LOW);
}

/*
* 此函数判断无线网络状态，网络已连接NET_LED亮，未连接灭
*/
bool netstat() {
  if(WiFi.status() == WL_CONNECTED) {
    dout(NET_LED, LOW);
    return true;
  }
  else {
    dout(NET_LED, HIGH);
    return false;
  }
}

/*
* 此函数使用字符串拼接方式生成json数据
*/
void mk_json(char *id, char *value, char *id2, char *value2) {
  /*
   {"datastreams":[{"id":"TEMP","datapoints":[{"value":40}]},{"id":"HUMI","datapoints":[{"value":50}]}]}
   */
  memset(json_data, 0, 200);
  strcat(json_data, "{\"datastreams\":[");

  strcat(json_data, "{\"id\":\"");
  strcat(json_data, id);
  strcat(json_data, "\",\"datapoints\":[{\"value\":");
  strcat(json_data, value);
  strcat(json_data, "}]}");

  //欲增加一次上传数据项，复制此段，更改id2及value2为指定参数
  strcat(json_data, ",{\"id\":\"");
  strcat(json_data, id2); 
  strcat(json_data, "\",\"datapoints\":[{\"value\":");
  strcat(json_data, value2); 
  strcat(json_data, "}]}");

  strcat(json_data, "]}");

  itoa(strlen(json_data), json_len, 10);   //此句将json_data长度（strlen(json_data)）转换为十进制字符串形式储存在json_len中
}

  /*
   * 此函数使用字符串拼接方式生成http数据包，包含http数据包头及json数据
   */
void mk_packet() {
  /*
    POST /devices/DEV_ID/datapoints HTTP/1.1
    api-key:API_KEY
    Host:api.heclouds.com
    Content-Length:101

   */
  memset(packet, 0, 400);
  strcat(packet, "POST /devices/");
  strcat(packet, DEV_ID);
  strcat(packet, "/datapoints HTTP/1.1"); 
  strcat(packet, "\r\napi-key:");
  strcat(packet, API_KEY);
  strcat(packet, "\r\nHost:");
  strcat(packet, SER_ADDR);
  strcat(packet, "\r\nContent-Length:");
  strcat(packet, json_len);
  strcat(packet,"\r\n\r\n");

  /*
  POST /devices/DEV_ID/datapoints HTTP/1.1
  api-key:API_KEY
  Host:api.heclouds.com
  Content-Length:101

  {"datastreams":[{"id":"TEMP","datapoints":[{"value":40}]},{"id":"HUMI","datapoints":[{"value":50}]}]}
   */
  strcat(packet, json_data);
}

void setup() {
  Serial.begin(115200);
  pmod(NET_LED, OUTPUT);
  dout(NET_LED, HIGH);
  pmod(DHT_PIN, INPUT);
  dht.setup(DHT_PIN, DHTesp::DHT11);
  netconnect();
}

void loop() {
  WiFiClient wc;
  var = dht.getTemperature();
  if(var < TEMP_MIN || var > TEMP_MAX) {
    sprint("数据错误");
    return;   //部分编译器void类型函数不允许有返回值
  }
  dtostrf(var, 2, 1, temp);
  var = dht.getHumidity();
  if(var < HUMI_MIN || var > HUMI_MAX) {
    sprint("数据错误");
    return;
  }
  dtostrf(var, 3, 1, humi);  

  mk_json("TEMP", temp, "HUMI", humi);
  mk_packet();
  dprint(packet);

  if(wc.connect(SER_ADDR, 80))  //尝试连接SER_ADDR:80
    wc.print(packet);           //发送http数据包
  else 
    if(!netstat()) 
      netconnect();
    else 
      sprint("连接到远程主机失败");

  wc.stop();
  delay(2000);
  
  analogWrite(D10,atoi(temp));
  sprint("\n");
  sprint(atoi(temp));
  sprint("\n");
  sprint(analogRead(D10));
  sprint("\n");
//  analogWrite(D11,int(atoi(humi)));
//  sprint(analogRead(D11));
//  sprint("\n");
}
