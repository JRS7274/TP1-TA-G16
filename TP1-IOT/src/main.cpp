#include "Device.h"

#define ENCODER_CLK 18
#define ENCODER_DT  5
#define ENCODER_SW  19

// ===== Estados de UI =====
enum UIState { MENU, PANTALLA, EDITAR_UMBRALES };
UIState estado = MENU;

const char* menuItems[] = {
    "Ver Temperatura",
    "Ver Humedad",
    "Editar Umbrales",
    "Control Manual"
};

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
int umbralTempSet = 0;

// Variables para valores manuales
int manualUmbralTemp = -999;  // -999 indica "no configurado"
int manualUmbralHum  = -1;

const short PIN_SENSOR = 14;
Device _device(128, 64, -1, PIN_SENSOR, DHT22);

void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  int dtState  = digitalRead(ENCODER_DT);

  if (clkState == HIGH) {
    if (dtState == LOW) encoderValue++;
    else encoderValue--;
    moved = true;
  }
}

void IRAM_ATTR onBoton() {
  botonPresionado = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  _device.begin();
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), onBoton, FALLING);

  // Generar umbral de humedad inicial
  randomSeed(analogRead(34)); 
  umbralHum = random(40, 61);
  _device.welcomeAnimation(umbralHum);
  inicio = true;
  Serial.print("Umbral de humedad generado: ");
  Serial.println(umbralHum);
}

void loop() {
  if (flagRiego) {
    digitalWrite(LED, HIGH);
    delay(400);
    digitalWrite(LED, LOW);
  }

  float temp = _device.readTemp();
  float hum  = _device.readHum();

  // --- Determinar umbrales ---
  int potVal = analogRead(POTE);
  if (manualUmbralTemp != -999) umbralTempSet = manualUmbralTemp; // usar valor manual si existe
  else umbralTempSet = map(potVal, 0, 4095, -40, 80);

  int umbralHumActual = (manualUmbralHum != -1) ? manualUmbralHum : umbralHum;

  // --- Botón ---
  if (botonPresionado) {
    botonPresionado = false;
    if (estado == MENU) {
        if (menuIndex == 2) {  // Ajuste de umbrales
            estado = EDITAR_UMBRALES;
            _device.showDisplay("Ingrese umbrales\npor Serial");
            Serial.println("Ingrese nuevo umbral de TEMPERATURA (-40 a 80):");
        } else {
            pantallaActual = menuIndex;
            estado = PANTALLA;
        }
    } else {
        estado = MENU;  // Volver al menú
    }
    Serial.println("Botón presionado");
  }

  // --- Encoder ---
  if (moved) {
    moved = false;  
    if (estado == MENU) {
      encoderValue = constrain(encoderValue, 0, totalPantallas - 1);
      menuIndex = encoderValue;
    }
    Serial.print("Encoder Value: ");
    Serial.println(encoderValue);
  }

  // --- Menú ---
  if (estado == MENU) {
    _device.showMenu(totalPantallas, menuIndex, menuItems);
  } 
  else if (estado == PANTALLA) {  
    switch (pantallaActual) {
      case 0: _device.showTempScreen(temp, umbralTempSet, statusFan); break;
      case 1: _device.showHumScreen(hum, umbralHumActual, statusRiego); break;
      case 3: _device.showDisplay("WIP: Control manual\nVent/Riego"); break;
    }
  } 
  else if (estado == EDITAR_UMBRALES) {
    // --- Paso 1: Ingreso de temperatura ---
    _device.showDisplay("Ingrese umbral\nTEMPERATURA (-40 a 80):");
    while (!Serial.available()) { delay(10); }
    String inputTemp = Serial.readStringUntil('\n'); inputTemp.trim();
    int valTemp = inputTemp.toInt();

    if (valTemp >= -40 && valTemp <= 80) {
        manualUmbralTemp = valTemp;
        char msgTemp[32];
        snprintf(msgTemp, sizeof(msgTemp), "Umbral TEMP\nactualizado: %d", manualUmbralTemp);
        _device.showDisplay(msgTemp);
        Serial.println(msgTemp);
        delay(1000);
    } else {
        _device.showDisplay("Valor fuera de rango\n(-40 a 80)");
        Serial.println("Valor fuera de rango (-40 a 80)");
        delay(1000);
    }

    // --- Paso 2: Ingreso de humedad ---
    _device.showDisplay("Ingrese umbral\nHUMEDAD (0 a 100):");
    while (!Serial.available()) { delay(10); }
    String inputHum = Serial.readStringUntil('\n'); inputHum.trim();
    int valHum = inputHum.toInt();

    if (valHum >= 0 && valHum <= 100) {
        manualUmbralHum = valHum;
        char msgHum[32];
        snprintf(msgHum, sizeof(msgHum), "Umbral HUM\nactualizado: %d", manualUmbralHum);
        _device.showDisplay(msgHum);
        Serial.println(msgHum);
        delay(1000);
    } else {
        _device.showDisplay("Valor fuera de rango\n(0 a 100)");
        Serial.println("Valor fuera de rango (0 a 100)");
        delay(1000);
    }

    estado = MENU; // volver al menú
  }

  // --- Control de ventilación ---
  if (temp >= umbralTempSet) {
    if (!statusFan) Serial.println("Temperatura alta, ventilador encendido");
    statusFan = true;
    if (!flagRiego) digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
    if (statusFan) Serial.println("Temperatura normal, ventilador apagado");
    statusFan = false;
  }

  // --- Control de riego ---
  if (hum < umbralHumActual) {
    flagRiego = 1;
    if (!statusRiego) Serial.println("Humedad baja, comenzando riego");
    statusRiego = 1;
  } else {
    flagRiego = 0;
    if (statusRiego) Serial.println("Humedad normal, no es necesario riego");
    statusRiego = 0;
  }

  delay(10);
}
