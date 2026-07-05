#define BLYNK_TEMPLATE_ID "TMPL6Oh-PQ5V8"
#define BLYNK_TEMPLATE_NAME "Greenhouse"
#define BLYNK_AUTH_TOKEN "5_BO8mHTnE-ClTzq_hm3pIfhWCAvj-Rq"
#define BOT_TOKEN "8679600659:AAFwfktK-G_PU2ViMrwt6GXbV4vBe26TWpA"
#define CHAT_ID "1124782539"
char ssid[] = "LAMEES";
char pass[] = "12345678";

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define FAN_IN1 32
#define FAN_IN2 33
#define PUMP_IN1 25
#define PUMP_IN2 26
#define FLAME_1_PIN 36
#define FLAME_2_PIN 39
#define SOIL_1_PIN 34
#define SOIL_2_PIN 35
#define DHT_PIN 23
#define DHT_TYPE DHT11
#define RED_PIN 19
#define GREEN_PIN 18
#define BLUE_PIN 5
#define SERVO_PIN 4

#define SERVO_RIGHT_ANGLE 170
#define SERVO_CENTER_ANGLE 90
#define SERVO_LEFT_ANGLE 10

#define FLAME_THRESHOLD 2000
#define SOIL_THRESHOLD 3500

#define FAN_TEMP_THRESHOLD 23
#define HUMIDITY_ALERT_THRESHOLD 45

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);
Servo myServo;
BlynkTimer timer;
WiFiClientSecure telegramClient;
UniversalTelegramBot bot(BOT_TOKEN, telegramClient);

bool pumbState = false;
bool fanState = false;
float temp = 0;
float humidity = 0;
int flame1 = 0;
int flame2 = 0;
bool isFlame1 = 0;
bool isFlame2 = 0;
int soil1 = 0;
int soil2 = 0;
bool needsWater1 = 0;
bool needsWater2 = 0;
int r = 0;
int g = 0;
int b = 0;
int servo_angle = 90;
int last_alert_type = 0;

void setup() {
  Serial.begin(9600);
  setupPins();
  setupLCD();
  setupDHT();
  setupServo();
  lcdPrint("System Ready", "");
  setupBlynk();
  setupTelegram();
  timer.setInterval(5000L, sendAllSensorsToBlynk);
  timer.setInterval(1000L, read_sesnors);
  timer.setInterval(1500L, handle_lcd);
  timer.setInterval(500L, handle_rgb);
  timer.setInterval(3200L, handle_servo);
  timer.setInterval(3200L, handle_pump);
  timer.setInterval(3000L, handle_fan);
  timer.setInterval(3000L, handle_alerts);
}

void loop() {
// test_lcd();
  // test_servo();
  // test_rgb();
//   test_DHT();
  // test_flame();
   //test_soil();
  //test_pumb();
  //test_fan();
  //test_blink();
  //test_telegram();
  //test_blynk_alert();
   main_code();
}

void setupPins() {
  pinMode(FAN_IN1, OUTPUT);
  pinMode(FAN_IN2, OUTPUT);
  pinMode(PUMP_IN1, OUTPUT);
  pinMode(PUMP_IN2, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  fanOFF();
  pumpOFF();
}

void setupLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void setupDHT() {
  dht.begin();
}

void setupServo() {
  myServo.attach(SERVO_PIN);
  delay(1000);
  moveServoToCenter();
}

void setupBlynk() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk Connected");
}

void setupTelegram() {
  telegramClient.setInsecure();
  Serial.println("Telegram Ready");
}

void fanON() {
  digitalWrite(FAN_IN1, HIGH);
  digitalWrite(FAN_IN2, LOW);
  fanState = true;
}

void fanOFF() {
  digitalWrite(FAN_IN1, LOW);
  digitalWrite(FAN_IN2, LOW);
  fanState = false;
}

void pumpON() {
  digitalWrite(PUMP_IN1, HIGH);
  digitalWrite(PUMP_IN2, LOW);
  pumbState = true;
}

void pumpOFF() {
  digitalWrite(PUMP_IN1, LOW);
  digitalWrite(PUMP_IN2, LOW);
  pumbState = false;
}

void readFlame1() {
  flame1 = analogRead(FLAME_1_PIN);
  isFlame1 = flame1 < FLAME_THRESHOLD;
}

void readFlame2() {
  flame2 = analogRead(FLAME_2_PIN);
  isFlame2 = flame2 < FLAME_THRESHOLD;
}

void readSoil1() {
  soil1 = analogRead(SOIL_1_PIN);
  needsWater1 = soil1 > SOIL_THRESHOLD;
}

void readSoil2() {
  soil2 = analogRead(SOIL_2_PIN);
  needsWater2 = soil2 > SOIL_THRESHOLD;
}

void read_DHT() {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
}

void setRGB(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
  r = red;
  g = green;
  b = blue;
}

void rgbOFF() {
  setRGB(0, 0, 0);
  r = 0;
  g = 0;
  b = 0;
}

void lcdPrint(String line1, String line2) {
  Serial.println("");
  Serial.println("-----------------------");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  Serial.println(line1);
  Serial.println(line2);
}

void moveServo(int angle) {
  myServo.write(angle);
  servo_angle = angle;
}

void moveServoToCenter() {
  moveServo(SERVO_CENTER_ANGLE);
}
void moveServoToLeft() {
  moveServo(SERVO_LEFT_ANGLE);
}
void moveServoToRight() {
  moveServo(SERVO_RIGHT_ANGLE);
}

void sendRGBStatus(String color) {
  Blynk.virtualWrite(V6, color);
}

void sendAlert(String message) {
  Blynk.logEvent("alert", message);
}

void sendTelegramMessage(String message) {
  bot.sendMessage(CHAT_ID, message, "");
}

void sendAllSensorsToBlynk() {
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, pumbState);
  Blynk.virtualWrite(V3, fanState);
  Blynk.virtualWrite(V4, isFlame1);
  Blynk.virtualWrite(V5, isFlame2);
  Blynk.virtualWrite(V6, needsWater1);
  Blynk.virtualWrite(V7, needsWater2);
}

void sendAllSensorsToBlynkTest() {
  int value = random(0, 2);
  Blynk.virtualWrite(V0, value);
  Blynk.virtualWrite(V1, value);
  Blynk.virtualWrite(V2, value);
  Blynk.virtualWrite(V3, value);
  Blynk.virtualWrite(V4, value);
  Blynk.virtualWrite(V5, value);
  Blynk.virtualWrite(V6, value);
  Blynk.virtualWrite(V7, value);
}

// Control Fan
BLYNK_WRITE(V10) {
  int value = param.asInt();
  if (value == 1) {
    fanON();
    Serial.println("fanON");
  } else {
    fanOFF();
    Serial.println("fanOFF");
  }
}

BLYNK_WRITE(V11) {
  int value = param.asInt();
  if (value == 1) {
    pumpON();
    Serial.println("pumpON");
  } else {
    pumpOFF();
    Serial.println("pumpOFF");
  }
}

void test_lcd() {
  while (1) {
    lcdPrint("LCD", "Working .. ");
    delay(2000);
  }
}

void test_servo() {
  while (1) {
    lcdPrint("TEST SERVO", "Center");
    moveServoToCenter();
    delay(3000);
    lcdPrint("TEST SERVO", "RIGHT");
    moveServoToRight();
    delay(3000);
    lcdPrint("TEST SERVO", "LEFT");
    moveServoToLeft();
    delay(3000);
  }
}

void test_rgb() {
  while (1) {
    lcdPrint("TEST RGB", "RED");
    setRGB(255, 0, 0);
    delay(3000);
    lcdPrint("TEST RGB", "GREEN");
    setRGB(0, 255, 0);
    delay(3000);
    lcdPrint("TEST RGB", "BLUE");
    setRGB(0, 0, 255);
    delay(3000);
  }
}

void test_flame() {
  while (1) {
    readFlame1();
    readFlame2();
    String val = String(flame1) + "  " + String(flame2);
    if (isFlame1 && isFlame2) {
      lcdPrint(val, "BOTH FIRE");
    } else if (isFlame1) {
      lcdPrint(val, "Fire 1");
    } else if (isFlame2) {
      lcdPrint(val, "Fire 2");
    } else {
      lcdPrint(val, "SAFE");
    }
    delay(2000);
  }
}

void test_DHT() {
  while (1) {
    read_DHT();
    String line1 = "temp : " + String(temp);
    String line2 = "humidity : " + String(humidity);
    lcdPrint(line1, line2);
    delay(2000);
  }
}

void test_soil() {
  while (1) {
    readSoil1();
    readSoil2();
    String val = String(soil1) + "  " + String(soil2);
    if (needsWater1 && needsWater2) {
      lcdPrint(val, "BOTH NEEDS WATER");
    } else if (needsWater1) {
      lcdPrint(val, "water 1");
    } else if (needsWater2) {
      lcdPrint(val, "water 2");
    } else {
      lcdPrint(val, "no need water");
    }
    delay(2000);
  }
}

void test_pumb() {
  while (1) {
    lcdPrint("PUMB", "ON");
    pumpON();
    delay(2000);
    lcdPrint("PUMB", "OFF");
    pumpOFF();
    delay(6000);
  }
}

void test_fan() {
  while (1) {
    lcdPrint("FAN", "ON");
    fanON();
    delay(1000);
    lcdPrint("FAN", "OFF");
    fanOFF();
    delay(10000);
  }
}

void test_blink() {
  timer.setInterval(3000L, sendAllSensorsToBlynkTest);
  while (1) {
    Blynk.run();
    timer.run();
    reconnectBlynk();
  }
}

void test_telegram() {
  while (1) {
    sendTelegramMessage("Hello, this is test ! ");
    delay(3000);
  }
}

void test_blynk_alert() {
  delay(4000);
  sendAlert("this is alert");
  Serial.println("Alert sent");
}

void main_code() {
  Blynk.run();
  timer.run();
  reconnectBlynk();
}

void read_sesnors() {
  readFlame1();
  readFlame2();
  read_DHT();
  readSoil1();
  readSoil2();
}

void handle_lcd() {
  if (isFlame1) {
    lcdPrint("Fire Alert", "Land 1");
  } else if (isFlame2) {
    lcdPrint("Fire Alert", "Land 2");
  } else if (needsWater1) {
    lcdPrint("watering", "Land 1");
  } else if (needsWater2) {
    lcdPrint("watering", "Land 2");
  } else if (temp > FAN_TEMP_THRESHOLD) {
    lcdPrint("Very Hot", "Fan is Working");
  } else {
    lcdPrint("Temp : " + String(temp), "Humidity : " + String(humidity));
  }
}

void handle_rgb() {
  if (isFlame1 || isFlame2) {
    setRGB(255, 0, 0);  // red
  } else if (needsWater1 || needsWater2 || fanState) {
    setRGB(0, 0, 255);  // blue
  } else {
    setRGB(0, 255, 0);  // green
  }
}

void handle_servo() {
  if (needsWater1) {
    moveServoToRight();
  } else if (needsWater2) {
    moveServoToLeft();
  } else {
    moveServoToCenter();
  }
}

void handle_pump() {
  // if (servo_angle == 90) { return; }

  if (needsWater1 || needsWater2) {
    pumpON();
  } else {
    pumpOFF();
  }
}

void handle_fan() {
  if (temp > FAN_TEMP_THRESHOLD) {
    fanON();
  } else {
    fanOFF();
  }
}

void sendBothAlerts(String text) {
  sendTelegramMessage(text);
  sendAlert(text);
}

void handle_alerts() {
  if (isFlame1 && last_alert_type != 1) {
    last_alert_type = 1;
    sendBothAlerts("Fire in Land 1 🔥");
  } else if (isFlame2 && last_alert_type != 2) {
    last_alert_type = 2;
    sendBothAlerts("Fire in Land 2 🔥");
  } else if (needsWater1 && last_alert_type != 3) {
    last_alert_type = 3;
    sendBothAlerts("Land 1 needs water 💦");
  } else if (needsWater2 && last_alert_type != 4) {
    last_alert_type = 4;
    sendBothAlerts("Land 2 needs water 💦");
  } else if (humidity > HUMIDITY_ALERT_THRESHOLD && last_alert_type != 5) {
    last_alert_type = 5;
    sendBothAlerts("Humidity is High , take care !!");
  } else if (temp > FAN_TEMP_THRESHOLD && last_alert_type != 6) {
    last_alert_type = 6;
    sendBothAlerts("Temperature is High , Fan is Working ..");
  } else {
    last_alert_type = 0;
  }
}

void reconnectBlynk() {
  if (!Blynk.connected()) {
    Serial.println("Reconnecting Blynk...");
    Blynk.connect();
  }
}
