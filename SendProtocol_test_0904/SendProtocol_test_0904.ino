#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출

// Constants
const int LED_PIN = 6;          // Pin number for LED

int count = 0;
int sensorval = 0;
int sensorval2 = 0;
int brightness = 0;
int resizedval = 0;
int resizedval2 = 0;
int i = 0;
char PRSarr[16];

// 시리얼 통신 핀 설정
int blueTx = 13; //Tx (보내는핀 설정)
int blueRx = 12; //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언

char getDigit(int val, int pos) { // pos : 0, 1, 2
  switch (pos){
    case 0:
      return (char) (val / 100) + '0'; 
    case 1:
      return (char) (val / 10 % 10) + '0';
    case 2:
      return (char) (val % 10) + '0'; 
  }
}



void setup() {

  // Configure LED pin as output
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);   //시리얼모니터
  mySerial.begin(9600); //블루투스 시리얼
}

void loop() {

  //아날로그 1번 핀에 압력센서를 연결합니다.
  sensorval = analogRead(A1);
  sensorval2 = analogRead(A2);

  // 0부터 1023의 센서값을 PWM 값 범위(0-255)로 변환 합니다.
  resizedval = map(sensorval, 0, 1023, 0, 999);
  resizedval2 = map(sensorval2, 0, 1023, 0, 999);

  PRSarr[0] = '#';  PRSarr[7] = '@';
  for (i=1; i<=3; i++) PRSarr[i] = getDigit(resizedval, i-1);
  for (i=4; i<=6; i++) PRSarr[i] = getDigit(resizedval2, i-4);

  
  //시리얼 모니터를 통해 센서값과 LED의 밝기를 표기합니다. 
  Serial.print("Sensorvalue : ");
  for (i=0; i<8; i++) Serial.print(PRSarr[i]);
  Serial.println();

  // If a touch is detected, turn on the LED

//  if (mySerial.available()) {
//    Serial.write(mySerial.read());  //블루투스측 내용을 시리얼모니터에 출력
//  }
//  if (Serial.available()) {
//    mySerial.write(Serial.read());  //시리얼 모니터 내용을 블루추스 측에 WRITE
//  }

  for(i=0; i<8; i++)
    mySerial.write(PRSarr[i]);
  mySerial.write('\n');

  count++;

  if ( flag == 0 ) delay(1000);
  else delay(30000);
}
