#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int DHTPIN = 2;      
const int DHTTYPE = DHT11; 

#define BUTTON D3 // Chân nối với nút nhấn onboard
#define LED D5 // Chân nối với đèn LED onboard

#define BUTTON2 D6 // Chân nối với nút nhấn onboard
#define MOTO D0 // Chân nối với đèn LED onboard

char auth[] = "jaHiwdjWTVcE3zAIoLBTxdBIK1GMrAZS";
char ssid[] = "iPhone của Duy Võ";
char pass[] = "duyvo123";

// What digital pin we're connected to
WidgetLED led1(V1);
WidgetLED led2(V2);
WidgetLED led3(V7);

boolean checkData = false;
int lastBtnStatus;
int pinValue;

boolean checkData2 = false;
int lastBtnStatus2;
int pinValue2;

DHT dht(DHTPIN, DHTTYPE);
int yl=0,value;;

BlynkTimer timer;

void sendSensor()
{
  int h = dht.readHumidity(); //Đọc độ ẩm
  int t = dht.readTemperature(); // Đọc nhiệt độ
  
  //Đọc độ ẩm đất
  for(int i=0;i<=9;i++){
    yl=yl+ analogRead(A0);
  }
  
  value=yl / 10;
  int percent = map(value, 350, 1023, 0, 100);
  percent=100-percent; // Tính giá trị phần trăm thực. Chuyển điện thế từ 3.3V ( khô ) thành 3.3V ( ẩm )
  Serial.print("Nhiệt độ:  ");
  Serial.println(t);
  Serial.print("Độ ẩm:  ");
  Serial.println(h);
  Serial.print("Độ ẩm đất:  ");
  Serial.print(percent);
  delay(200);
  yl=0;
   if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else{
    lcd.setCursor(3, 0);
    lcd.print("T"+String(t)+"oC");
    lcd.setCursor(3, 1);
    lcd.print("H"+String(h)+"%");
    lcd.setCursor(12, 0);
    lcd.print("M"+String(percent)+"%");
    
  
  Blynk.virtualWrite(V4, percent);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  }
}

void blinkLedWidget()
{
  if (led1.getValue()) {
    led1.off();
    Serial.println("LED on V1: off");
  } else {
    led1.on();
    Serial.println("LED on V1: on");
  }
}

BLYNK_WRITE(V0)
{
  pinValue = param.asInt();
  Serial.print("Tín hiệu đèn là: ");
  Serial.println(pinValue);
  //digitalWrite(D0,pinValue);
    Blynk.virtualWrite(V2,pinValue );//làm led trong app sáng khi bật button
  if (digitalRead(LED)) {
    led2.on();
    Serial.println("LED2 on V2: on");
  } else {
    led2.off();
    Serial.println("LED2 on V2: off");
  }
  checkData = true;
}


BLYNK_WRITE(V3)
{
  pinValue2 = param.asInt();
  Serial.print("Tín hiệu moto là: ");
  Serial.println(pinValue2);
  //digitalWrite(D6,pinValue2);
  
  Blynk.virtualWrite(V7,pinValue2 );//làm led trong app sáng khi bật button;
  if (digitalRead(MOTO)) {
    led3.off();
    Serial.println("LED3 on V7: off");
  } else {
    led3.on();
    Serial.println("LED3 on V7: on");
  }
  checkData2 = true;
}


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  dht.begin();

  pinMode(LED,OUTPUT); //Led chân D0
  pinMode(BUTTON,INPUT_PULLUP);//nút nhấn chân D3
  //digitalWrite(D0,HIGH);
  lastBtnStatus = digitalRead(BUTTON);
  pinMode(MOTO,OUTPUT);
  pinMode(BUTTON2,INPUT_PULLUP);
  lastBtnStatus2 = digitalRead(BUTTON2);
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, blinkLedWidget);
}

//ĐỒNG BỘ NÚT BẤM ĐÈN LED
void BUTTON_LED_ONBOARD(){
  if (checkData == true) {
digitalWrite(LED, pinValue);
checkData == false;
}
if (digitalRead(BUTTON) == 0)
{
delay(100);
if (digitalRead(BUTTON) != 0)
{
pinValue = !pinValue;
digitalWrite(LED, pinValue);
Blynk.virtualWrite(V0, pinValue);
Blynk.virtualWrite(V2, pinValue);
  if (digitalRead(D5)) {
    led2.off();
    //Serial.println("LED2 on V2: off");
  } else {
    led2.on();
    //Serial.println("LED2 on V2: on");
  }
Serial.println("In Here" + String(pinValue));
}}
}

void dongbomoto()
{
  if (checkData2 == true) {
digitalWrite(MOTO, pinValue2);
checkData2 == false;
}
if (digitalRead(BUTTON2) == 0)
{
delay(100);
if (digitalRead(BUTTON2) != 0)
{
pinValue2 = !pinValue2;
digitalWrite(MOTO, pinValue2);
Blynk.virtualWrite(V3, pinValue2);
Blynk.virtualWrite(V7, pinValue2);
  if (digitalRead(MOTO)) {
    led3.on();
    //Serial.println("LED3 on V7: on");
  } else {
    led3.off();
    //Serial.println("LED3 on V7: off");
  }
Serial.println("In Here" + String(pinValue2));
}}
  
}

void loop()
{
  Blynk.run();
  timer.run();

  lcd.begin();  
  lcd.backlight();
  lcd.clear();
  
  BUTTON_LED_ONBOARD();
  dongbomoto();
  //lcd.clear();
  
}
