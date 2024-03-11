#include <math.h>
#include <Wire.h>

//INA219用設定
int16_t BusVoltage, ShuntVoltage;
//INA219用設定終わり

//103AT-2用設定・関数
#define THERMISTOR_PIN A0 // nano every A0ピン
//#define THERMISTOR_PIN A0 //A0ピン uno

const float DIVIDER = 10000.0; // 分圧抵抗10kΩ
const float T0 = 298.15;  //25℃時の華氏 273.15+25
const float R0 = 10000.0; //25℃時の抵抗値,103AT-2
const float B = 3435.0;   //B定数、25℃時に計測

/*float calcTemp(float Rt) { 
  float T = (1/(1/T0 + 1/B * log(Rt/R0)))-273.15;
  return T;
}*/

float calcTemp(float Aout) { //103AT-2での温度計測用関数

  //Serial.println(Aout);
  if (Aout == NULL){ 
    Serial.println("Failed reading Aout");
    return;
  }

  float Rt = DIVIDER * Aout / (1024.0 - Aout); //103ATの抵抗値計算　
  float T = (1/(1/T0 + 1/B * log(Rt/R0)))-273.15;

  return T;
}
//103AT-2用終わり 

int16_t BusVoltageF() { //Get BusVoltage用関数
  Wire.beginTransmission(0x40);                 //デバイスアドレス
  Wire.write(0x02);                             //BusVoltageレジスタ
  Wire.endTransmission();
  Wire.requestFrom(0x40, 16);
  while (Wire.available() < 16);
  BusVoltage = Wire.read() << 8 | Wire.read();

  return BusVoltage = (BusVoltage >> 3) * 4;
} 

int16_t ShuntVoltageF(){ //Get ShuntVoltage用関数
  Wire.beginTransmission(0x40);                 //デバイスアドレス
  Wire.write(0x01);                             //ShuntVoltageレジスタ
  Wire.endTransmission();
  Wire.requestFrom(0x40, 16);
  while (Wire.available() < 16);

  return ShuntVoltage = Wire.read() << 8 | Wire.read();
}

void setup() {
  //103AT-2用 初期設定
  Serial.begin(9600);
  pinMode(THERMISTOR_PIN, INPUT);

  //INA219用　初期設定
  //Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  int16_t config_value = 0x399F;           //デフォルト
  Wire.beginTransmission(0x40);            //デバイスアドレス
  Wire.write(0x00);                        //configアドレス
  Wire.write((config_value >> 8) & 0xFF);
  Wire.write(config_value & 0xFF);
  Wire.endTransmission();
  //INA219用設定終わり

}


void loop() { 
  float Aout = float(analogRead(THERMISTOR_PIN)); 
  float temp = calcTemp(Aout);  //103AT-2での計測結果

  int16_t BusVoltage = BusVoltageF(); 
  int16_t ShuntVoltage = ShuntVoltageF(); //INA219での計測結果

  //Serial.print("BusVoltage");
  Serial.print(BusVoltage * 0.001);
  //Serial.print("V");
  Serial.print(",");
  //Serial.print("ShuntVoltage");
  Serial.print(ShuntVoltage * 0.01);
  //Serial.print("mV");
  Serial.print(",");

  //ラズパイへのシリアル通信・計測結果送信
  Serial.print(Aout);
  Serial.print(",");
  Serial.println(temp);
  delay(1000);

}

