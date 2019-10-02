#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
#include <Wire.h> //자이로센서 I2C 통신
#include <TaskScheduler.h>  //병렬처리 라이브러리 호출
#define ARR_LEN 16
#define GYRO1 0x68
#define GYRO2 0x69

// 블루투스 통신용 전역변수
const int LED_PIN = 6;          // Pin number for LED

int count = 0;
int sensorval = 0;
int sensorval2 = 0;
int brightness = 0;
int resizedval = 0;
int resizedval2 = 0;
int i = 0;
char arr[ARR_LEN];

// 각도 계산용 전역변수
int gyro_x, gyro_y, gyro_z;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
long gyro_x_cal_1, gyro_y_cal_1, gyro_z_cal_1;
long gyro_x_cal_2, gyro_y_cal_2, gyro_z_cal_2;
boolean set_gyro_angles;

long acc_x, acc_y, acc_z, acc_total_vector;
float angle_roll_acc, angle_pitch_acc;

float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
float angle_pitch_output, angle_roll_output;
int angle_1, angle_2;

long loop_timer;
int temp;


// 시리얼 통신 핀 설정
int blueTx = 13; //Tx (보내는핀 설정)
int blueRx = 12; //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언

// 콜백 메소드 원형
void callback_calculate_angle();
void callback_main_loop();

// 태스크 객체 생성자 호출
Task main_task(1000, TASK_FOREVER, &callback_main_loop);
Task angle_task(4, TASK_FOREVER, &callback_calculate_angle);

// 태스크 스케줄러 객체 생성
Scheduler scheduler;

// 콜백 메소드 정의
void callback_calculate_angle() {
  calculate_angle(GYRO1);
  calculate_angle(GYRO2);
}

void callback_main_loop(){
  // 시작문자, 종료문자 설정
  arr[0] = '#';  arr[ARR_LEN] = '@';

  // 아날로그 1번 핀에 압력센서를 연결합니다.
  sensorval = analogRead(A1);
  sensorval2 = analogRead(A2);

  // 0부터 1023의 센서값을 PWM 값 범위(0-255)로 변환 합니다.
  resizedval = map(sensorval, 0, 1023, 0, 999);
  resizedval2 = map(sensorval2, 0, 1023, 0, 999);

  // 압력센서1, 입력센서2의 값을 세개의 문자로 저장
  for (i=1; i<=3; i++) arr[i] = getDigit(resizedval, i-1);
  for (i=4; i<=6; i++) arr[i] = getDigit(resizedval2, i-4);


  /* ========================================================= */

  
  // 기울기 계산
  calculate_angle(GYRO1);
  angle_1 = round(angle_pitch_output);
    
  calculate_angle(GYRO2);
  angle_2 = round(angle_pitch_output);

  // 기울기센서1, 기울기센서2의 값을 세개의 문자로 저장
  arr[7] = (angle_1 >= 0) ? '+' : '-';
  arr[11] = (angle_2 >= 0) ? '+' : '-';
  for (i=8; i<=10; i++) arr[i] = getDigit(abs(angle_1), i-8);
  for (i=12; i<=14; i++) arr[i] = getDigit(abs(angle_2), i-12);
    
  // 시리얼 모니터를 통해 송신값을 확인합니다.
  Serial.print("Sensorvalue : ");
  for (i=0; i<ARR_LEN; i++) Serial.print(arr[i]);
  Serial.println();

  // 블루투스 시리얼로 송신합니다.
  for(i=0; i<ARR_LEN; i++) mySerial.print(arr[i]);
  mySerial.println();

  count++;
}




void setup() {
  Serial.begin(9600);   //시리얼모니터
  mySerial.begin(9600); //블루투스 시리얼

  wire_setup(GYRO1);
  gyro_x_cal_1 = gyro_x_cal;
  gyro_y_cal_1 = gyro_y_cal;
  gyro_z_cal_1 = gyro_z_cal;
  
  wire_setup(GYRO2);
  gyro_x_cal_2 = gyro_x_cal;
  gyro_y_cal_2 = gyro_y_cal;
  gyro_z_cal_2 = gyro_z_cal;

  scheduler.init();
  Serial.println("Initialized scheduler");

  scheduler.addTask(angle_task);
  Serial.println("added angle_task");
  
  scheduler.addTask(main_task);
  Serial.println("added main_task");
  
  angle_task.enable();
  Serial.println("Enabled angle_task");

  main_task.enable();
  Serial.println("Enabled main_task");
}


void loop() {
  scheduler.execute();
}



// 세자리 정수로부터 해당하는 자리 pos의 수의 문자 반환
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


void setup_mpu_6050_registers(int adr){
  //Activate the MPU-6050
  Wire.beginTransmission(adr);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                             
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(adr);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                             
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(adr);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                             
}


void read_mpu_6050_data(int adr){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(adr);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(adr,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  
  acc_y = Wire.read()<<8|Wire.read();                                  
  acc_z = Wire.read()<<8|Wire.read();                                  
  temp = Wire.read()<<8|Wire.read();                                   
  gyro_x = Wire.read()<<8|Wire.read();                                 
  gyro_y = Wire.read()<<8|Wire.read();                                 
  gyro_z = Wire.read()<<8|Wire.read();                                 
}


void wire_setup(int adr){
  Wire.begin(adr);                                                        //Start I2C as master
  setup_mpu_6050_registers(adr);                                          //Setup the registers of the MPU-6050 
  for (int cal_int = 0; cal_int < 1000 ; cal_int ++){                  //Read the raw acc and gyro data from the MPU-6050 for 1000 times
    read_mpu_6050_data(adr);                                             
    gyro_x_cal += gyro_x;                                              //Add the gyro x offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z offset to the gyro_z_cal variable
    delay(3);                                                          //Delay 3us to have 250Hz for-loop
  }

  // divide by 1000 to get avarage offset
  gyro_x_cal /= 1000;                                                 
  gyro_y_cal /= 1000;                                                 
  gyro_z_cal /= 1000;    
}

void calculate_angle(int adr){
  read_mpu_6050_data(adr);   
  //Subtract the offset values from the raw gyro values
  if (adr == GYRO1) {
    gyro_x -= gyro_x_cal_1;                                                
    gyro_y -= gyro_y_cal_1;                                                
    gyro_z -= gyro_z_cal_1;
  }
  else if (adr == GYRO2) {
    gyro_x -= gyro_x_cal_2;                                                
    gyro_y -= gyro_y_cal_2;                                                
    gyro_z -= gyro_z_cal_2;
  }
             
  //Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
  
  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll

  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  Serial.print(" | Angle ");
  Serial.print(String((adr == GYRO1)? 1 : 2));
  Serial.print(" = "); 
  Serial.println(angle_pitch_output);
}
