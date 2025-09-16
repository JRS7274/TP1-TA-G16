#include "Device.h"

#define ENCODER_CLK 18
#define ENCODER_DT  5
#define ENCODER_SW  19

// ===== Estados de UI =====
enum UIState { MENU, PANTALLA, MOSTRAR };
UIState estado = MENU;

volatile int encoderValue = 0;
volatile bool moved = false;
volatile bool botonPresionado = false;  
int menuIndex = 0;                 
int pantallaActual = 0;                 
const int totalPantallas = 4;

const short LED = 23;
const short POTE = 32;

bool statusFan = false;
bool statusRiego = false;
bool inicio = false;
bool flagRiego = false;
int umbralHum = 0;

const short PIN_SENSOR = 14;
Device _device(128, 64, -1, PIN_SENSOR, DHT22);

void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  int dtState  = digitalRead(ENCODER_DT);

  if (clkState == HIGH) {
    if (dtState == LOW) {
      encoderValue++;
    } else {
      encoderValue--;
    }
    moved = true;
  }
}

void IRAM_ATTR onBoton() {
  botonPresionado = true;
}

void setup()
{
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  _device.begin();
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), onBoton, FALLING);

  randomSeed(analogRead(34)); 
  umbralHum = random(40, 61);
  _device.welcomeAnimation(umbralHum);
  inicio = true;
  Serial.print("Umbral de humedad generado: ");
  Serial.println(umbralHum);
}

void loop()
{
  if (!inicio) {
    randomSeed(analogRead(34)); 
    umbralHum = random(40, 61);  
    _device.welcomeAnimation(umbralHum);
    inicio = true;
    Serial.print("Umbral de humedad generado: ");
    Serial.println(umbralHum);
  }

  if (flagRiego) {
    digitalWrite(LED, HIGH);
    delay(400);
    digitalWrite(LED, LOW);
  }

  float temp = _device.readTemp();
  float hum  = _device.readHum();
  int valor  = analogRead(POTE);
  int umbralTempSet = map(valor, 0, 4095, -40, 80);

  if (botonPresionado) {
    botonPresionado = false;

    if (estado == MENU) {
      // Selecciona opción
      pantallaActual = menuIndex;
      estado = MOSTRAR;
    } else {
      // Vuelve al menú
      estado = MENU;
    }
    Serial.println("Botón presionado");
  }

  if (moved) {
    moved = false;  
    if (estado == MENU) {
      encoderValue = constrain(encoderValue, 0, totalPantallas - 1);
      menuIndex = encoderValue;
    }
    Serial.print("Encoder Value: ");
    Serial.println(encoderValue);
  }

  if (estado == MENU) {
    _device.showMenu(totalPantallas, menuIndex);
  } 
  else {  
    char buffer[64];
    switch (pantallaActual) {
      case 0:
      /*sprintf(buffer, "Temp: %.1f C\nRef: %d C\nVent: %s", 
                temp, umbralTempSet, statusFan ? "ON" : "OFF");
        _device.showDisplay(buffer);
        break;
      */
          _device.showTempScreen(temp, umbralTempSet, statusFan);
          break;

      case 1:
      /*sprintf(buffer, "Hum: %.1f %%\nRND: %d\nRiego: %s", 
                hum, umbralHum, statusRiego ? "ON" : "OFF");
        _device.showDisplay(buffer);
        break;
      */
        _device.showHumScreen(hum, umbralHum, statusRiego);
        break;

      case 2:
        _device.showDisplay("WIP: Ajuste umbrales\n(Serial)");
        break;

      case 3:
        _device.showDisplay("WIP: Control manual\nVent/Riego");
        break;
    }
  }

  // --- Control de ventilación ---
  if (temp >= umbralTempSet) {
    if (!statusFan) {
      Serial.println("Temperatura alta, ventilador encendido");
    }
    statusFan = true;
    if (!flagRiego) {
      digitalWrite(LED, HIGH);
    }
  } else {
    digitalWrite(LED, LOW);
    if (statusFan) {
      Serial.println("Temperatura normal, ventilador apagado");
    }
    statusFan = false;
  }

  // --- Control de riego ---
  if (hum < umbralHum) {
    flagRiego = 1;
    if (!statusRiego) {
      Serial.println("Humedad baja, comenzando riego");
    }
    statusRiego = 1;
  } else {
    flagRiego = 0;
    if (statusRiego) {
      Serial.println("Humedad normal, no es necesario riego");
    }
    statusRiego = 0;
  }
  delay(10);
}
