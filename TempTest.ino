int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

//create constants for the three analog input pins
const int redPot = 0;
const int greenPot = 1;
const int bluePot = 2;

//create constants for the three RGB pulse width pins
const int redPin = 5;
const int greenPin = 6;
const int bluePin = 9;

//create variables to store the red, green and blue values
int redVal;
int greenVal;
int blueVal;


void setup() {
Serial.begin(9600);
}

void loop() {

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;

  Serial.println("Temperature: "); 
  Serial.println(T);

  delay(1000);

  //read the three analog input pins and store their value to the color variables
  redVal = analogRead(redPot);
  greenVal = analogRead(greenPot);
  blueVal = analogRead(bluePot);
    
      redVal = map(redVal, 0, 1023, 0, 255);
      greenVal = map(greenVal, 0, 1023, 0, 255);
      blueVal = map(blueVal, 0, 1023, 0, 255);

      analogWrite(redPin, redVal);
      analogWrite(greenPin, greenVal); 
      analogWrite(bluePin, blueVal);

      if (T < 16)  {
        digitalWrite (bluePin, HIGH);
      } else if (T < 20) {
        digitalWrite (greenPin, HIGH);
      } else if (T < 24) {
        digitalWrite (bluePin, LOW);
      } else if (T < 28) {
        digitalWrite (redPin, HIGH);
      } else if (T < 31)  {
        digitalWrite (bluePin, HIGH);
        digitalWrite (greenPin, LOW);
        digitalWrite(redPin, HIGH);
      } else  {
        digitalWrite (redPin, HIGH);
        digitalWrite (bluePin, LOW);
        digitalWrite (greenPin, LOW);
      }
}
