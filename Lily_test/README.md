# 아두이노 릴리패드 블루투스 시리얼 통신 테스트

```cpp
#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출

// Constants
const int TOUCH_BUTTON_PIN = 9;  // Input pin for touch state
const int LED_PIN = 8;          // Pin number for LED

// Global Variables
int buttonState = 0;             // Variable for reading button
int count = 0;
char state_print = '0';

// 시리얼 통신 핀 설정
int blueTx=12;   //Tx (보내는핀 설정)at
int blueRx=11;   //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
 

void setup() {

  // Configure button pin as input 
  pinMode(TOUCH_BUTTON_PIN, INPUT);

  // Configure LED pin as output
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);   //시리얼모니터
  mySerial.begin(9600); //블루투스 시리얼
}

void loop() {

  // Read the state of the capacitive touch board
  buttonState = digitalRead(TOUCH_BUTTON_PIN);
  
  // If a touch is detected, turn on the LED
  if (buttonState == HIGH) {
    digitalWrite(LED_PIN, HIGH);
    state_print = '1';
  } else {
    digitalWrite(LED_PIN, LOW);
    state_print = '0';
  }

  if (mySerial.available()) {       
    Serial.write(mySerial.read());  //블루투스측 내용을 시리얼모니터에 출력
  }
  if (Serial.available()) {         
    mySerial.write(Serial.read());  //시리얼 모니터 내용을 블루추스 측에 WRITE
  }

  if (count % 50 == 0) {
    mySerial.write(state_print);
    if (count >= 500) {
      mySerial.write('\n');
      count = 0;
    }
  }
  count++;
  delay(10);
}
```
