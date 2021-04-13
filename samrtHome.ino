#include<Wire.h>
#include<String.h>
#include"SharpGP2Y10.h"//Thư viện cảm biến bụi
#include"DHT.h"//Thư viện cảm biến nhiệt độ
#include<BH1750.h>//Thư viện cảm biến cường độ ánh sáng
#include <Servo.h>//thư viện của động cơ servo (mới thêm)

#define DHTPIN 4  //khai báo chân cho cảm biến nhiệt độ vào chân digital 4
#define DHTTYPE DHT11 //chọn cảm biến DHT11
#define bui_Vo_Pin A0 //Chân A0 của cảm biến bụi
#define bui_led_Pin 2 //Chân D2 của cảm biến bụi
#define cambien_mua_pin 8  //Cảm biến mưa D8
#define servo 10 //khai báo chân cho động cơ servo (mới thêm)
#define l298_in1 11 //chân in1 của l298 đk động cơ
#define l298_in2 12 //chân in2 của l298 đk động cơ

int relay[3] = {5, 6, 7};
//Chân In của relay 1 điều khiển bóng đèn
//Chân In của relay 2 điều khiển quạt
//Chân In của relay 3 điều khiển còi cảnh báo

float dustDensity = 0;//Mật độ bụi

BH1750 lightMeter;//khai báo đối tượng cho cảm biến cường độ ánh sáng
DHT dht(DHTPIN, DHTTYPE);//khai báo đối tượng cho cảm biến nhiệt độ
SharpGP2Y10 dustSensor(bui_Vo_Pin, bui_led_Pin);//Khai báo đối tượng cho cảm biến bụi

Servo myservo;  //khai báo đối tượng cho động cơ servo (mới thêm)

//Đọc cảm biến nhiệt độ: sử dụng chân D4
void camBienNhietDoDoAm() { //hàm cho chế độ thủ công (x:get)
  float h = dht.readHumidity();//đọc giá trị nhiệt độ từ cảm biến
  float t = dht.readTemperature();//đọc giá trị độ ẩm từ cảm biến

  Serial.print(t);
  //Serial.print("độ C ");
  Serial.print(" ");
  Serial.print(h);
  //Serial.print("%");
}

void nhietDo() { //hàm cho chế độ tự động
  float t = dht.readTemperature();
  //Serial.println("nhiet do");
  if (t >= 30) {
    digitalWrite(relay[1], LOW);
    //Serial.println("quat da bat");
  } else {
    digitalWrite(relay[1], HIGH);
    //Serial.println("quat da tat");
  }
}

//Đọc cảm biến bụi
void camBienBui() {
  dustDensity = dustSensor.getDustDensity();
  Serial.print(dustDensity);
}


//Đọc cảm biến cường độ ánh sáng
void camBienCuongDoAnhSang() {
  float lux = lightMeter.readLightLevel();
  Serial.print(lux);
}

void anhSang() {
  float lux = lightMeter.readLightLevel();
  //Serial.println("anh sang");
  if (lux <= 50.0) {
    digitalWrite(relay[0], LOW);
    //Serial.println("den da bat");
  } else {
    digitalWrite(relay[0], HIGH);
    //Serial.println("den da tat");
  }
}

//Đọc cảm biến khí ga sử dụng chân A1
void camBienKhiGa() {
  int value = analogRead(A1);
  Serial.print(value);
}

void khiGas() {
  int value = analogRead(A1);
  //Serial.println("khi ga");
  if (value >= 400) {
    digitalWrite(relay[2], LOW);
    //Serial.println("canh bao");
  } else {
    digitalWrite(relay[2], HIGH);
    //Serial.println("khong canh bao");
  }
}


//Đọc cảm biến mưa sử dụng chân D8
void camBienMua() {
  int value = digitalRead(cambien_mua_pin);//lưu giá trị cảm biến vào biến value
  Serial.print(value);
}
/*
  void blink1() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  }*/

void mua()
{
  if (!digitalRead(cambien_mua_pin))
  {
    Serial.println("It's raining outside");
    digitalWrite(l298_in1, HIGH);
    digitalWrite(l298_in2, LOW);
    delay(100);
    digitalWrite(l298_in1, LOW);
    digitalWrite(l298_in2, LOW);
    while (!digitalRead(cambien_mua_pin))
    {
      digitalWrite(l298_in1, LOW);
      digitalWrite(l298_in2, LOW);
    }
    if (digitalRead(cambien_mua_pin))
    {
      Serial.println("It's not raining anymore");
      digitalWrite(l298_in1, LOW);
      digitalWrite(l298_in2, HIGH);
      delay(100);
      digitalWrite(l298_in1, LOW);
      digitalWrite(l298_in2, LOW);
    }
  }
  else
  {
    Serial.println("It's not raining outside");
  }
}
//Điều khiển tự động
void automatic() {
  nhietDo();
  anhSang();
  khiGas();
  //mua();
}

//Điều khiển bán thủ công
void manual(String x) {
  if (x == "x: ledon" || x == "x: ledon\n") {
    Serial.println(x);
    digitalWrite(relay[0], LOW);
    Serial.println("den da bat");
    Serial.print("******\n");
  }
  if (x == "x: ledoff" || x == "x: ledoff\n") {
    Serial.println(x);
    digitalWrite(relay[0], HIGH);
    Serial.println("den da tat");
    Serial.print("******\n");
  }

  if (x == "x: quaton" || x == "x: quaton\n") {
    Serial.println(x);
    digitalWrite(relay[1], LOW);
    Serial.println("quat da bat");\
    Serial.print("******\n");
  }
  if (x == "x: quatoff" || x == "x: quatoff\n") {
    Serial.println(x);
    digitalWrite(relay[1], HIGH);
    Serial.println("quat da tat");
    Serial.print("******\n");
  }

  if (x == "x: warning" || x == "x: warning\n") {
    Serial.println(x);
    digitalWrite(relay[2], LOW);
    Serial.println("canh bao");
    Serial.print("******\n");
  }
  if (x == "x: nowarning" || x == "x: nowarning\n") {
    Serial.println(x);
    digitalWrite(relay[2], HIGH);
    Serial.println("khong canh bao");
    Serial.print("******\n");
  }

  if (x == "x: open" || x == "x: open\n")
  {
    Serial.println("opening the door");
    myservo.write(90);
    Serial.print("******\n");
  }
  if (x == "x: close" || x == "x: Close\n")
  {
    Serial.println("closing the door");
    myservo.write(0);
    Serial.print("******\n");
  }

  if (x == "x: comua" || x == "x: comua\n" )
  {
    Serial.println("dang don do");
    digitalWrite(l298_in1, HIGH);
    digitalWrite(l298_in2, LOW);
    delay(3000);
    digitalWrite(l298_in1, LOW);
    digitalWrite(l298_in2, LOW);
    Serial.print("******\n");
  }
  if (x == "x: tanhmua" || x == "x: tanhmua\n")
  {
    Serial.println("cho do ra");
    digitalWrite(l298_in1, LOW);
    digitalWrite(l298_in2, HIGH);
    delay(3000);
    digitalWrite(l298_in1, LOW);
    digitalWrite(l298_in2, LOW);
    Serial.print("******\n");
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);  //chân nào đây???
  pinMode(cambien_mua_pin, INPUT); // pinMode ở chế độ nhận tín hiệu vào cho cảm biến
  digitalWrite(cambien_mua_pin, HIGH);
  Wire.begin();
  lightMeter.begin();
  //Serial.println(F("BH1750 test begin"));
  dht.begin();
  for (int i = 0; i <= 2; i++) {
    pinMode(relay[i], OUTPUT);
  }
  digitalWrite(relay[0], HIGH);
  digitalWrite(relay[1], HIGH);
  digitalWrite(relay[2], HIGH);

  pinMode(l298_in1, OUTPUT);
  pinMode(l298_in2, OUTPUT);
  pinMode(servo, OUTPUT);
  myservo.attach(10); //gán chân của đối tượng thư viện servo với chân nối với servo

}


void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    Serial.print("Mode: ");
    if (data == "x: manual" || data == "x: manual\n") {
      Serial.println("------");
      Serial.println("manual");
      Serial.println("******\n");
      //blink1();
      while (1) {
        String data1 = Serial.readStringUntil('\n');
        manual(data1);
        if (data1 == "x: end" || data1 == "x: end\n") {
          Serial.println("da ket thuc");
          break;
        }
        if (data1 == "x: get" || data1 == "x: get\n") {
          Serial.println("Getting data");
          Serial.print("y: ");
          camBienNhietDoDoAm();
          Serial.print(" ");
          camBienMua();
          Serial.print(" ");
          camBienBui();
          Serial.print(" ");
          camBienKhiGa();
          Serial.print(" ");
          camBienCuongDoAnhSang();
          Serial.println("\n******\n");
        }
        delay(500);
      }
    }
  }
  //Serial.println("auto");
  automatic();
  delay(1000);
  Serial.print("########\n");
}
