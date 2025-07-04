#include <TWEN32F0xx.h>
#include "HardwareSerial.h"

String msg = "";

int main(void)
{
  TW32F0xx_init();
  Serial1.begin(9600);
  pinMode(PB0, GPIO_Mode_Out_PP);
  digitalWrite(PB0, 0);
  while(1){
    if(Serial1.available() > 0){
      msg = Serial1.readString();
      if(msg == "LED_ON"){
        digitalWrite(PB0, 1);
      }
      else if(msg == "LED_OFF"){
        digitalWrite(PB0, 0);
      }
    }
  }
  return 1;
}
