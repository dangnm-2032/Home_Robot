/***************************************************
  HUSKYLENS An Easy-to-use AI Machine Vision Sensor
  <https://www.dfrobot.com/product-1922.html>

 ***************************************************
  This example shows the basic function of library for HUSKYLENS via I2c.

  Created 2020-03-13
  By [Angelo qiao](Angelo.qiao@dfrobot.com)

  GNU Lesser General Public License.
  See <http://www.gnu.org/licenses/> for details.
  All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
  1.Connection and Diagram can be found here
  <https://wiki.dfrobot.com/HUSKYLENS_V1.0_SKU_SEN0305_SEN0336#target_23>
  2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include "HUSKYLENS.h"
#include <Servo.h>

#define IN1 A0
#define IN2 A1
#define IN3 A2
#define IN4 A3

HUSKYLENS huskylens;
//HUSKYLENS green line >> SDA; blue line >> SCL
Servo myservo;
int pos = 90;
int cen_pos = 90;
void printResult(HUSKYLENSResult result);

unsigned long previousMillis = 0UL;
unsigned long interval = 9UL;
unsigned long currentMillis;

void forward(int spd) {
  analogWrite(IN1, spd);
  analogWrite(IN2, 0);
  analogWrite(IN3, spd);
  analogWrite(IN4, 0);
}

void backward(int spd) {
  analogWrite(IN1, 0);
  analogWrite(IN2, spd);
  analogWrite(IN3, 0);
  analogWrite(IN4, spd);
}

void tilt_left(int spd) {
  analogWrite(IN1, 100);
  analogWrite(IN2, 0);
  analogWrite(IN3, spd);
  analogWrite(IN4, 0);
}

void tilt_right(int spd) {
  analogWrite(IN1, spd);
  analogWrite(IN2, 0);
  analogWrite(IN3, 100);
  analogWrite(IN4, 0);
}

void left(int spd) {
  analogWrite(IN1, 0);
  analogWrite(IN2, spd);
  analogWrite(IN3, spd);
  analogWrite(IN4, 0);
}

void right(int spd) {
  analogWrite(IN1, spd);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, spd);
}
void stp() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  while (!huskylens.begin(Wire))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  myservo.attach(9);
}

void loop() {
  currentMillis = millis();

  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if (!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if (!huskylens.available()) {
    Serial.println(F("No block or arrow appears on the screen!"));
    stp();
  }
  else
  {
    Serial.println(F("###########"));
    while (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
  }
}

void printResult(HUSKYLENSResult result) {
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);

    //    if ((140 <= result.xCenter) && (result.xCenter <= 180) && (result.ID == 1)) {
    //      forward(255);
    //    }
    //    else if ((70 <= result.xCenter) &&
    //             (result.xCenter < 140) &&
    //             (result.ID == 1)) {
    //      tilt_left(255);
    //    }
    //    else if ((180 < result.xCenter) &&
    //             (result.xCenter <= 250) &&
    //             (result.ID == 1)) {
    //      tilt_right(255);
    //    }
    //    else if ((result.xCenter < 70) &&
    //             (result.ID == 1)) {
    //      left(255);
    //    }
    //    else if ((250 < result.xCenter) &&
    //             (result.ID == 1)) {
    //      right(255);
    //    }

    if (currentMillis - previousMillis > interval)
    {
      if (180 < result.xCenter &&
          result.ID == 1) {
        pos -= 1;
      }
      else if (result.xCenter < 140 &&
               result.ID == 1) {
        pos += 1;
      }
      pos = constrain(pos, 50, 130);
      myservo.write(pos);

      if (85 <= pos && pos <= 95) forward(255);
      else if (68 <= pos && pos < 85) tilt_right(255);
      else if (50 <= pos && pos < 68) right(255);
      else if (95 < pos && pos <= 112) tilt_left(255);
      else if (112 < pos && pos <= 130) left(255);
      previousMillis = currentMillis;
    }


  }
  else if (result.command == COMMAND_RETURN_ARROW) {
    Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
  }
  else {
    Serial.println("Object unknown!");
  }
}
