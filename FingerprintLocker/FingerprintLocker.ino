#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <SoftReset.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    2
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

uint8_t getFingerprintEnroll();


int buttonPressed = 0;
int button = 4;
//int light = 13;
int count = 0;
int enrollMode = 0;
int a = 0;
int b = 0;
int c = 0;
int val = 0;
int readVal = 0;
uint8_t getFingerprintID();
int isReg;
int transistorPin = 13;
int isStored = -1;
int isId = -1;
int checked = 0;
int noFinger = 1;

void setup() {
  //EEPROM.write(1,0);
  pinMode(button, INPUT);
  pinMode(transistorPin, OUTPUT);
  //pinMode(light, OUTPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  int doPrep = EEPROM.read(0);
  if(doPrep != 1) {
    EEPROM.write(1,1);
    EEPROM.write(0,1);
  }
  Serial.begin(9600);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(500);
  
  Serial.begin(9600);
  Serial.println("Adafruit Fingerprint Sensor Test/Enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }

}

void loop() {
  leds[1] = CRGB::Black; FastLED.show();
  if(buttonPressed != 1){
  count++;
  //Serial.println(count);
  }
  
  buttonPressed = digitalRead(button);
  //Serial.print(buttonPressed);
  if (buttonPressed == 1 && count == 1 || enrollMode == 1) {
    enrollMode = 1;
    leds[1] = CRGB::Blue; FastLED.show();
    while(isId != 0 && checked == 0){
      int checkPrint = getFingerprintID();
      Serial.println(isId);
      Serial.println("Place Master Finger...");
      leds[0] = CRGB::Blue; FastLED.show();
        if(isId == 0){
        checked = 1;
      isId = -1;

      }
    }
    Serial.println("Ready to enroll a fingerprint! Please Type in the ID # you want to save this finger as...");
    id = EEPROM.read(1);
    val = id+1;
    EEPROM.write(1,val);
    Serial.print("Enrolling ID #");
    Serial.println(id);
  
  while (!  getFingerprintEnroll() );
  if(isStored == 0){ 
    checked = 0;
    val = id+1;
    EEPROM.write(1,val);
    soft_restart();

  }
    Serial.println("Works");
  }
  
  else {
   
    //Normal
    led('b');
    led('s');

    
  int checkPrint = getFingerprintID();

  

  Serial.println(isId);
  isId = -1;

  if(checkPrint == 1 && checked == 0){
    Serial.println("works!!");
    //unlock;
    leds[0] = CRGB::Green;
    FastLED.show();
    leds[0] = CRGB::Black;
    
    digitalWrite(transistorPin,HIGH);
    delay(6000);
    digitalWrite(transistorPin,LOW);
    FastLED.show();
    //isReg = 0;
    checkPrint = 0;
  }
  else if(noFinger == 0){
    digitalWrite(transistorPin,LOW);
    led('r'); led('s');
    delay(500);
    led('f'); led('s');
  }

  }
  if(buttonPressed == 1){
      while(isId != 0 && checked == 0){
      int checkPrint = getFingerprintID();
      Serial.println(isId);
      Serial.println("Place Master Finger...");
      leds[0] = CRGB::Blue; FastLED.show();

        if(isId == 0){
        checked = 1;
      isId = -1;

      }
    }
    EEPROM.write(1,1);
      c++;
      checked = 0;
  while (c < 163 && c > 0){
    deleteFingerprint(c);
    delay(2);
    Serial.println(c);
    Serial.println("Erasing...");
    leds[0] = CRGB::Pink; FastLED.show(); delay(100);  leds[0] = CRGB::Black; FastLED.show();
      if(c == 162){
        checked = 0;
        soft_restart();
        
        }
     c++;
     }
     
 
  }

}

void deleteAll() {
  //c++;
  //while (c < 162 && c > 0){
    //deleteFingerprint(c);
    //delay(2);
    //Serial.println("Erasing...");
    Serial.println("Erasing...");
  //}
}


uint8_t readnumber(void) {
  uint8_t num = 0;
  boolean validnum = false; 
  while (1) {
    while (! Serial.available());
    char c = Serial.read();
    if (isdigit(c)) {
       num *= 10;
       num += c - '0';
       validnum = true;
    } else if (validnum) {
      return num;
    }
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id); led('y'); led('s');
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error"); led('r'); led('s'); delay(1000);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error"); led('r'); led('s'); delay(1000);
      break;
    default:
      Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(1000);
      return p;
    default:
      Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
      return p;
  }
  
  Serial.println("Remove finger"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show(); delay(500); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again"); led('y'); led('s');
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print("."); 
      break;
    case FINGERPRINT_PACKETRECIEVEERR: 
      Serial.println("Communication error"); led('r'); led('s'); delay(1000);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error"); led('r'); led('s'); delay(1000);
      break;
    default:
      Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(1000);
      return p;
    default:
      Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error"); led('r'); led('s'); delay(1000);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match"); led('r'); led('s'); delay(1000);
    return p;
  } else {
    Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  isStored = p;
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!"); leds[0] = CRGB::Green; FastLED.show(); delay(500);  leds[0] = CRGB::Black; FastLED.show();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error"); led('r'); led('s'); delay(1000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location"); led('r'); led('s'); delay(1000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash"); led('r'); led('s'); delay(1000);
    return p;
  } else {
    Serial.println("Unknown error"); led('r'); led('s'); delay(1000);
    return p;
  }   
}


uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken"); noFinger = 0;
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected"); noFinger = 1;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error"); noFinger = 1; led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error"); noFinger = 1; led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    default:
      Serial.println("Unknown error"); noFinger = 1; led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");  led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error"); led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features"); led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
    default:
      Serial.println("Unknown error"); led('r'); led('s'); delay(250); led('f'); led('s');
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    isReg = 1; led('g'); led('s'); delay(250); led('f'); led('s');
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    isReg = 0; led('r'); led('s'); delay(250); led('f'); led('s');
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    isReg = 0; led('r'); led('s'); delay(250); led('f'); led('s');
    return p;
  } else {
    Serial.println("Unknown error");
    isReg = 0; led('r'); led('s'); delay(250); led('f'); led('s');
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  isId = finger.fingerID; 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  return isReg;
}

//void lock() {
//digitalWrite(transistorPin, LOW);
//}

//void unlock(){
//digitalWrite(transistorPin, HIGH);
//}

char led(char color){
  switch (color){
    case 'b':
    leds[0] = CRGB::LightBlue;
    //Serial.println("LightBlue");
        break;
    case 'y':
    leds[0] = CRGB::Yellow;
        break;
    case 'r':
    leds[0] = CRGB::Red;
        break;
    case 'B':
    leds[0] = CRGB::Blue;
        break;
    case 'p':
    leds[0] = CRGB::Pink;
        break;
    case 'f':
    leds[0] = CRGB::Black;
        break;
    case 's':
    FastLED.show();
    default:
        //Serial.println("You Screwed Up");
        break;
  }
  
}
