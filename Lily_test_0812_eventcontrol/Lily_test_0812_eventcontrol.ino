#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
#include <TaskScheduler.h>  //콜백메소드를 통한 이벤트 처리 라이브러리 호출
#include <ButtonEvent.h>



// 콜백 메소드 원형
void t1Callback();

// 태스크 객체 생성자 호출
Task t1(100, TASK_FOREVER, &t1Callback);

// 태스크 스케줄러 객체 생성
Scheduler scheduler;


// 콜백 메소드 정의
void t1Callback() {
  ButtonEvent.loop(); 
}

void setup() {
  ButtonEvent.addButton(9,       //button pin
                        onDown,   //onDown event function
                        onUp,     //onUp event function
                        onHold,   //onHold event function
                        2000,     //hold time in milliseconds
                        onDouble, //double event function
                        100);     //double time interval

  
  ButtonEvent.addButton(7,       //button pin
                        onDown,   //onDown event function
                        onUp,     //onUp event function
                        onHold,   //onHold event function
                        2000,     //hold time in milliseconds
                        onDouble, //double event function
                        100);     //double time interval

  Serial.begin(9600);

  scheduler.init();
  Serial.println("Initialized scheduler");

  scheduler.addTask(t1);
  Serial.println("added t1");
  
  t1.enable();
  Serial.println("Enabled t1");
}

void loop() {
  scheduler.execute();
}

void onDown(ButtonInformation* Sender) {
  Serial.print("Button (pin:");
  Serial.print(Sender->pin);
  Serial.println(") down!");
}

void onUp(ButtonInformation* Sender) {
  Serial.print("Button (pin:");
  Serial.print(Sender->pin);
  Serial.println(") up!");
}

void onHold(ButtonInformation* Sender) {
  Serial.print("Button (pin:");
  Serial.print(Sender->pin);
  Serial.print(") hold for ");
  Serial.print(Sender->holdMillis);
  Serial.println("ms!");
}

void onDouble(ButtonInformation* Sender) {
  Serial.print("Button (pin:");
  Serial.print(Sender->pin);
  Serial.print(") double click in ");
  Serial.print(Sender->doubleMillis);
  Serial.println("ms!");
}
