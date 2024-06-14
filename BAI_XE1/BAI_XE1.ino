#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10
#define RELAY_PIN       6  // Chọn chân điều khiển relay (ví dụ: chân 3)
int relayState = LOW; 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
LiquidCrystal_I2C lcd(0x27,16,2); 

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID, user, bienxe;

char rdata;
String data_r;
String myString;

Servo cua_vao;

int anhsang, lua = HIGH;
int s1;
int s2;
int s3;
int s4;
int cua = 3;
                                              \[[
void senddata() {
  readsuccess = getid();
  if (readsuccess) {
    user_data();
    Serial.println((String) "DATA,DATE,TIME," + StrUID + ',' + user  + ',' + bienxe  + ',' + "In" );
    if (user == "Warning. Wrong Card" ) {}
    else {
      mo_cua();
      delay(2000); // Đợi một khoảng thời gian trước khi tắt relay
      digitalWrite(cua, LOW); // Tắt relay sau khi đã mở cửa
    }
  }
}

void mo_cua() {   
  // Mở cửa bằng servo
  digitalWrite(cua,HIGH);
  LCD_TRUE();
  for (int i = 0; i < 2; i++) {
    digitalWrite(5, LOW);       
    delay(100); 
    digitalWrite(5, ~LOW);       
    delay(100);  
  } 
  delay(3200);
}

void user_data() {
  if (StrUID == "D3C9B00C") {  
    user = "Nguyen Van Thien"; bienxe = "30A44509";
  } else if (StrUID == "0477425B") {
    user = "Do Thanh Tung"; bienxe = "30A44509";
  } else if (StrUID == "2280451A") {
    user = "Dang Xuan Quang"; bienxe = "30A44509";
  } else if (StrUID == "7944BFC2") {
    user = "Luong Van Thuong"; bienxe = "30A44509";
  } else {  
    user = "Warning. Wrong Card" ;
    LCD_FALSE();
    for (int i = 0; i < 4; i++) {
      digitalWrite(5, LOW);       
      delay(500); 
      digitalWrite(5, ~LOW);       
      delay(500);  
    } 
  }
}

int getid() {  
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i];
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}

void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

void sensor() {
  s1 = digitalRead(A0);
  s2 = digitalRead(A1);
  s3 = digitalRead(A2);
  s4 = digitalRead(A3);
  anhsang = digitalRead(8);
  lua = digitalRead(7);

  if (anhsang == HIGH) digitalWrite(4, LOW); //batden
  else digitalWrite(4, HIGH);
 
  if (lua == LOW) {
    LCD_BAO_CHAY();
    digitalWrite(RELAY_PIN, HIGH); // Bật relay khi có cháy
    digitalWrite(5, LOW); // loa keu 
  } else {
    digitalWrite(5, HIGH);
    LCD();               
  }  
}

void LCD() {
  lcd.setCursor(0, 0);   
  lcd.print("     Welcome    ");
  lcd.setCursor(0, 1);
  lcd.print("Check Your Card ");
}

void LCD_FALSE() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Wrong card");
  lcd.setCursor(0, 1);
  lcd.print("Check Again");
}

void LCD_TRUE() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1    2    3    4"); 
  if (s1 == LOW) {
    lcd.setCursor(0, 1);
    lcd.print("X    "); 
  } else {
    lcd.setCursor(0, 1);
    lcd.print("V    "); 
  }
  if (s2 == LOW) {
    lcd.setCursor(5, 1);
    lcd.print("X    "); 
  } else {
    lcd.setCursor(5, 1);
    lcd.print("V    ");
  }
  if (s3 == LOW) {
    lcd.setCursor(10, 1);
    lcd.print("X    "); 
  } else {
    lcd.setCursor(10, 1);
    lcd.print("V    "); 
  }
  if (s4 == LOW) {
    lcd.setCursor(15, 1);
    lcd.print("X"); 
  } else {
    lcd.setCursor(15, 1);
    lcd.print("V"); 
  }    
}

void LCD_BAO_CHAY() {
  lcd.setCursor(0, 0);
  lcd.print("****Warning*****");
  lcd.setCursor(0, 1);
  lcd.print("Canh Bao Co Chay");
}

void setup() {  
  Serial.begin(9600);
pinMode(cua,OUTPUT);
digitalWrite(cua,LOW);

lcd.init();
lcd.init(); // initialize the lcd
lcd.backlight();

SPI.begin(); // Init SPI bus
mfrc522.PCD_Init(); // Init MFRC522 card

Serial.println("CLEARDATA");
Serial.println("LABEL,Date,Time,RFID UID,USER,Bien xe,IN/OUT");
delay(1000);
Serial.println("Scan PICC to see UID...");
Serial.println("");
delay(250);
LCD();
delay(250);
}

void loop() {
if (Serial.available() == 0) {
sensor();
senddata();
}
if (Serial.available() > 0) {
rdata = Serial.read();
myString = myString + rdata;
if (rdata == '\n') {
data_r = myString;
myString = "";
Serial.println(data_r);
}
}
}
