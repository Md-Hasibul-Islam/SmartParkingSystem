#include <Servo.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

//Global Pin Declaration:
#define TRIG_PIN 8
#define ECHO_PIN 9
#define IR_GATE_PIN A0
#define IR_SLOT_1_PIN A1
#define IR_SLOT_2_PIN A2
#define IR_SLOT_3_PIN A3
#define IR_SLOT_4_PIN A4
#define RFID_SS_PIN 5
#define RFID_RST_PIN A5

//Creating Objects & Variables:
Servo servo;
int servoAngle = 0;
bool servoTurning = false;
long ultrasonicDuration;
int ultrasonicDistance;
int buzzerPin = 7;
bool buzzerOn = false;
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);

LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

void setup() {

  //Ultrasonic:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //IR Sensors:
  pinMode(IR_GATE_PIN, INPUT);
  pinMode(IR_SLOT_1_PIN, INPUT);
  pinMode(IR_SLOT_2_PIN, INPUT);
  pinMode(IR_SLOT_3_PIN, INPUT);
  pinMode(IR_SLOT_4_PIN, INPUT);

  //Buzzer:
  pinMode(buzzerPin, OUTPUT);

  //LCD Display
  lcd.begin(16, 2);
  lcd.print("Loading...");

  //Servo:
  servo.attach(3);
  servo.write(servoAngle);

  //RFID:
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522


  Serial.begin(9600);
}

void loop() {
  int irSlotReading1 = digitalRead(IR_SLOT_1_PIN);
  int irSlotReading2 = digitalRead(IR_SLOT_2_PIN);
  int irSlotReading3 = digitalRead(IR_SLOT_3_PIN);
  int irSlotReading4 = digitalRead(IR_SLOT_4_PIN);
  int irGateReading = digitalRead(IR_GATE_PIN);

  lcd.clear();
  lcd.setCursor(0, 0);

  //Check for free slots: 
  if (irSlotReading1 == LOW && irSlotReading2 == LOW && irSlotReading3 == LOW && irSlotReading4 == LOW) {
    lcd.print("No Slots Available");
    Serial.println("No Slots Available");
  } else {
    int freeSlotCount = 0;
    if (irSlotReading1 == HIGH) {
      lcd.print("Slot 1 ");
      Serial.print("Slot 1 ");
      freeSlotCount++;
    }
    if (irSlotReading2 == HIGH) {
      lcd.print("Slot 2 ");
      Serial.print("Slot 2 ");
      freeSlotCount++;
    }
    if (irSlotReading3 == HIGH) {
      lcd.print("Slot 3 ");
      Serial.print("Slot 3 ");
      freeSlotCount++;
    }
    if (irSlotReading4 == HIGH) {
      lcd.print("Slot 4 ");
      Serial.print("Slot 4 ");
      freeSlotCount++;
    }
    lcd.setCursor(0, 1);
    lcd.print(freeSlotCount + " Free Slots");
    Serial.print("\tFree Slots: ");
    Serial.println(freeSlotCount);
  }

  // Check for RFID tag:
// if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//   // Verify the UID of the tag:
//   String uid = "";
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
//     uid.concat(String(mfrc522.uid.uidByte[i], HEX));
//   }

//   // Print the UID:
//   Serial.print("Card UID: ");
//   Serial.println(uid);

//   // Check if the UID matches the authorized tag:
//   if (uid == "OUR_AUTHORIZED_UID") {
//     // Move the servo to angle 90:
//     if (!servoTurning) {
//       for (int i = 0; i <= 90; i++) {
//         servo.write(i);
//         delay(0);
//       }
//       servoTurning = true;
//     }
//   } else {
//     servo.write(0);
//     // Buzz the buzzer for 1 second:
//     // if (!buzzerOn) {
//     //   digitalWrite(buzzerPin, HIGH);
//     //   delay(1000);
//     //   digitalWrite(buzzerPin, LOW);
//     //   buzzerOn = true;
//     // }
//   }
// }


  if (irGateReading == LOW) {
    if (servoAngle == 0) {
      servoTurning = true;
      servoAngle = 90;
      servo.write(servoAngle);
      Serial.println("Car exiting. Turning servo to 90 degrees.");
    } else if (servoAngle == 90) {
      servoTurning = true;
      servoAngle = 0;
      servo.write(servoAngle);
      Serial.println("Car entered. Turning servo to 0 degrees.");
    }
  } else {
    if (servoTurning) {
      servoTurning = false;
      servo.write(servoAngle);
      // Serial.print("No obstacle detected. Returning servo to ");
      Serial.println(servoAngle);
    }


  // Ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  ultrasonicDuration = pulseIn(ECHO_PIN, HIGH);
  ultrasonicDistance = ultrasonicDuration * 0.034 / 2;

  if (ultrasonicDistance > 2.5 && ultrasonicDistance < 12) {
    if (!buzzerOn) {
      buzzerOn = true;
      tone(buzzerPin, 1000);
    }
  } else if (ultrasonicDistance <= 2.5) {
    if (!buzzerOn) {
      buzzerOn = true;
      tone(buzzerPin, 1000, 300);
    }
  } else {
    if (buzzerOn) {
      buzzerOn = false;
      noTone(buzzerPin);
    }
  }
  }
}
  