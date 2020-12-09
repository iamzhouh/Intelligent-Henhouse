
#include <stdio.h>
#include <string.h>
#include <DS1302.h>
/* 接口定义
  RST(DS1302 pin5) -> Arduino D5
  DAT(DS1302 pin6) -> Arduino D6
  SCLK(DS1302 pin7) -> Arduino D7
*/
uint8_t RST_PIN   = 5;
uint8_t DAT_PIN   = 6;
uint8_t SCLK_PIN = 7;
/* 日期变量缓存 */
char buf[50];
char day[10];
/* 串口数据缓存 */
String comdata = "";    //用于保存串口的输入数据
int numdata[7] ={0}, j = 0, mark = 0;
DS1302 rtc(RST_PIN, DAT_PIN, SCLK_PIN);  // 创建 DS1302 对象 rtc
void print_time(){
  Time t = rtc.time();        //创建一个Time对象 t ，赋值为DS1302对象中的time（）函数，以获取当前时间
  memset(day, 0, sizeof(day));//将星期数字对应转换 
  switch (t.day){
    case 1:
      strcpy(day, "周日"); 
    break;
    case 2: 
      strcpy(day, "周一"); 
    break;
    case 3: 
      strcpy(day, "周二");
    break;
    case 4: 
      strcpy(day, "周三"); 
    break;
    case 5: 
      strcpy(day, "周四"); 
    break;
    case 6: 
      strcpy(day, "周五"); 
    break;
    case 7: 
      strcpy(day, "周六");
    break;
  }
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);//将日期格式化成buf等待
  Serial.println(buf); //串口打印日期
}
void setup(){
  Serial.begin(9600);       //串口波特率
  rtc.write_protect(false);  //关闭写保护
  rtc.halt(false);           //开启时钟
}
void loop(){
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
    print_time();  //打印当前时间
    delay(1000);
}
