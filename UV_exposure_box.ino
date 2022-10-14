#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int buttonStartPause = 2;
const int buttonReset = 3;
const int led = LED_BUILTIN;
const int relay = 6;

//Rotary encoder
#define CLK 4
#define DT 5
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool isLightning = false;
bool isPaused = false;


int time = 64;
int remainingTime;
int cells = 0;
double percentage;
int pauseTime = 0;

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT_PULLUP);  //const
  pinMode(3, INPUT_PULLUP);
  pinMode(relay, OUTPUT);

  //Rotační enkodér
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT); 
  

  // LCD
  lcd.begin();
  lcd.backlight();

  attachInterrupt(0, pressedStartPauseButton, FALLING);
  attachInterrupt(1, pressedResetButton, FALLING);

  Serial.begin(9600);
}

void lightning(){
  remainingTime = time;
  
  while(remainingTime > 0) {
    if (!isPaused){
      delay(1000);
      remainingTime--;
      digitalWrite(relay, HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Zbyva ");
      lcd.print(remainingTime);
      lcd.print(" s");
    }
    else{
      digitalWrite(relay, LOW);
      lcd.setCursor(0,1);
      lcd.print("     Paused     ");
    }
  }
  digitalWrite(relay, LOW);

  isLightning = false;
}


void pressedStartPauseButton() {
  if (!isLightning){
    isLightning = true;
  }
  else {
     isPaused = !isPaused;
  }
}

void pressedResetButton(){
  remainingTime = 0;
  isPaused = false;
}

void encoder(){
	// Read the current state of CLK
	currentStateCLK = digitalRead(CLK);

	// Iftimend current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

		// ItimeT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK) {
			time --;
			currentDir ="CCW";
		} else {
			// Encoder is rotating CW so increment
			time ++;
			currentDir ="CW";
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(time);

    printTimeValue();
  }

	// Remember last CLK state
	lastStateCLK = currentStateCLK;

	// Put in a slight delay to help debounce the reading
	delay(1);
}

void printTimeValue(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nastaveni casu");
  lcd.setCursor(0, 1);
  lcd.print("Cas: ");
  lcd.print(time);
  lcd.print(" s");
}

void loop(){
  isPaused = false;
  isLightning = false;
  do {
    encoder();
  } while (!isLightning);

  lightning();

  printTimeValue();


}