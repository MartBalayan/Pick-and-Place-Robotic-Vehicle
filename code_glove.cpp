#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>

#define but_pin           2
#define LED_R             22     
#define LED_G             23
#define LED_B             24     

#define flex_5            A0
#define flex_4            A1
#define flex_3            A2
#define flex_2            A3
#define flex_1            A4

#define TX_buf_size       4

#define Mot_avg_speed     125
#define IMU_min_angle     25
#define Roll_min_angle    25
#define Pitch_min_angle   10
#define Gesture_min_angle 30
#define Speed_diff_max    Mot_avg_speed/2
#define flex_limit_val    250

#define MODE_OFF          1
#define MODE_VEHICLE      2
#define MODE_ARM          3

int selected_mode = 0;

int speed_diff = 0;
uint8_t mot_speed = 0;

int but_val = 0, but_status = 0, but_pressed = 1;

int flex_val_1, flex_val_2, flex_val_3, flex_val_4, flex_val_5;
int dir_pitch, dir_roll, dir_thumb, dir_index, dir_middle, dir_ring, dir_pinkie;
int dir_S1, dir_S2, dir_S3, dir_S4, dir_S5, dir_S6;

float gyro_x, gyro_y, gyro_z;
float accel_x, accel_y, accel_z;
float mag_x, mag_y, mag_z;
double roll, pitch;

uint8_t ble_buffer[TX_buf_size];

char buffer[40];

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

void setup() {
  pinMode(but_pin, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(flex_1, INPUT);
  pinMode(flex_2, INPUT);
  pinMode(flex_3, INPUT);
  pinMode(flex_4, INPUT);
  pinMode(flex_5, INPUT);

  digitalWrite(LED_R, 1);
  digitalWrite(LED_G, 1);
  digitalWrite(LED_B, 1);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Nano 33 BLE (Central)"); 
  BLE.advertise();

  Serial.begin(9600);
}

void loop() {
  connectToPeripheral(); 
}

void ReadData()
{
  but_val = digitalRead(but_pin);
  flex_val_1 = analogRead(flex_1);
  flex_val_2 = analogRead(flex_2);
  flex_val_3 = analogRead(flex_3);
  flex_val_4 = analogRead(flex_4);
  flex_val_5 = analogRead(flex_5);
}

void ModeButton()
{
  //Pressed first time and then released
  if (!but_val && !but_status && but_pressed % 2 == 1)
  {
      but_status = 1;
      but_pressed++;
  }
  if (but_val && but_status && but_pressed % 2 == 1)
  {
      but_status = 0;
  }

  //Pressed second time and then released
  if (!but_val && !but_status && but_pressed % 2 == 0)
  {
      but_status = 1;
      but_pressed++;
  }
  if (but_val && but_status && but_pressed % 2 == 0)
  {
      but_status = 0;
  }

  if(but_pressed % 3 == 0)
  {
    RGB_LED_Write(0,0,1);
    selected_mode = MODE_ARM;
  }
  else if (but_pressed % 3 == 1)
  {
    RGB_LED_Write(0,1,0);
    selected_mode = MODE_VEHICLE;
  }
  else if (but_pressed % 3 == 2)
  {
    RGB_LED_Write(1,0,0);
    selected_mode = MODE_OFF;
  }
  ble_buffer[0] = selected_mode;
}

void RGB_LED_Write(int R, int G, int B)
{
  digitalWrite(LED_R, !R);
  digitalWrite(LED_G, !G);
  digitalWrite(LED_B, !B);
}

void connectToPeripheral()
{
  BLEDevice peripheral;
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();

    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
  if (!gestureCharacteristic) {
    Serial.println("* Peripheral device does not have gesture_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canWrite()) {
    Serial.println("* Peripheral does not have a writable gesture_type characteristic!");
    peripheral.disconnect();
    return;
  }
  
  while (peripheral.connected()) {
    // loop while connected
    ReadData();
    ModeButton();
    if (selected_mode == MODE_VEHICLE)
    {
      MovementDetection();
      gestureCharacteristic.writeValue(ble_buffer, TX_buf_size);
    }
    else if(selected_mode == MODE_ARM)
    {
      GestureDetection();
      gestureCharacteristic.writeValue(ble_buffer, TX_buf_size);
    }
    else if(selected_mode == MODE_OFF)
    {
      ble_buffer[1] = 0;
      ble_buffer[2] = 0;
      ble_buffer[3] = 0;
      gestureCharacteristic.writeValue(ble_buffer, TX_buf_size);
    }
  }
  RGB_LED_Write(0, 0, 0);
  Serial.println("- Peripheral device disconnected!");
}

void GestureDetection()
{
  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accel_x, accel_y, accel_z);

    pitch = atan2((- accel_x) , sqrt(accel_y * accel_y + accel_z * accel_z)) * 57.3;
    roll = atan2f(accel_y, accel_z)*57.3;

    if (flex_val_2 > flex_limit_val && flex_val_3 <= flex_limit_val & flex_val_4 <= flex_limit_val && flex_val_5 <= flex_limit_val)
    {
      if      (roll > Gesture_min_angle)      {dir_S2 = 1;}
      else if (roll < -Gesture_min_angle)     {dir_S2 = 2;}
      else                                    {dir_S2 = 3;}

      if      (pitch > Gesture_min_angle)     {dir_S1 = 1;}
      else if (pitch < -Gesture_min_angle)    {dir_S1 = 2;}
      else                                    {dir_S1 = 3;}
    }
    else if (flex_val_2 <= flex_limit_val && flex_val_3 > flex_limit_val && flex_val_4 <= flex_limit_val && flex_val_5 <= flex_limit_val)
    {
      if      (roll > Gesture_min_angle)      {dir_S3 = 1;}
      else if (roll < -Gesture_min_angle)     {dir_S3 = 2;}
      else                                    {dir_S3 = 3;}

      if      (pitch > Gesture_min_angle)     {dir_S4 = 1;}
      else if (pitch < -Gesture_min_angle)    {dir_S4 = 2;}
      else                                    {dir_S4 = 3;}
    }
    else if (flex_val_2 <= flex_limit_val && flex_val_3 <= flex_limit_val && flex_val_4 > flex_limit_val && flex_val_5 <= flex_limit_val)
    {
      if      (roll > Gesture_min_angle)      {dir_S5 = 1;}
      else if (roll < -Gesture_min_angle)     {dir_S5 = 2;}
      else                                    {dir_S5 = 3;}
    }
    else if (flex_val_2 <= flex_limit_val && flex_val_3 <= flex_limit_val && flex_val_4 <= flex_limit_val && flex_val_5 > flex_limit_val)
    {
      if      (roll > Gesture_min_angle)     {dir_S6 = 1;}
      else if (roll < -Gesture_min_angle)    {dir_S6 = 2;}
      else                                   {dir_S6 = 3;}
    }
    else
    {
      dir_S1 = 3;
      dir_S2 = 3;
      dir_S3 = 3;
      dir_S4 = 3;
      dir_S5 = 3;
      dir_S6 = 3;
    }

    ble_buffer[1] = dir_S2 * 10 + dir_S1;
    ble_buffer[2] = dir_S3 * 10 + dir_S4;
    ble_buffer[3] = dir_S5 * 10 + dir_S6;
  }
}

void MovementDetection()
{
  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accel_x, accel_y, accel_z);

    pitch = atan2((- accel_x) , sqrt(accel_y * accel_y + accel_z * accel_z)) * 57.3;
    roll = atan2f(accel_y, accel_z)*57.3;

    if (abs(pitch) <= IMU_min_angle && abs(roll) <= IMU_min_angle)
    {
      ble_buffer[1] = 0;
      ble_buffer[2] = 0;
    }
    else if (abs(roll) > IMU_min_angle)
    {
      if (roll > IMU_min_angle)
      {
        ble_buffer[3] = 1;
      }
      else if (roll < -IMU_min_angle)
      {
        ble_buffer[3] = 2;
      }  
      
      speed_diff = map(abs(pitch), 0, 90, 0, Speed_diff_max);
      mot_speed = map(abs(roll), 0, 90, 60, 255);

      if(abs(pitch) <= Pitch_min_angle)
      {
        ble_buffer[1] = mot_speed;
        ble_buffer[2] = mot_speed;
      }
      else if (pitch > Pitch_min_angle)
      {
        ble_buffer[1] = mot_speed+speed_diff;
        ble_buffer[2] = mot_speed-speed_diff;
      }
      else if (pitch < -Pitch_min_angle)
      {
        ble_buffer[1] = mot_speed-speed_diff;
        ble_buffer[2] = mot_speed+speed_diff;
      }
    }
    else if(abs(pitch) > IMU_min_angle)
    {
      if (pitch < -IMU_min_angle)
      {
        ble_buffer[3] = 3;
      }
      else if (pitch > IMU_min_angle)
      {
        ble_buffer[3] = 4;
      } 
      ble_buffer[1] = Mot_avg_speed;
      ble_buffer[2] = Mot_avg_speed;
    }
    else
    {
      ble_buffer[1] = 0;
      ble_buffer[2] = 0;
    }
  }
}

void MovementStop()
{
  ble_buffer[1] = 0;
  ble_buffer[2] = 0;
}