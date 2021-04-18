/*

 ::::::::       :::      ::::    ::::   ::::::::::  :::::::::  
:+:    :+:    :+: :+:    +:+:+: :+:+:+  :+:         :+:    :+: 
+:+          +:+   +:+   +:+ +:+:+ +:+  +:+         +:+    +:+ 
:#:         +#++:++#++:  +#+  +:+  +#+  +#++:++#    +#++:++#:  
+#+   +#+#  +#+     +#+  +#+       +#+  +#+         +#+    +#+ 
#+#    #+#  #+#     #+#  #+#       #+#  #+#         #+#    #+# 
 ########   ###     ###  ###       ###  ##########  ###    ### 

:::::::::   ::::::::::  :::::::::   :::::::::    ::::::::    ::::::::   ::::     ::::  
:+:    :+:  :+:         :+:    :+:  :+:    :+:  :+:    :+:  :+:    :+:  +:+:+: :+:+:+ 
+:+    +:+  +:+         +:+    +:+  +:+    +:+  +:+    +:+  +:+    +:+  +:+ +:+:+ +:+ 
+#++:++#+   +#++:++#    +#+    +:+  +#++:++#:   +#+    +:+  +#+    +:+  +#+  +:+  +#+ 
+#+    +#+  +#+         +#+    +#+  +#+    +#+  +#+    +#+  +#+    +#+  +#+       +#+ 
#+#    #+#  #+#         #+#    #+#  #+#    #+#  #+#    #+#  #+#    #+#  #+#       #+# 
#########   ##########  #########   ###    ###   ########    ########   ###       ### 

Author: Gabriela Bezerra

This .ino controls a ws2811 led strip based on input coming from a HM-10 CC2541 bluetooth.

Sources:
https://www.tweaking4all.com/hardware/arduino/arduino-ws2812-led/
https://how2electronics.com/bluetooth-low-energy-tutorial-with-hm-10-ble-4-0-arduino/

*/

/* Color Abstraction */
typedef struct RGB {
    double r;
    double g;
     double b;
} RGB;

/* BLE Include */
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

/* LED Strip */
#include <Adafruit_NeoPixel.h>
#define PIN 6
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN, NEO_GRB + NEO_KHZ800);
int ledpin = 13;

String option = "VPW";

int light_begin = 0;

int light_mode = 0;
// 0 - only table
// 1 - all filled

int luminosity = 50; //percentage

uint32_t cc1 = strip.Color(255 * luminosity / 100, 0, 0);
uint32_t cc2 = strip.Color(0, 255 * luminosity / 100, 0);

struct RGB colorConverter(String hex, int color_init_pos) {
  
  long number = (long) strtol( &hex[color_init_pos], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println("-");
  
  struct RGB rgbColor;
  rgbColor.r = r;
  rgbColor.g = g;
  rgbColor.b = b;

  return rgbColor;
}

void setup() {

  //Setup BLE
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);

  //Setup LED Strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  
  // When Receive new Option from BLE
  if (mySerial.available()) {
    
    String ble_input = mySerial.readString();
    Serial.println("DATA RECEIVED:");
    Serial.println(ble_input);
    
    //Setting light_mode
    if (ble_input == "LM0") {
      light_mode = 0;
      clearLights();
    } else if (ble_input == "LM1") {
      light_mode = 1;
      clearLights();
    }
    
    //Setting Luminosity
    else if (ble_input == "AL") {
      luminosity = 100;
    }
    else if (ble_input.indexOf("L") == 2) {
      luminosity = ble_input.substring(0,1).toInt();
      clearLights();
    }
    
    //Setting color 1
    else if (ble_input.indexOf("1C") >= 0) {
      struct RGB color = colorConverter(ble_input, 2);
      cc1 = strip.Color(color.r * luminosity / 100, color.b * luminosity / 100, color.g * luminosity / 100);
      clearLights();
    }
    
    //Setting color 2
    else if (ble_input.indexOf("2C") >= 0) {
      struct RGB color = colorConverter(ble_input, 2);
      cc2 = strip.Color(color.r * luminosity / 100, color.b * luminosity / 100, color.g * luminosity / 100);
      clearLights();
    }
    
    else {
      option = ble_input;
    }
  }
  
  //Adjust light_begin accordinlgy to light_mode 
  switch (light_mode) {
    case 0: light_begin = (strip.numPixels() / 2 - 1); break;
    case 1: light_begin = 0; break;
  }
  
  // Option Check and enable Lights
  handleLights();
}

void handleLights() {
  
  if (option == "RB") {
    rainbow(30);
  }

  else if (option == "RBC3") {
    rainbowCycle(5);
  }
  
  else if (option == "RBC2") {
    rainbowCycle(10);
  }
  
  else if (option == "RBC1") {
    rainbowCycle(30);
  }
  
  else if (option == "RBC0") {
    rainbowCycle(60);
  }

  else if (option == "R") {
    colorWipe(strip.Color(255 * luminosity / 100, 0, 0), 50); // Red
  }

  else if (option == "O") {
    colorWipe(strip.Color(255 * luminosity / 100, 0, 50 * luminosity / 100), 50); // Orange
  }

  else if (option == "Y") {
    colorWipe(strip.Color(255 * luminosity / 100, 0, 100 * luminosity / 100), 50); // Yellow
  }

  else if (option == "G") {
    colorWipe(strip.Color(0, 0, 255 * luminosity / 100), 50); // Green
  }

  else if (option == "C") {
    colorWipe(strip.Color(0, 255 * luminosity / 100, 255 * luminosity / 100), 50); // Cyan
  }

  else if (option == "B") {
    colorWipe(strip.Color(0, 255 * luminosity / 100, 0), 50); // Blue
  }

  else if (option == "PU") {
    colorWipe(strip.Color(128 * luminosity / 100, 255 * luminosity / 100, 0), 50); // Purple
  }

  else if (option == "PI") {
    colorWipe(strip.Color(255 * luminosity / 100, 128 * luminosity / 100, 0), 50); // Pink
  }

  else if (option == "W") {
    colorWipe(strip.Color(255 * luminosity / 100, 255 * luminosity / 100, 255 * luminosity / 100), 50); // White
  }
  
  else if (option == "S0") {
    colorWipe(strip.Color(10 * luminosity / 100, 0 * luminosity / 100, 2 * luminosity / 100), 50); // Green-blue
  }
  
  else if (option == "S2") {
    colorWipe(strip.Color(10 * luminosity / 100, 20 * luminosity / 100, 0 * luminosity / 100), 50); // Purple
  }
  
  else if (option == "S1") {
    colorWipe(strip.Color(30 * luminosity / 100, 2 * luminosity / 100, 1 * luminosity / 100), 50); // Red-sunset
  }
  
  else if (option == "VPW") {
    uint32_t c1 = strip.Color(255 * luminosity / 100, 50 * luminosity / 100, 1 * luminosity / 100);
    uint32_t c2 = strip.Color(50 * luminosity / 100, 255 * luminosity / 100, 1 * luminosity / 100);
    dualLight(c1, c2, 50);
  }
  
  else if (option == "JD") {
    uint32_t c1 = strip.Color(255 * luminosity / 100, 0, 0);
    uint32_t c2 = strip.Color(0, 255 * luminosity / 100, 0);
    dualLight(c1, c2, 50);
  }
  
  else if (option == "SH") {
    uint32_t c1 = strip.Color(0, 255 * luminosity / 100, 0);
    uint32_t c2 = strip.Color(255 * luminosity / 100, 0, 0);
    dualLight(c1, c2, 50);
  }
  
  else if (option == "DC") {
    dualLight(cc1, cc2, 50);
  }
  
  else if (option.length() == 6) {
    struct RGB color = colorConverter(option, 0);
    colorWipe(strip.Color(color.r * luminosity / 100, color.b * luminosity / 100, color.g * luminosity / 100), 0);
  }

  else {
    rainbowCycle(20);
  }

}

/***********************/
/* LED Strip Functions */
/***********************/
void clearLights() {
  int i = 0;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, NULL);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = light_begin; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void dualLight(uint32_t c1, uint32_t c2, uint8_t wait) {
  for (int i = light_begin; i < strip.numPixels(); i++) {
      int c1_end = (light_begin + ((strip.numPixels() - light_begin) / 2) - 4);
      int c2_begin = (light_begin + ((strip.numPixels() - light_begin) / 2) - 5);
      
      if (i < c1_end) {
        strip.setPixelColor(i, c1);
      } else if (i > c2_begin) {
        strip.setPixelColor(i, c2);
      }
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  
  for (j = 0; j < 256; j++) {
    for (i = light_begin; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = light_begin; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
