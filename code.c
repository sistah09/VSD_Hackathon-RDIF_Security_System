#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

/*Using Hardware SPI of Arduino */
/*MOSI (11), MISO (12) and SCK (13) are fixed */
/*You can configure SS and RST Pins*/
#define SS_PIN 10  /* Slave Select Pin */
#define RST_PIN 7  /* Reset Pin */

#define SERVO_PIN 9  /* Servo Pin - Must be a PWM Pin */

/* Use 'DumpInfo' example in MFRC522 Library to get RFID Card's UID */
/* Replace the following with your RFID UID */
/* Do not put 0's */
/* In my case, UID is F3 9E 3D 03 */
/* So, I put F39E3D3, without any 0's and spaces */ 
String masterTagID = "F39E3D3";

/* Create a string to capture scanned card's UID */
String scanTagID = "";

/* Create an instance of Servo */
Servo myservo;

/* Create an instance of MFRC522 */
MFRC522 mfrc522(SS_PIN, RST_PIN);

/* Create an instance of LiquidCrystal_I2C */
/* LiquidCrystal_I2C lcd(Slave Address, # of Columns, # of Rows); */
LiquidCrystal_I2C lcd(0x3F, 16, 2);

/* Custom Character for Happy Face */
byte customCharGranted[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

/* Custom Character for Frown Face */
byte customCharDenied[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000
};


void setup() 
{
  /* Initialize LCD */
  lcd.init();
  lcd.backlight();
  /* Store Custom Characters into ROM of LCD */
  lcd.createChar(0, customCharGranted);
  lcd.createChar(1, customCharDenied);
  lcd.setCursor(0,0);
  lcd.print("   RC522 RFID   ");
  lcd.setCursor(0,1);
  lcd.print(" Access Control ");
  /* Initialize Servo */
  myservo.attach(SERVO_PIN);
  /* Set initial position of Servo */
  /* In this position, the gate is closed */
  myservo.write(90); 
    /* Initialize SPI Bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*Scan Your Card*");    
}

void loop()
{
  while (readTagID())
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (scanTagID == masterTagID) 
    {
      lcd.print("Access Granted ");
      lcd.write(0);
      lcd.setCursor(0, 1);
      lcd.print("Welcome: ");
      lcd.print(scanTagID);
      /* Open the gate */
      myservo.write(20); 
    }
    else
    {
      lcd.print("Access Denied  ");
      lcd.write(1);
      lcd.setCursor(0, 1);
      lcd.print("Get Out: ");
      lcd.print(scanTagID);
      //myservo.write(90); 
    }

    delay(3000);
     /* Close the gate after a delay of 3s */
    myservo.write(90);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("*Scan Your Card*");

  }
}

boolean readTagID() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }
  /* Clear the string */
  scanTagID = "";
  for ( uint8_t i = 0; i < 4; i++) 
  {
    scanTagID += String(mfrc522.uid.uidByte[i], HEX);
  }
  scanTagID.toUpperCase();
  mfrc522.PICC_HaltA();
  return true;
}