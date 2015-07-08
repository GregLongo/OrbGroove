/*
* LED Orb Groove
 *
 *  This code is used to drive the sound activated lighting orbs from 
 *  PEX Summer Festival 2013. It utilizes the MSGEQ7 spectrum analyzer chip
 *  to analyze incoming audio data, and then maps the amplitude of different
 *  frequency bands to the light intensity of the corresponding orb.
 *
 */

#include <FastSPI_LED.h>

#define NUM_LEDS 40

int strobe = 4; // strobe pins on digital 4
int res = 5; // reset pins ondigital 5
float left[7]; //freq bands left channel
float right[7];//freq bands right channel
int band;

struct CRGB { 
  unsigned char b; 
  unsigned char r; 
  unsigned char g; 
};
struct CRGB *leds;
int mode;
int time;
int val2;

//Initialize brightness values
float Intensity1;
float Intensity2;
float Intensity3;
float Intensity4;
float Intensity5;

//Initialize color values to default blue
int rr;
int gg;
int bb;

int r1 = 0;
int g1 = 0;
int b1 = 200;

int r2 = 0;
int g2 = 0;
int b2 = 200;

int r3 = 0;
int g3 = 0;
int b3 = 200;

int r4 = 0;
int g4 = 0;
int b4 = 200;

int r5 = 0;
int g5 = 0;
int b5 = 200;

//initialize orbs
int orb1;
int orb2;
int orb3;
int orb4;
int orb5;

//initialize smoothing values
float smoothedVal     = 0.0;
float smoothedVal2     = 0.0;
float smoothedVal3     = 0.0;
float smoothedVal4     = 0.0;
float smoothedVal5     = 0.0;

void setup(){

  Serial.begin(9600);//serial terminal for debugging

  pinMode(7, INPUT_PULLUP);//inititalize mode switch 
  pinMode(res, OUTPUT); // reset
  pinMode(strobe, OUTPUT); // strobe
  digitalWrite(res,LOW); // reset low
  digitalWrite(strobe,HIGH); //pin 5 is RESET on the shield

  //initialize FastSPI_LED library and specify WS2801 chipset 
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);
  FastSPI_LED.setDataRate(2);

  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
}

void loop() { 

  mode = digitalRead(7);//hardware switch for music or standalone mode

  if(mode == LOW){
    //standalone mode. lights fade up and down or "breathe" at a specified tempo
    int breath = time;  
    time++;
    changeColor1();
    if (time > 3000){
      breath = 6000 - time;
    }
    if (time == 6000){
      time = 1;
    }
    smoothedVal = breath/3000.0;
    smoothedVal2 = breath/3000.0;
    smoothedVal3 = breath/3000.0;
    smoothedVal4 = breath/3000.0;
    smoothedVal5 = breath/3000.0;

    smoothedVal = smoothedVal + 0.1;
    smoothedVal2 = smoothedVal + 0.1;
    smoothedVal3 = smoothedVal + 0.1;
    smoothedVal4 = smoothedVal + 0.1;
    smoothedVal5 = smoothedVal + 0.1;
    //Serial.println(smoothedVal);
  }
  else{
    //music mode
    readMSGEQ7();//read from spectrum analyzer
    changeColor2();//change orb colors

    //map spectrum analyzer amplitudes to orb brightness values   
    //orb1
    if(left[0] > 400){
      Intensity1 = (left[0])/400.0;
    }
    else {
      Intensity1 = 0.3;
    }
    Intensity1 = constrain(Intensity1, 0.0, 1.0);
    //orb2
    if(left[3] > 800){
      Intensity2 = (left[3])/800.0;
    }
    else {
      Intensity2 = 0.3;
    }
    Intensity2 = constrain(Intensity2, 0.0, 1.0);
    //orb3
    if(left[6] > 300){
      Intensity3 = (left[6])/300.0;
    }
    else {
      Intensity3 = 0.3;
    }
    Intensity3 = constrain(Intensity3, 0.0, 1.0);
    //orb4
    if(right[3] > 800){
      Intensity4 = (right[3])/800.0;
    }
    else {
      Intensity4 = 0.3;
    }
    Intensity4= constrain(Intensity4, 0.0, 1.0);
    //orb5
    if(right[6] > 300){
      Intensity5 = (right[6])/300.0;
    }
    else {
      Intensity5 = 0.3;
    }
    Intensity5 = constrain(Intensity5, 0.0, 1.0);

    //smooth brightness values for smoother lighting effect
    smoothedVal = smoothedVal + ((Intensity1 - smoothedVal)/4);
    smoothedVal2 = smoothedVal2 + ((Intensity2 - smoothedVal2)/4);
    smoothedVal3 = smoothedVal3 + ((Intensity3 - smoothedVal3)/4);
    smoothedVal4 = smoothedVal4 + ((Intensity4 - smoothedVal4)/4);
    smoothedVal5 = smoothedVal5 + ((Intensity5 - smoothedVal5)/4);
  }

  //send color data to LEDS, orb1
  for(int i = 0 ; i < 11; i++ ) {
    leds[i].r = r1 * smoothedVal; 
    leds[i].g = g1 * smoothedVal; 
    leds[i].b = b1 * smoothedVal; 
  }
  //orb2
  for(int i = 11 ; i < 18; i++ ) {
    leds[i].r = r2 * smoothedVal2; 
    leds[i].g = g2 * smoothedVal2; 
    leds[i].b = b2 * smoothedVal2; 
  }   
  //orb3
  for(int i = 18 ; i < 25; i++ ) {
    leds[i].r = r3 * smoothedVal3; 
    leds[i].g = g3 * smoothedVal3; 
    leds[i].b = b3 * smoothedVal3; 
  }  
  //orb4
  for(int i = 25 ; i < 32; i++ ) {
    leds[i].r = r4 * smoothedVal4;
    leds[i].g = g4 * smoothedVal4;
    leds[i].b = b4 * smoothedVal4;
  }  
  //orb5
  for(int i = 32 ; i < 39; i++ ) {
    leds[i].r = r5 * smoothedVal5; 
    leds[i].g = g5 * smoothedVal5; 
    leds[i].b = b5 * smoothedVal5;
  }      
  FastSPI_LED.show();//send data to LEDs via SPI interface
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(res, HIGH);
  digitalWrite(res, LOW);
  //cycle through frequency bands to read their value
  for(band=0; band <7; band++)
  {   
    digitalWrite(strobe,LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    left[band] = analogRead(0); // store left band reading
    right[band] = analogRead(1); // ... and the right
    digitalWrite(strobe,HIGH); 
  }
  delay(30);//sampling interval
}

void changeColor1(){
  //color change mechanism for standalone mode. Allows orbs to change color
  //independently, at pseudo random intervals

  //orb1
  if((millis() % (random(10000, 30000)) == 0))
  {
    Serial.println("moo1");
    chooseColor();
    r1 = rr;
    g1 = gg;
    b1 = bb;
  }
  //orb2
  if((millis() % (random(10000, 30000)) == 0))
  {
    Serial.println("moo2");
    chooseColor();
    r2 = rr;
    g2 = gg;
    b2 = bb; 
  }
  //orb3
  if((millis() % (random(10000, 30000)) == 0))
  {
    Serial.println("moo3");
    chooseColor();
    r3 = rr;
    g3 = gg;
    b3 = bb;
  }
  //orb4
  if((millis() % (random(10000, 20000)) == 0))
  {
    Serial.println("moo4");
    chooseColor();
    r4 = rr;
    g4 = gg;
    b4 = bb;
  }
  //orb5
  if((millis() % (random(10000, 30000)) == 0))
  {
    Serial.println("moo5");
    chooseColor();
    r5 = rr;
    g5  = gg;
    b5 = bb;
  }
  Serial.flush();
}    

void changeColor2(){
  //mechanism for changing colors in music mode. 
  //counts amplitude spikes on each orb and changes color every 100
  if(left[0] > 400){
    ++orb1;
    if(orb1 == 100){      
      Serial.println("moo1");
      chooseColor();
      r1 = rr;
      g1 = gg;
      b1 = bb;
      orb1 = 0;
    }  
  }

  if(left[3] > 800){
    ++orb2;
    if(orb2 == 100){      
      Serial.println("moo2");
      chooseColor();
      r2= rr;
      g2 = gg;
      b2 = bb;
      orb2 = 0;
    }  
  }

  if(left[6] > 300){
    ++orb3;
    if(orb3 == 100){      
      Serial.println("moo3");
      chooseColor();
      r3 = rr;
      g3 = gg;
      b3 = bb;
      orb3 = 0;
    }  
  }

  if(right[3] > 800){
    ++orb4;
    if(orb4 == 100){      
      Serial.println("moo4");
      chooseColor();
      r4 = rr;
      g4 = gg;
      b4 = bb;
      orb4 = 0;
    }  
  }

  if(right[6] > 300){
    ++orb5;
    if(orb5 == 100){      
      Serial.println("moo4");
      chooseColor();
      r5 = rr;
      g5 = gg;
      b5 = bb;
      orb5 = 0;
    }  
  }
}

void chooseColor(){
  //randomly assign color from pallete of 7 hues
  int color1 = random(8);
  switch(color1)
  {
    //red
  case 1:
    rr = 200;
    gg = 0;
    bb = 0;
    break;
    //green
  case 2:
    rr = 0;
    gg = 200;
    bb = 0;
    break;
    //blue
  case 3:
    rr = 0;
    gg = 0;
    bb = 200;
    break;
    //orange
  case 4:
    rr = 150;
    gg = 50;
    bb = 0;
    break;
    //turquoise
  case 5:
    rr = 0;
    gg = 150;
    bb = 50;
    break;
    //pink
  case 6:
    rr = 150;
    gg = 0;
    bb = 50;
    break; 
    //purple
  case 7:
    rr = 50;
    gg = 0;
    bb = 150;
    break; 
  }
}



