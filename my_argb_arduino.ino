#include <FastLED.h>
//#include <Bounce2.h>;

#define NUM_LEDS 59
#define DATA_PIN 4
#define COOLING  30
#define SPARKING 120
#define FRAMES_PER_SECOND 24
#define CHIPSET WS2812B
#define COLOR_ORDER GRB
#define PIXEL_RANDOMIZED 4

#define MEM_ADDR_BRIGHT 0
#define MEM_ADDR_MODE 1

//ARGB
byte argb_rainbow_counter;

uint8_t BRIGHTNESS = 250;
uint8_t BRIGHTNESS_LATEST = BRIGHTNESS;
uint8_t BRIGHTNESS_LEVEVS[] = {50, 100, 150, 200, 250}; //0 .. 255

bool gReverseDirection = true;
uint8_t gHue = 0;
CRGB leds[NUM_LEDS];

volatile uint8_t argb_mode_counter = 1;
//volatile uint8_t rgb_12_mode_counter = 1;

volatile boolean btnFlag1 = false;   // флаг1
//volatile boolean btnFlag2 = false;   // флаг2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //MEMORY
  BRIGHTNESS = eeprom_read_byte(MEM_ADDR_BRIGHT);
  if (eeprom_read_byte(MEM_ADDR_MODE) > 8) { // первый запуск
    eeprom_write_byte(MEM_ADDR_MODE, argb_mode_counter);
  }
  argb_mode_counter = eeprom_read_byte(1);
  
  pinMode(13, OUTPUT); //DEBUG INTERNAL LED
  pinMode(2, INPUT_PULLUP); //Button on D2
  //pinMode(3, INPUT_PULLUP); //Button on D3

  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  
  attachInterrupt(0, Btn1, RISING);
  Serial.println("INIT OK");
}

void loop() {

     //   * * *   MAIN MODE SWITCH   * * * 
   switch(argb_mode_counter)
  {
   case 1: ArgbRainbow();break;
   case 2: ArgbSetRed();break;
   case 3: ArgbSetOrange();break;
   case 4: ArgbSetYellow();break;
   case 5: ArgbSetGreen();break;
   case 6: ArgbSetBlue();break;
   case 7: ArgbSetPurple();break;
   case 8: ArgbSetAqua();break;

  }

  //   * * *   SERIAL PORT   * * * 
  
  if (Serial.available()){
    
    int state = Serial.parseInt();
    
    if (state == 111){
      FastLED.setBrightness(BRIGHTNESS_LATEST);
      //digitalWrite(13, HIGH);
      eeprom_write_byte(MEM_ADDR_BRIGHT, BRIGHTNESS_LATEST); //MEMORY
      Serial.println("Command completed LED turned ON");
      state = 0;
      }
      
    if (state == 110){
      BRIGHTNESS_LATEST = BRIGHTNESS;
      FastLED.setBrightness(0);
      //digitalWrite(13, LOW);
      eeprom_write_byte(MEM_ADDR_BRIGHT, 0); //MEMORY
      Serial.println("Command completed LED turned OFF");
      state = 0;
      }

    if (state >= 201 && state <= 208){
      int res = state - 200;
      if (res > 0 && res <= 8) argb_mode_counter = res;
      eeprom_write_byte(MEM_ADDR_MODE, res); //MEMORY
      Serial.println("Command completed ");
      state = 0;
      }

   if (state >= 401 && state <= 405){
      int res = state - 401;
      if (res >= 0 && res <= 4) {
        BRIGHTNESS = BRIGHTNESS_LEVEVS[res];
        FastLED.setBrightness(BRIGHTNESS);
        eeprom_write_byte(MEM_ADDR_BRIGHT, BRIGHTNESS); //MEMORY
      }
      Serial.println("Command completed ");
      state = 0;
      }
   }


  //   * * *   BUTTONS   * * * 
  if (btnFlag1) {
    delay(400);
    btnFlag1 = false;    // сбрасываем
    
    // совершаем какие-то действия
    Serial.println("BTN 1");
    
    if (argb_mode_counter >= 8) argb_mode_counter = 1;    // 1 .. 8
    else argb_mode_counter++;
    
    Serial.println(argb_mode_counter);
  }
}



void Btn1() {
  if (millis() > 3000) btnFlag1 = true;   // подняли флаг прерывания
}


   //   * * *   ARGB MODES LOGIC   * * * 
void ArgbSetRed() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}
void ArgbSetGreen() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
}
void ArgbSetBlue() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
}
void ArgbSetOrange() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Orange;
  }
  FastLED.show();
}
void ArgbSetYellow() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();
}
void ArgbSetPurple() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Purple;
  }
  FastLED.show();
}
void ArgbSetAqua() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Aqua;
  }
  FastLED.show();
}
void ArgbRainbow() {
     for (int i = 0; i < NUM_LEDS; i++ ) {         // от 0 до первой трети
    leds[i] = CHSV(argb_rainbow_counter + i * 6, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
  argb_rainbow_counter++;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
  delay(4);         // скорость движения радуги
}
