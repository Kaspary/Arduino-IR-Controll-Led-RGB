#include <IRremote.h>

#define CONTROL 8
#define LAMP 9
#define BUZZER 10
#define RED 3
#define GREEN 5
#define BLUE 6

float red = 0;
float green = 0;
float blue = 0;

boolean go_colors = false;
boolean set_color = false;

boolean color_up = false;
boolean color_down = false;
boolean color_left = false;
boolean color_right = false;

IRrecv irSensor (CONTROL);
decode_results response;

void setup() {
  Serial.begin(9600);
  irSensor.enableIRIn();
  pinMode(LAMP, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //Inicia o rele com o estado normal dele, evita que a lampada desligue quando inicia
  digitalWrite(LAMP, HIGH);
}

void loop() {
  if (irSensor.decode(&response)) {
    Serial.println(response.value, HEX);
    switch (response.value) {
      case (0xFFA25D): {
          //1 (Liga ou desliga a lampada)
          buzzer(true);
          digitalWrite(LAMP, !digitalRead(LAMP));
          break;
        }
      case (0xFF629D): {
          //2 (Inicia o modo para ajustar a cor, ou sai do mesmo)
          buzzer(!set_color);
          set_color = !set_color;
          go_colors = false;
          break;
        }
      case (0xFFE21D): {
          //3 (Inicia o loop por todas as cores, ou para o loop)
          buzzer(!go_colors);
          go_colors = !go_colors;
          set_color = false;
          break;
        }
      case (0xFF22DD): {
          //4 (Seta a cor branca)
          buzzer(true);
          go_colors = false;
          set_color = false;
          rgb(255, 255, 255);
          break;
        }
      case (0xFF02FD): {
          //5 (Desliga todos os leds)
          buzzer(false);
          go_colors = false;
          set_color = false;
          rgb(0, 0, 0);
          break;
        }
      case (0xFF38C7): {
          //OK
          break;
        }
      case (0xFFB04F): {
          //#
          buzzer(true);
          Serial.println("CURTO-CIRCUITO");
          for (int i = 0; i < 100; i++) {
            rgb(255, 0, 0);
            delay(90);
            rgb(0, 0, 255);
            delay(90);
            if (irSensor.decode(&response)) {
              // Se precionar OK sai do loop
              if (response.value == 0xFF38C7) {
                break;
              }
            }
            irSensor.resume();
          }
          break;
        }
      case (0xFF6897): {
          //* (Desliga todas as luzes - modo sleep)
          buzzer(false);
          go_colors = false;
          set_color = false;
          color_up = false;
          color_down = false;
          rgb(0, 0, 0);
          digitalWrite(LAMP, LOW);
          break;
        }
    }
    if (set_color) {
      setColor(response.value);
    }

    irSensor.resume();
  }
  if (go_colors) {
    goColors(1);
    delay(50);
  }
}

void goColors(float x) {

  if (blue == 255 && red + x <= 255 && red + x >= 0 && green == 0) {
    red += x;
  } else if (red == 255 && blue - x >= 0 && blue - x <= 255 && green == 0) {
    blue -= x;
  } else if (red == 255 && green + x <= 255 &&  green + x >= 0 && blue == 0) {
    green += x;
  } else if (green == 255 && red - x >= 0 && red - x <= 255 && blue == 0) {
    red -= x;
  } else if (green == 255 && blue + x <= 255 && blue + x >= 0 && red == 0) {
    blue += x;
  } else if (blue == 255 && green - x >= 0 && green - x <= 255 && red == 0) {
    green -= x;
  } else if (red > 0 && green > 0 && blue > 0) {
    green -= x;
  } else if (red < 255 && green < 255 && blue < 255) {
    blue += x;
  }
  rgb(red, green, blue);
  printColors();
}


void brightness(float x){
//  float current_state = 
  if (red + x <= 255 && red > 0) {
    red += x;
  }
  if (green + x <= 255 && green > 0) {
    green += x;
  }
  if (blue + x <= 255 && blue > 0) {
    blue += x;
  }
  if (red - x > 0) {
    red -= x;
  }
  if (green - x > 0) {
    green -= x;
  }
  if (blue - x > 0) {
    blue -= x;
  }
  rgb(red, green, blue);
}

void setColor(long comand) {

  float number_run = 12.75;

  if (comand == 0xFF5AA5) {
    //Direita
    color_right = true;
    color_left = false;
    color_up = false;
    color_down = false;
  } else if (comand == 0xFF10EF) {
    //Esquerda
    color_left = true;
    color_right = false;
    color_up = false;
    color_down = false;
  } else if (comand == 0xFF4AB5) {
    //Pra baixo
    color_down = true;
    color_up = false;
    color_left = false;
    color_right = false;
  } else if (comand == 0xFF18E7) {
    //Pra cima
    color_up = true;
    color_down = false;
    color_left = false;
    color_right = false;
  }

  if (color_right) {
    goColors(number_run);
  } else if (color_left) {
    goColors(-number_run);
  } else if (color_up) {
    brightness(number_run);
  } else if (color_down) {
    brightness(-number_run);
  }
}

void buzzer(boolean x){
  if(x){
    tone(BUZZER, 800, 50);
  }else{
    tone(BUZZER, 1000, 50);    
  }
}

void rgb(int r, int g, int b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

void printColors() {
  Serial.print("RED = ");
  Serial.print(red);
  Serial.print(" | GREEN = ");
  Serial.print(green);
  Serial.print(" | BLUE = ");
  Serial.print(blue);
  Serial.println("");
}
