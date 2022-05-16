
// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int stdbyTime = 30;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int btnl=8, btnm=9, btnr=10, shutter=11, LED=12, LED2=13;
const int backlight = A0, focus = A1;

bool btnlSt, btnmSt, btnrSt;
// nFoto, delay
int setting[2] = {10, 10};
int menu = 0;
int initSpd = 500;
bool exti = false;
int row = 0;
int maxRows = 2;
int held = 1;
int stp = 0;
long fatte = 0;
long daFare = setting[0];
long del = setting[1];
long elapsed = 0;
bool done = false;
int shootDelay = 0;
int sby = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void lcdClear(int row = 2) {
  switch (row) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      break;
     case 1:
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
     case 2:
      lcdClear(0);
      lcdClear(1);
      break;
  }
}
void initi() { //Initialization screen
  
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Initializing!"); 
  delay(initSpd);

  lcd.setCursor(0, 0);
  lcd.print("Initializing.!");
  delay(initSpd);

  lcd.setCursor(0, 0);
  lcd.print("Initializing..!");
  delay(initSpd);

  lcd.setCursor(0, 0);
  lcd.print("Initializing...!");
  delay(initSpd);

  lcd.setCursor(0, 0);
  lcd.print("Hi!             ");
  lcd.setCursor(0, 1);
  lcd.print("Settings - Start");
}


void setup() {
  // set up the LCD's number of columns and rows:
  Wire.setClock(10000);
  lcd.begin(16, 2);

  pinMode(btnl, INPUT);
  pinMode(btnm, INPUT);
  pinMode(btnr, INPUT);
  pinMode(shutter, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(backlight, OUTPUT);
  pinMode(focus, OUTPUT);
  digitalWrite(shutter, HIGH);
  digitalWrite(focus, HIGH);
  digitalWrite(LED, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(backlight, LOW);
  menu = 0;
  
  initi();
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("Hi!             ");
  lcd.setCursor(0, 1);
  lcd.print("Settings - Start          ");
  
  if(digitalRead(btnl)) {
    
    if(menu == 0){
      menu = 1;
      //Settings menu:
      String settings[2] = {"Foto:", "Delay:"};
      
      lcd.setCursor(0, 0);
      lcd.print("Settings       ^");
      lcd.setCursor(0, 1);
      lcd.print(settings[0] + setting[0]);
      
      lcdClear(1);
      delay(200);
      while(!exti) {
        lcd.setCursor(0, 1);
        lcd.print(settings[row] + setting[row] + "            ");
        
        if(digitalRead(btnl)) { //BTN LEFT BEHAVIOUR
          if(setting[row] > 1) {
            setting[row]--;  
          }
          delay(200 / held);
          held++;

          if(digitalRead(btnr)) {
            setting[row] = 10;
            delay(500); 
          }
        }
        if(digitalRead(btnm)) {//BTN MIDDLE BEHAVIOUR
          
          setting[row]++;
          delay(200 / held);
          held++;

          if(digitalRead(btnr)) {
            setting[row] += 1000;
            delay(300); 
          }
        }
        if(digitalRead(btnr)) {//BTN RIGHT BEHAVIOUR
          row+=1;
          row >= 2 ? row = 0 : row = row;
          delay(400);
          if(digitalRead(btnr)) {
            exti = true;
            menu = 0;
            break;
          }
        }

        if(digitalRead(btnl) || digitalRead(btnm) || digitalRead(btnr)) {
          if (held < 40){
            held ++;
          }
        } else {
          held = 1;
        }
      }
    }
  }
  
  if(digitalRead(btnm)) { // START BEHAVIOUR
    
    
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    
    while(!done){ //Shooting cycle
      if(stp == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Foto:" + (String)fatte + "/" + (String)daFare);
        lcd.setCursor(0, 1);
        lcd.print("Next:"+ (String)(del-elapsed) + "|" + (String)(((daFare-fatte)*del-elapsed)) + "s     ");
      }
      delay(1000-shootDelay); //Main clock
      sby += 1; //add second to standby timer;
      shootDelay=0;
      elapsed ++;

      if (stdbyTime <= sby) { // if standby elapsed, turn off
        digitalWrite(backlight, HIGH);
      }

      if(digitalRead(btnr)) {
        sby = 0; //rst stdby
        digitalWrite(backlight, LOW);
        if(stp == 1) { //?????? don't know what it is but works so ok
          stp = 0;
        }
        if(stp == 0){ //ASK IF EXIT FROM SHOOTING MODE
          digitalWrite(backlight, LOW);
          digitalWrite(LED2, LOW);
          lcd.setCursor(0, 0);
          lcd.print("Exit?             ");
          lcd.setCursor(0, 1);
          lcd.print("YES  /  NO         ");
          stp = 1;
        }
        
        delay(500);
        shootDelay += 500;
      }
      if(digitalRead(btnm)) {
        sby = 0; //rst stdby
        digitalWrite(backlight, LOW);
      }
      if(stp == 1 && digitalRead(btnl)) { //YES, EXIT 
        digitalWrite(LED2, HIGH);
        daFare = fatte;
        stp = 0;
      } else if (stp == 0 && digitalRead(btnl)) { //TOGGLE BACKLIGHT
        digitalWrite(backlight, !digitalRead(backlight));
        sby = 0; //rst stdby
      }
      if(stp == 1 && digitalRead(btnm)) { //NO DON'T EXIT
        sby = 0; //rst stdby
        digitalWrite(backlight, LOW);
        digitalWrite(LED2, HIGH);
        stp = 0;
      }
      
      if(elapsed >= del) { //SHOOT PHOTO
        elapsed = 0;
        if(stp == 0) {
          lcd.setCursor(0, 0);
          lcd.print("SHOOTING!             ");
        }
        digitalWrite(LED, LOW);
        delay(300);
        delay(100);
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
        
        digitalWrite(shutter, LOW);
        
        delay(300);
        digitalWrite(shutter, HIGH);
        digitalWrite(LED, HIGH);
        shootDelay += 900;
        fatte ++;
      }
      if (fatte >= daFare){ //DONE SHOOTING
        digitalWrite(backlight, LOW);
        digitalWrite(LED2, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Done! " + (String)daFare + " Foto        ");
        lcd.setCursor(0, 1);
        lcd.print("            Exit");
        while (true) {
          digitalWrite(LED, LOW);
          delay(500);
          digitalWrite(LED, HIGH);
          delay(500);
          if(digitalRead(btnr)) { //Exit Done Menu
            digitalWrite(LED2, HIGH);
            break;
          }
        }
        lcd.setCursor(0, 0);
        lcd.print("Bye!         ");
        delay(1000);
        break;
      }
    }
  }
  if(digitalRead(btnr)) {
    delay(400);
    while(true){
      lcd.setCursor(0, 0);
      lcd.print("Manual Shooting!");
      lcd.setCursor(0, 1);
      lcd.print("SHOOT-FOCUS-EXIT");
      if(digitalRead(btnl)) { //SHOOT
        digitalWrite(LED, LOW);
        delay(300);
        digitalWrite(LED, HIGH);
        digitalWrite(shutter, LOW);
        delay(300);
        digitalWrite(shutter, HIGH);
      }
      if(digitalRead(btnm)) { //FOCUS 
        digitalWrite(focus, LOW);
        digitalWrite(LED2, LOW);
      } else { //FOCUS RELEASE
        digitalWrite(focus, HIGH);
        digitalWrite(LED2, HIGH);
      }
      if(digitalRead(btnr)) { //EXIT
        delay(200);
        break;
      }
    }
  }
}
