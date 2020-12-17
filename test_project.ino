#include "DHT.h"
DHT dht;
ISR(TIMER1_COMPA_vect);
int quartersec = 0, sec = 0, minutes = 0, hours = 0;
int normal = HIGH;

int red_led = 9;
int green_led = 8;
int button = 7;
int sound = 11;
int fan = 12;

int Time = millis();
int timeCheck;


void setup()
{
  dht.setup(2); // data pin 2
  Serial.begin(9600);
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(sound, OUTPUT);
  pinMode(fan, OUTPUT);
  

  noInterrupts();
  TCCR1A = 0; // Clear TCCR1A/B registers
  TCCR1B = 0;
  TCNT1 = 0;

  // set compare match register for TIMER1: CLOCKFREQUENCY / frequency / prescaler - 1   การคิดค่าออกมาใช้เพื่อนำจำนวนครั้ง
  OCR1A = 3905; // 12MHz/(1Hz*1024) – 1 (must be <65536)

  TCCR1B |= (1 << WGM12); // Mode 4, turn on CTC mode

  TCCR1B |= (1 << CS12) | (1 << CS10); // Set CS10 and CS12 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // นับค่าไปเรื่อยๆ จนครบจำนวนรอบที่หาร แล้วถึงทำ ฟังชั่น interrupt 1 รอบ แบบนี้วนไปเรื่อยๆ

  interrupts();

}


void loop()
{

  float humidity = dht.getHumidity(); // ดึงค่าความชื้น
  float temperature = dht.getTemperature(); // ดึงค่าอุณหภูมิ
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);

  ///////////////////////////////////

  Time = millis();
  button = digitalRead(7);
  //Serial.println(Time);
  // Serial.println(sec);
  if (normal)
  {
    digitalWrite(green_led, HIGH);
    //Serial.println(digitalRead(7));
    if (button == LOW)
    {
      normal = !normal;
      digitalWrite(green_led, LOW);
      timeCheck = Time;
    }
  }
  else
  {
    digitalWrite(fan, HIGH);
    if (quartersec % 2 == 0)
    {
      digitalWrite(red_led, HIGH);
      digitalWrite(sound, HIGH);
    }
    else
    {
      digitalWrite(red_led, LOW);
      digitalWrite(sound, LOW);
    }
    if (Time - timeCheck >= 5000)
    {
      normal = !normal;
      digitalWrite(red_led, LOW);
      digitalWrite(sound, LOW);
      digitalWrite(fan, LOW);
    }

  }



}



ISR(TIMER1_COMPA_vect)
{
  quartersec++;
  if (quartersec >= 4)
  {
    sec++;
    quartersec = 0;
  }
  if (sec >= 60)
  {
    minutes++;
    sec = 0;

  }
  if (minutes >= 60)
  {

    hours++;
    minutes = 0;

  }
}
