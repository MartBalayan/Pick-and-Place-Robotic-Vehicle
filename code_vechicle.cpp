#include <ArduinoBLE.h>
#include <Servo.h>

#define PWM_MOT1      5
#define PWM_MOT2      6

#define MOT1_DIR1     2
#define MOT1_DIR2     4
#define MOT2_DIR1     7
#define MOT2_DIR2     8

#define PWM_SERVO1    A0
#define PWM_SERVO2    A1
#define PWM_SERVO3    A2
#define PWM_SERVO4    A3
#define PWM_SERVO5    A4
#define PWM_SERVO6    A5

#define RX_buf_size   4

#define MODE_OFF      1
#define MODE_VEHICLE  2
#define MODE_ARM      3

#define SERVO_SPEED_COEF    1
#define SERVO_SPEED_COEF_2  2

#define SERVO_1_MIN_ANGLE   0
#define SERVO_1_MAX_ANGLE   180
#define SERVO_1_INIT_ANGLE  90

#define SERVO_2_MIN_ANGLE   0
#define SERVO_2_MAX_ANGLE   180
#define SERVO_2_INIT_ANGLE  100

#define SERVO_3_MIN_ANGLE   0
#define SERVO_3_MAX_ANGLE   180
#define SERVO_3_INIT_ANGLE  150

#define SERVO_4_MIN_ANGLE   0
#define SERVO_4_MAX_ANGLE   180
#define SERVO_4_INIT_ANGLE  100

#define SERVO_5_MIN_ANGLE   0
#define SERVO_5_MAX_ANGLE   180
#define SERVO_5_INIT_ANGLE  85

#define SERVO_6_MIN_ANGLE   0
#define SERVO_6_MAX_ANGLE   90
#define SERVO_6_INIT_ANGLE  85

int selected_mode = 0;

Servo servo1, servo2, servo3, servo4, servo5, servo6;

int dir_S2, dir_S1, dir_thumb, dir_S3, dir_S4, dir_S5, dir_S6;

int servo_1_angle = SERVO_1_INIT_ANGLE, 
    servo_2_angle = SERVO_2_INIT_ANGLE, 
    servo_3_angle = SERVO_3_INIT_ANGLE, 
    servo_4_angle = SERVO_4_INIT_ANGLE, 
    servo_5_angle = SERVO_5_INIT_ANGLE, 
    servo_6_angle = SERVO_6_INIT_ANGLE;

uint8_t ble_buffer[RX_buf_size];

char buffer[40];

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService gestureService(deviceServiceUuid); 
BLECharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite, RX_buf_size, true);

void setup() {
  servo1.attach(PWM_SERVO1,500,2500);
  servo2.attach(PWM_SERVO2,500,2500);
  servo3.attach(PWM_SERVO3,500,2500);
  servo4.attach(PWM_SERVO4,500,2500);
  servo5.attach(PWM_SERVO5,500,2500);
  servo6.attach(PWM_SERVO6,500,2500);

  pinMode(PWM_MOT1, OUTPUT);  
  pinMode(PWM_MOT2, OUTPUT);
  pinMode(MOT1_DIR1, OUTPUT);
  pinMode(MOT1_DIR2, OUTPUT);
  pinMode(MOT2_DIR1, OUTPUT);
  pinMode(MOT2_DIR2, OUTPUT);
  
  servo_init();

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");

  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gestureCharacteristic);
  BLE.addService(gestureService);
  BLE.advertise();

  Serial.begin(9600);
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  // delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (gestureCharacteristic.written()) {
        gestureCharacteristic.readValue(ble_buffer, RX_buf_size);

        selected_mode = ble_buffer[0];

        if (selected_mode == MODE_VEHICLE)//2
        {
          motor_dir_speed(ble_buffer[1], ble_buffer[2], ble_buffer[3]);
        }
        else if(selected_mode == MODE_ARM)//3
        {
          dir_S2  = ble_buffer[1] / 10;
          dir_S1  = ble_buffer[1] % 10;
          dir_S3  = ble_buffer[2] / 10;
          dir_S4  = ble_buffer[2] % 10;
          dir_S5  = ble_buffer[3] / 10;
          dir_S6  = ble_buffer[3] % 10;

          servo_movement();

          motor_dir_speed(0, 0, 0);
        }
        else if (selected_mode == MODE_OFF)//1
        {
          motor_dir_speed(0, 0, 0);
        }
        else
        {
          motor_dir_speed(0, 0, 0);
        }

        
       }
    }
    
    Serial.println("* Disconnected to central device!");
  }
}


void servo_init()
{
  servo1.write(SERVO_1_INIT_ANGLE);
  servo2.write(SERVO_2_INIT_ANGLE);
  servo3.write(SERVO_3_INIT_ANGLE);
  servo4.write(SERVO_4_INIT_ANGLE);
  servo5.write(SERVO_5_INIT_ANGLE);
  servo6.write(SERVO_6_INIT_ANGLE);
}

void servo_movement()
{
  if      (servo_1_angle > SERVO_1_MAX_ANGLE) {servo_1_angle = SERVO_1_MAX_ANGLE;}
  else if (servo_1_angle < SERVO_1_MIN_ANGLE) {servo_1_angle = SERVO_1_MIN_ANGLE;}
  else
  {
    if        (dir_S1 == 2) {servo_1_angle+=SERVO_SPEED_COEF;}
    else if   (dir_S1 == 1) {servo_1_angle-=SERVO_SPEED_COEF;}
    else if   (dir_S1 == 3) {  }
  }

  if      (servo_2_angle > SERVO_2_MAX_ANGLE) {servo_2_angle = SERVO_2_MAX_ANGLE;}
  else if (servo_2_angle < SERVO_2_MIN_ANGLE) {servo_2_angle = SERVO_2_MIN_ANGLE;}
  else
  {
    if        (dir_S2 == 2) {servo_2_angle+=SERVO_SPEED_COEF;}
    else if   (dir_S2 == 1) {servo_2_angle-=SERVO_SPEED_COEF;}
    else if   (dir_S2 == 3) {  }
  }

  if      (servo_3_angle > SERVO_3_MAX_ANGLE) {servo_3_angle = SERVO_3_MAX_ANGLE;}
  else if (servo_3_angle < SERVO_3_MIN_ANGLE) {servo_3_angle = SERVO_3_MIN_ANGLE;}
  else
  {
    if        (dir_S3 == 1) {servo_3_angle+=SERVO_SPEED_COEF;}
    else if   (dir_S3 == 2) {servo_3_angle-=SERVO_SPEED_COEF;}
    else if   (dir_S3 == 3) {  }
  }
  
  if      (servo_4_angle > SERVO_4_MAX_ANGLE) {servo_4_angle = SERVO_4_MAX_ANGLE;}
  else if (servo_4_angle < SERVO_4_MIN_ANGLE) {servo_4_angle = SERVO_4_MIN_ANGLE;}
  else
  {
    if        (dir_S4 == 1) {servo_4_angle+=SERVO_SPEED_COEF_2;}
    else if   (dir_S4 == 2) {servo_4_angle-=SERVO_SPEED_COEF_2;}
    else if   (dir_S4 == 3) {  }
  }

  if      (servo_5_angle > SERVO_5_MAX_ANGLE) {servo_5_angle = SERVO_5_MAX_ANGLE;}
  else if (servo_5_angle < SERVO_5_MIN_ANGLE) {servo_5_angle = SERVO_5_MIN_ANGLE;}
  else
  {
    if        (dir_S5 == 2) {servo_5_angle+=SERVO_SPEED_COEF_2;}
    else if   (dir_S5 == 1) {servo_5_angle-=SERVO_SPEED_COEF_2;}
    else if   (dir_S5 == 3) {  }
  }

  if      (servo_6_angle > SERVO_6_MAX_ANGLE) {servo_6_angle = SERVO_6_MAX_ANGLE;}
  else if (servo_6_angle < SERVO_6_MIN_ANGLE) {servo_6_angle = SERVO_6_MIN_ANGLE;}
  else
  {
    if        (dir_S6 == 2) {servo_6_angle+=SERVO_SPEED_COEF_2;}
    else if   (dir_S6 == 1) {servo_6_angle-=SERVO_SPEED_COEF_2;}
    else if   (dir_S6 == 3) {  }
  }

  servo1.write(servo_1_angle);
  servo2.write(servo_2_angle);
  servo3.write(servo_3_angle);
  servo4.write(servo_4_angle);
  servo5.write(servo_5_angle);
  servo6.write(servo_6_angle);
}

void motor_dir_speed(int speed_1, int speed_2, int dir)
{
  analogWrite(PWM_MOT1, speed_1);
  analogWrite(PWM_MOT2, speed_2);
  if (dir == 1) 
  {
    digitalWrite(MOT1_DIR1, 0);
    digitalWrite(MOT1_DIR2, 1);

    digitalWrite(MOT2_DIR1, 0);
    digitalWrite(MOT2_DIR2, 1);   
  }
  else if (dir == 2)
  {
    digitalWrite(MOT1_DIR1, 1);
    digitalWrite(MOT1_DIR2, 0);

    digitalWrite(MOT2_DIR1, 1);
    digitalWrite(MOT2_DIR2, 0);  
  }
  else if (dir == 3)
  {
    digitalWrite(MOT1_DIR1, 1);
    digitalWrite(MOT1_DIR2, 0);

    digitalWrite(MOT2_DIR1, 0);
    digitalWrite(MOT2_DIR2, 1);
  }
  else if (dir == 4)
  {
    digitalWrite(MOT1_DIR1, 0);
    digitalWrite(MOT1_DIR2, 1);

    digitalWrite(MOT2_DIR1, 1);
    digitalWrite(MOT2_DIR2, 0);
  }
  else
  {
    digitalWrite(MOT1_DIR1, 0);
    digitalWrite(MOT1_DIR2, 0);

    digitalWrite(MOT2_DIR1, 0);
    digitalWrite(MOT2_DIR2, 0);
  }
}
