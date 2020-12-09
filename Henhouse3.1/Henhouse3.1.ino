  
#include <stdio.h>
#include <string.h>
#include <DS1302.h>
#include <DFRobot_LiquidCrystal_I2C.h>
#include <DFRobot_URM10.h>
#include "DHT.h"  
#define DHTPIN 8 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
uint8_t RST_PIN   = 5;
uint8_t DAT_PIN   = 6;
uint8_t SCLK_PIN = 7;
int ledPin = 13; 
double temp,data;
DFRobot_URM10 urm10(11,12);
/* 日期变量缓存 */
char buf[50];
char buf1[50];
char buf2[50];
char day[10];
/* 串口数据缓存 */
String comdata = "";    //用于保存串口的输入数据
int numdata[7] ={0}, j = 0, mark = 0;
int val;

float shidu;
float wendu;

char dataa = 'a';

DS1302 rtc(RST_PIN, DAT_PIN, SCLK_PIN);  // 创建 DS1302 对象 rtc

DFRobot_LiquidCrystal_I2C lcd1602(0x3F, 16, 2);
//########################################print_time###############################################
void print_time(){
  Time t = rtc.time();        //创建一个Time对象 t ，赋值为DS1302对象中的time（）函数，以获取当前时间
  memset(day, 0, sizeof(day));//将星期数字对应转换 
  switch (t.day){
    case 1:
      strcpy(day, "Sun"); 
    break;
    case 2: 
      strcpy(day, "Mon"); 
    break;
    case 3: 
      strcpy(day, "Tues");
    break;
    case 4: 
      strcpy(day, "Wed"); 
    break;
    case 5: 
      strcpy(day, "Thur"); 
    break;
    case 6: 
      strcpy(day, "Fri"); 
    break;
    case 7: 
      strcpy(day, "Sat");
    break;
  }
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  snprintf(buf1, sizeof(buf1), "%04d-%02d-%02d %02d:%02d",t.yr, t.mon, t.date, t.hr, t.min);
  snprintf(buf2, sizeof(buf2), "%s   %02doC   %02d",day, int(wendu), int(shidu));
  Serial.println(buf); //串口打印日期
  lcd1602.setCursor(0, 1-1);
  lcd1602.printLine(buf1);
  lcd1602.setCursor(0, 2-1);
  lcd1602.printLine(buf2);
  lcd1602.setCursor(15,2);
  lcd1602.printLine("%");

 
  val=analogRead(0); 
  //灯光
  if (((600>val>0)&&(t.hr>=6)&&(18>=t.hr))||((35>(urm10.getDistanceCM()))&&((t.hr<6)||(t.hr>18))))  
  {
    digitalWrite(ledPin,HIGH);
  }
  else
  {
    digitalWrite(ledPin,LOW);
  } 
 
  //清粪便
  if(((t.hr==7)&&(t.min==0)&&(t.sec<=5))||((t.hr==19)&&(t.min==0)&&(t.sec<=5)))
  {
    digitalWrite(4, LOW);
  }
  else
  {
    digitalWrite(4, HIGH);
  }

  //喂食
  if(((t.hr==8)&&(t.min==0)&&(t.sec<=5))||((t.hr==20)&&(t.min==0)&&(t.sec<=5))||((t.hr==12)&&(t.min==0)&&(t.sec<=5)))
  {
    digitalWrite(10, LOW);
  }
  else
  {
    digitalWrite(10, HIGH);
  }
  
}

//########################################print_time2###############################################
void print_time2(){
  Time t = rtc.time();        //创建一个Time对象 t ，赋值为DS1302对象中的time（）函数，以获取当前时间
  memset(day, 0, sizeof(day));//将星期数字对应转换 
  switch (t.day){
    case 1:
      strcpy(day, "Sun"); 
    break;
    case 2: 
      strcpy(day, "Mon"); 
    break;
    case 3: 
      strcpy(day, "Tues");
    break;
    case 4: 
      strcpy(day, "Wed"); 
    break;
    case 5: 
      strcpy(day, "Thur"); 
    break;
    case 6: 
      strcpy(day, "Fri"); 
    break;
    case 7: 
      strcpy(day, "Sat");
    break;
  }
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  snprintf(buf1, sizeof(buf1), "%04d-%02d-%02d %02d:%02d",t.yr, t.mon, t.date, t.hr, t.min);
  snprintf(buf2, sizeof(buf2), "%s   %02doC   %02d",day, int(wendu), int(shidu));
  Serial.println(buf); //串口打印日期
  lcd1602.setCursor(0, 1-1);
  lcd1602.printLine(buf1);
  lcd1602.setCursor(0, 2-1);
  lcd1602.printLine(buf2);
  lcd1602.setCursor(15,2);
  lcd1602.printLine("%");

}
//########################################setup###############################################
void setup() {
  Serial.begin(9600);       //串口波特率
  rtc.write_protect(false);  //关闭写保护
  rtc.halt(false);           //开启时钟
  lcd1602.init();
  lcd1602.backlight();
  pinMode(ledPin, OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(A2,OUTPUT);
  dht.begin();
}

//########################################changedate###############################################
void changedate(){
  while (Serial.available() > 0){  //当串口有数据输入时，将数据拼接保存到变量comdata
    comdata += char(Serial.read());
    delay(2);
    mark = 1;                      //输入标志置1，表示有数据输入        
   }
   if(mark == 1){   //以逗号分隔分解comdata的字符串，分解结果变成转换成数字到numdata[]数组
     Serial.print("You inputed : ");
     Serial.println(comdata);
     for(int i = 0; i < comdata.length() ; i++){
       if(comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13){
         j++;
        }
        else{
          numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
        }
      }  
      Time t(numdata[0], numdata[1], numdata[2], numdata[3], numdata[4], numdata[5], numdata[6]); //将转换好的numdata转换为时间格式，写入DS1302
      rtc.time(t);
      mark = 0;j=0;
      comdata = String("");   //清空 comdata 变量，以便等待下一次输入
      for(int i = 0; i < 7 ; i++) numdata[i]=0;  //清空 numdata
    }  
    print_time();   
  }

//########################################loop###############################################
void loop(){
  dataa = 'a';
  changedate();
  temp=(long)analogRead(1); //控制水位
  data=(temp/650)*4;
  if(data==0){
    digitalWrite(A2, LOW);
   }
  if(data>=4){
    digitalWrite(A2, HIGH);
  }

  shidu = dht.readHumidity();
  wendu = dht.readTemperature();
  if(int(wendu) >= 26)   //温度控制风扇
  {
    digitalWrite(9, LOW);
   }
  else{
    digitalWrite(9, HIGH);
   }
   
//蓝牙LED
  if(Serial.available() > 0) 
  {
    dataa = Serial.read(); 
    Serial.print(dataa); 
    Serial.print("\n"); 
    if(dataa == '0')  //进入蓝牙控制
    {
      dataa = 'a';
      digitalWrite(13,LOW);
      digitalWrite(9, HIGH);
      digitalWrite(10, HIGH);
      digitalWrite(A2, HIGH);
      digitalWrite(4, HIGH);
      while(1)
      {
         shidu = dht.readHumidity();
         wendu = dht.readTemperature();
         print_time2();

         dataa = Serial.read(); //控制灯光 
         if(dataa == '2')  
         {
           dataa = 'a';
           digitalWrite(13,!digitalRead(13)); 
         }

         if(dataa == '3')  //控制风扇
         {
           dataa = 'a';
           digitalWrite(9,!digitalRead(9));
         }
         
         if(dataa == '4')  //控制清粪
         {
            dataa = 'a';
            digitalWrite(4,!digitalRead(4));
         }
         
         if(dataa == '5')  //控制水位
         {
           dataa = 'a';
           digitalWrite(A2,!digitalRead(A2));
         }

         if(dataa == '6')  //控制喂食
         {
           dataa = 'a';
           digitalWrite(10,!digitalRead(10));
         }
         
         if(dataa == '0')  //退出蓝牙控制
         {
            dataa = 'a';
            break;
         }
      }  
    }
  }
}
