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

/* BLE related include */
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

/* LED strip include */
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

String option = "S1";

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
    option = mySerial.readString();
    Serial.println("DATA RECEIVED:");
    Serial.println(option);
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
    colorWipe(strip.Color(255, 0, 0), 50); // Red
  }

  else if (option == "O") {
    colorWipe(strip.Color(255, 0, 50), 50); // Orange
  }

  else if (option == "Y") {
    colorWipe(strip.Color(255, 0, 100), 50); // Yellow
  }

  else if (option == "G") {
    colorWipe(strip.Color(0, 0, 255), 50); // Green
  }

  else if (option == "C") {
    colorWipe(strip.Color(0, 255, 255), 50); // Cyan
  }

  else if (option == "B") {
    colorWipe(strip.Color(0, 255, 0), 50); // Blue
  }

  else if (option == "PU") {
    colorWipe(strip.Color(128, 255, 0), 50); // Purple
  }

  else if (option == "PI") {
    colorWipe(strip.Color(255, 128, 0), 50); // Pink
  }

  else if (option == "W") {
    colorWipe(strip.Color(255, 255, 255), 50); // White
  }
  
  else if (option == "S0") {
    colorWipe(strip.Color(10, 0, 2), 50); // Green-blue
  }
  
  else if (option == "S2") {
    colorWipe(strip.Color(10, 20, 0), 50); // Purple
  }
  
  else if (option == "S1") {
    colorWipe(strip.Color(30, 2, 1), 50); // Red-sunset
  }

  else {
    rainbowCycle(20);
  }

}


/* LED Strip Functions */

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
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
    for (i = 0; i < strip.numPixels(); i++) {
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
