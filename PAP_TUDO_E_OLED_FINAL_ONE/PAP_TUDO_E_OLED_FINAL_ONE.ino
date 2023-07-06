//====================================================================
//   
//    Code Developed By Calisto                  07/06/2023 04:06
//    Project Name: Estufa Mecânizada v7.1
//    PAP 2022/2023
//    CURSO DE AUTOMAÇÃO, ELETRICIDADE, ELETRÔNICA E COMPUTADORES
//    Escola Secundária da Gafanha da Nazaré   
//           
//====================================================================


//OLED ---------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Use I2C Scanner to know the adress
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

int pushbutton_OLED_Right = 42;
int pushbutton_OLED_Left = 43;
int Dsp = 0;

//Water Sensor -------------------------------------------------------
int lowerThreshold = 480;
int upperThreshold = 500;
int WaterSensor = 3;
int WaterSensor_Pin = A0;
int WaterSensor_val = 0;
int Led_R = 48;
int Led_Y = 49;
int Led_G = 50;
int Led_B = 51;

//DHT ----------------------------------------------------------------
#include <dht.h>
dht DHT;
#define DHT22_PIN_1 7

int chk_1;

//Rlys & Fcs ---------------------------------------------------------
int pushbutton_In = 44;
int pushbutton_Out = 45;

int Rly_1_00 = 22;
int Rly_2_01 = 23;
int Fc_1_NA = 36;

int Rly_3_02 = 24;
int Rly_4_03 = 25;
int Fc_2_NA = 37;

int Rly_5_04 = 26;
int Rly_6_05 = 27;
int Fc_3_NA = 38;

int Fc_4_NA = 39;

//Variables For Protection -------------------------------------------
int x = 0;
int y = 0;
int z = 0;
int w = 0;
int p = 0;
int b = 0;
int AS_Vent = 0;
int AS_Wp = 0;
int WP_P = 0;

//OTHERS -------------------------------------------------------------
int Vent = 29;
int WP = 28;


//Millis -------------------------------------------------------------
unsigned long currentTime = 0;
unsigned long previousTime = 0;

unsigned long currentTime1 = 0;
unsigned long previousTime1 = 0;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);

  //BOTÃO ------------------------------------------------------------
  pinMode(pushbutton_In, INPUT_PULLUP);
  pinMode(pushbutton_Out, INPUT_PULLUP);

  pinMode(pushbutton_OLED_Right, INPUT_PULLUP);
  pinMode(pushbutton_OLED_Left, INPUT_PULLUP);

  //Water Sensor -----------------------------------------------------
  pinMode(Led_R, OUTPUT);
  pinMode(Led_Y, OUTPUT);
  pinMode(Led_G, OUTPUT);
  pinMode(Led_B, OUTPUT);

  //RELÉS ------------------------------------------------------------
  pinMode(Rly_1_00, OUTPUT);
  pinMode(Rly_2_01, OUTPUT);
  pinMode(Rly_3_02, OUTPUT);
  pinMode(Rly_4_03, OUTPUT);
  pinMode(Rly_5_04, OUTPUT);
  pinMode(Rly_6_05, OUTPUT);

  //FIM DE CURSO -----------------------------------------------------
  pinMode(Fc_1_NA, INPUT_PULLUP);
  pinMode(Fc_2_NA, INPUT_PULLUP);
  pinMode(Fc_3_NA, INPUT_PULLUP);
  pinMode(Fc_4_NA, INPUT_PULLUP);

  //Water Sensor -----------------------------------------------------
  pinMode(WaterSensor, OUTPUT);
  digitalWrite(WaterSensor, LOW);

  //Others -----------------------------------------------------------
  pinMode(Vent, OUTPUT);
  pinMode(WP, OUTPUT);
}


void loop() {
  //OLED -------------------------------------------------------------
  My_Oled();

  //Water Sensor -----------------------------------------------------
  int level = readSensor();

  //DHT DELAY --------------------------------------------------------
  currentTime = millis();
  if ((currentTime - previousTime) > 5000) {
    previousTime = currentTime;

    chk_1 = !chk_1;
    int chk_1 = DHT.read22(DHT22_PIN_1);
    readSensor();
  }

  //DHT EM USO -------------------------------------------------------
  if (DHT.temperature >= 26) {
    digitalWrite(Vent, HIGH);
    AS_Vent = 1;
  } if (DHT.temperature <= 22) {
    digitalWrite(Vent, LOW);
    AS_Vent = 0;
  }


  if (DHT.humidity <= 76 && digitalRead(Fc_2_NA) == LOW && (level > 0 && level <= lowerThreshold) && z == 3 && WP_P == 1) {
    digitalWrite(WP, HIGH);
    AS_Wp = 1;
  } if (DHT.humidity >= 81) {
    digitalWrite(WP, LOW);
    AS_Wp = 0;
  }



  if (DHT.humidity <= 75 && digitalRead(Fc_3_NA) == LOW && z == 1) {
    digitalWrite(Rly_5_04, HIGH);
    z = 2;
  }
  if (digitalRead(Fc_2_NA) == LOW && z == 2) {
    digitalWrite(Rly_5_04, LOW);
    z = 3;
  }
  if (DHT.humidity >= 81 && digitalRead(Fc_2_NA) == LOW && z == 3) {
    digitalWrite(Rly_6_05, HIGH);
    z = 4;
  }
  if (digitalRead(Fc_3_NA) == LOW && z == 4) {
    digitalWrite(Rly_6_05, LOW);
    z = 1;
  }




  //Colocar dentro ---------------------------------------------------
  if (digitalRead(pushbutton_In) == LOW && x == 0) {
    digitalWrite(Rly_2_01, HIGH);
    x = 1;

  }

  if (digitalRead(Fc_1_NA) == LOW && x == 1) {
    digitalWrite(Rly_2_01, LOW);
    digitalWrite(Rly_4_03, HIGH);
    x = 2;

  }
  if (digitalRead(Fc_2_NA) == LOW && x == 2) {
    digitalWrite(Rly_4_03, LOW);
    digitalWrite(Rly_6_05, HIGH);
    x = 3;

  }
  if (digitalRead(Fc_3_NA) == LOW && x == 3 && w == 0) {
    digitalWrite(Rly_6_05, LOW);
    y = 1;
    z = 1;
    w = 1;
    WP_P = 1;

  }


  //colocar fora na parte da luz -------------------------------------
  if (digitalRead(pushbutton_Out) == LOW && y == 1 && digitalRead(Fc_3_NA) == LOW) {
    digitalWrite(Rly_5_04, HIGH);
    y = 2;
    z = 0;
    WP_P = 0;

  }
  if (digitalRead(Fc_2_NA) == LOW && y == 2) {
    digitalWrite(Rly_5_04, LOW);
    digitalWrite(Rly_3_02, HIGH);
    y = 3;

  }
  if (digitalRead(Fc_1_NA) == LOW && y == 3) {
    digitalWrite(Rly_3_02, LOW);
    digitalWrite(Rly_1_00, HIGH);
    y = 4;

  }
  if (digitalRead(Fc_4_NA) == LOW && y == 4) {
    digitalWrite(Rly_1_00, LOW);
    x = 0;
    y = 0;
    w = 0;
  }

  //colocar fora na parte da rega ------------------------------------
  if (digitalRead(pushbutton_Out) == LOW && y == 1 && digitalRead(Fc_2_NA) == LOW) {
    digitalWrite(Rly_3_02, HIGH);
    digitalWrite(WP, LOW);
    z = 0;
    y = 5;
    WP_P = 0;

  } if (digitalRead(Fc_1_NA) == LOW && y == 5) {
    digitalWrite(Rly_3_02, LOW);
    digitalWrite(Rly_1_00, HIGH);
    y = 6;

  }
  if (digitalRead(Fc_4_NA) == LOW && y == 6) {
    digitalWrite(Rly_1_00, LOW);
    x = 0;
    y = 0;
    w = 0;

  }
}

//WATER SENSOR -------------------------------------------------------
int readSensor() {
  digitalWrite(WaterSensor, HIGH); 
  WaterSensor_val = analogRead(WaterSensor_Pin);   
  delay(10);
  digitalWrite(WaterSensor, LOW);   
  return WaterSensor_val;             
}

//OLED ---------------------------------------------------------------
void My_Oled() {
  int level = readSensor();

  if (digitalRead(pushbutton_OLED_Right) == LOW) {
    Dsp ++;
    delay(1000);
  }
  if (digitalRead(pushbutton_OLED_Left) == LOW) {
    Dsp --;
    delay(1000);
  }
  if (Dsp >= 6) {
    Dsp = 0;
  }

  if (Dsp <= -6) {
    Dsp = 0;
  }


  if (Dsp == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.drawRoundRect(10, 8, 110, 50, 8, WHITE);
    display.setCursor(24, 17);
    display.print("Welcome");
    display.setCursor(31, 35);
    display.print("Farmer");
    display.display();
  }

  if (Dsp == 1 || Dsp == -5) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.drawFastHLine(10, 19, 107, WHITE);
    display.drawFastHLine(36, 43, 52, WHITE);
    display.setCursor(22, 8);
    display.print("SGH Atmosphere");
    display.setCursor(13, 25);
    display.print("DHT22 TEMPERATURE");
    display.setCursor(48, 34);
    display.print(DHT.temperature);
    display.setCursor(22, 47);
    display.print("DHT22 HUMIDITY");
    display.setCursor(48, 56);
    display.print(DHT.humidity);
    display.drawRect(78, 34, 3, 3, WHITE);
    display.setCursor(79, 56);
    display.print("%");
    display.display();
  }

  else if (Dsp == 2 || Dsp == -4) {
    if (level == 0) {
      digitalWrite(Led_R, LOW);
      digitalWrite(Led_Y, LOW);
      digitalWrite(Led_G, LOW);
      digitalWrite(Led_B, HIGH);

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.setCursor(31, 8);
      display.print("Water Level");
      display.drawRoundRect(11, 25, 46, 16, 6, WHITE);
      display.drawFastHLine(14, 33, 40, WHITE);
      display.setCursor(20, 29);
      display.print("Empty");
      display.drawRoundRect(70, 25, 46, 16, 6, WHITE);
      display.setCursor(85, 29);
      display.print("Low");
      display.drawRoundRect(11, 45, 46, 16, 6, WHITE);
      display.setCursor(17, 49);
      display.print("Medium");
      display.drawRoundRect(70, 45, 46, 16, 6, WHITE);
      display.setCursor(82, 49);
      display.print("High");
      display.display();
    }   else if (level > 0 && level <= lowerThreshold) {
      digitalWrite(Led_R, HIGH);
      digitalWrite(Led_Y, LOW);
      digitalWrite(Led_G, LOW);
      digitalWrite(Led_B, LOW);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.setCursor(31, 8);
      display.print("Water Level");
      display.drawRoundRect(11, 25, 46, 16, 6, WHITE);
      display.setCursor(20, 29);
      display.print("Empty");
      display.drawRoundRect(70, 25, 46, 16, 6, WHITE);
      display.drawFastHLine(73, 33, 40, WHITE);
      display.setCursor(85, 29);
      display.print("Low");
      display.drawRoundRect(11, 45, 46, 16, 6, WHITE);
      display.setCursor(17, 49);
      display.print("Medium");
      display.drawRoundRect(70, 45, 46, 16, 6, WHITE);
      display.setCursor(82, 49);
      display.print("High");
      display.display();
    }   else if (level > lowerThreshold && level <= upperThreshold) {
      digitalWrite(Led_R, LOW);
      digitalWrite(Led_Y, HIGH);
      digitalWrite(Led_G, LOW);
      digitalWrite(Led_B, LOW);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.setCursor(31, 8);
      display.print("Water Level");
      display.drawRoundRect(11, 25, 46, 16, 6, WHITE);
      display.setCursor(20, 29);
      display.print("Empty");
      display.drawRoundRect(70, 25, 46, 16, 6, WHITE);
      display.setCursor(85, 29);
      display.print("Low");
      display.drawRoundRect(11, 45, 46, 16, 6, WHITE);
      display.drawFastHLine(15, 53, 40, WHITE);
      display.setCursor(17, 49);
      display.print("Medium");
      display.drawRoundRect(70, 45, 46, 16, 6, WHITE);
      display.setCursor(82, 49);
      display.print("High");
      display.display();
    }   else if (level > upperThreshold) {
      digitalWrite(Led_R, LOW);
      digitalWrite(Led_Y, LOW);
      digitalWrite(Led_G, HIGH);
      digitalWrite(Led_B, LOW);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.setCursor(31, 8);
      display.print("Water Level");
      display.drawRoundRect(11, 25, 46, 16, 6, WHITE);
      display.setCursor(20, 29);
      display.print("Empty");
      display.drawRoundRect(70, 25, 46, 16, 6, WHITE);
      display.setCursor(85, 29);
      display.print("Low");
      display.drawRoundRect(11, 45, 46, 16, 6, WHITE);
      display.setCursor(17, 49);
      display.print("Medium");
      display.drawRoundRect(70, 45, 46, 16, 6, WHITE);
      display.drawFastHLine(73, 53, 40, WHITE);
      display.setCursor(82, 49);
      display.print("High");
      display.display();
    }
  }

  else if (Dsp == 3 || Dsp == -3) {
    if (AS_Vent == 0 && AS_Wp == 0) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.drawFastVLine(83 , 28 , 26 , WHITE);
      display.setCursor(31, 8);
      display.print("Auto System");
      display.setCursor(10, 31);
      display.print("Ventilation");
      display.setCursor(81, 31);
      display.print("  OFF");
      display.setCursor(10, 44);
      display.print("Waterpump");
      display.setCursor(81, 44);
      display.print("  OFF");
      display.display();
    }   else if (AS_Vent == 1 && AS_Wp == 0) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.drawFastVLine(83 , 28 , 26 , WHITE);
      display.setCursor(31, 8);
      display.print("Auto System");
      display.setCursor(10, 31);
      display.print("Ventilation");
      display.setCursor(81, 31);
      display.print("  ON");
      display.setCursor(10, 44);
      display.print("Waterpump");
      display.setCursor(81, 44);
      display.print("  OFF");
      display.display();
    }   else if (AS_Vent == 0 && AS_Wp == 1) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.drawFastVLine(83 , 28 , 26 , WHITE);
      display.setCursor(31, 8);
      display.print("Auto System");
      display.setCursor(10, 31);
      display.print("Ventilation");
      display.setCursor(81, 31);
      display.print("  OFF");
      display.setCursor(10, 44);
      display.print("Waterpump");
      display.setCursor(81, 44);
      display.print("  ON");
      display.display();
    }   else if (AS_Vent == 1 && AS_Wp == 1) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.drawFastHLine(10, 19, 107, WHITE);
      display.drawFastVLine(83 , 28 , 26 , WHITE);
      display.setCursor(31, 8);
      display.print("Auto System");
      display.setCursor(10, 31);
      display.print("Ventilation");
      display.setCursor(81, 31);
      display.print("  ON");
      display.setCursor(10, 44);
      display.print("Waterpump");
      display.setCursor(81, 44);
      display.print("  ON");
      display.display();
    }
  }

  else if (Dsp == 4 || Dsp == -2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.drawFastHLine(10, 19, 107, WHITE);
    display.drawFastVLine(82 , 25 , 36 , WHITE);
    display.setCursor(3, 8);
    display.print("Motor Monitorization");
    display.setCursor(10, 27);
    display.print("Motor_One");
    display.setCursor(10, 40);
    display.print("Motor_Two");
    display.setCursor(10, 53);
    display.print("Motor_Three");
    if (digitalRead(Rly_1_00) == LOW && digitalRead(Rly_2_01) == HIGH) {
      display.setCursor(91, 27);
      display.print("RGT");
      display.display();
    }   else if (digitalRead(Rly_1_00) == HIGH && digitalRead(Rly_2_01) == LOW) {
      display.setCursor(91, 27);
      display.print("LFT");
      display.display();
    }   else if (digitalRead(Rly_1_00) == LOW && digitalRead(Rly_2_01) == LOW) {
      display.setCursor(91, 27);
      display.print("STP");
      display.display();
    }
    if (digitalRead(Rly_3_02) == LOW && digitalRead(Rly_4_03) == HIGH) {
      display.setCursor(91, 40);
      display.print("RGT");
      display.display();
    }   else if (digitalRead(Rly_3_02) == HIGH && digitalRead(Rly_4_03) == LOW) {
      display.setCursor(91, 40);
      display.print("LFT");
      display.display();
    }   else if (digitalRead(Rly_3_02) == LOW && digitalRead(Rly_4_03) == LOW) {
      display.setCursor(91, 40);
      display.print("STP");
      display.display();
    }
    if (digitalRead(Rly_5_04) == LOW && digitalRead(Rly_6_05) == HIGH) {
      display.setCursor(91, 53);
      display.print("RGT");
      display.display();
    }   else if (digitalRead(Rly_5_04) == HIGH && digitalRead(Rly_6_05) == LOW) {
      display.setCursor(91, 53);
      display.print("LFT");
      display.display();
    }   else if (digitalRead(Rly_5_04) == LOW && digitalRead(Rly_6_05) == LOW) {
      display.setCursor(91, 53);
      display.print("STP");
      display.display();
    }
    display.display();
  }

  else if (Dsp == 5 || Dsp == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.drawFastHLine(10, 19, 107, WHITE);
    display.setCursor(2, 8);
    display.print("Switch Monitorization");
    display.drawFastVLine(62, 25, 32, WHITE);
    display.setCursor(10, 29);
    display.print("Fc_1:");
    display.setCursor(69, 29);
    display.print("Fc_3:");
    display.setCursor(10, 49);
    display.print("Fc_2:");
    display.setCursor(69, 49);
    display.print("Fc_4:");
    if (digitalRead(Fc_1_NA) == LOW) {
      display.setCursor(40, 29);
      display.print("ON");
      display.display();
    } else {
      display.setCursor(40, 29);
      display.print("OFF");
      display.display();
    }   if (digitalRead(Fc_2_NA) == LOW) {
      display.setCursor(40, 49);
      display.print("ON");
      display.display();
    } else {
      display.setCursor(40, 49);
      display.print("OFF");
      display.display();
    }   if (digitalRead(Fc_3_NA) == LOW) {
      display.setCursor(99, 29);
      display.print("ON");
      display.display();
    } else {
      display.setCursor(99, 29);
      display.print("OFF");
      display.display();
    }   if (digitalRead(Fc_4_NA) == LOW) {
      display.setCursor(99, 49);
      display.print("ON");
      display.display();
    } else {
      display.setCursor(99, 49);
      display.print("OFF");
      display.display();
    }
  }

}
