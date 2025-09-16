#include "Device.h"

#define ENCODER_CLK 18
#define ENCODER_DT  5
#define ENCODER_SW  19

volatile bool botonPresionado = false;  // Se activa en la ISR
int pantallaActual = 0;                 // Estado actual de la pantalla
const int totalPantallas = 2;           // Cantidad de pantallas que vas a rotar

const short LED = 23;
const short POTE = 32;

bool statusFan = false;
bool statusRiego = false;
bool inicio = false;
bool flagRiego = false;
int umbralHum = 0;

const short PIN_SENSOR = 14;
Device _device(128, 64, -1, PIN_SENSOR, DHT22);

void IRAM_ATTR onBoton() {
  botonPresionado = true;  // Marca que hubo un evento
}

void setup()
{
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  _device.begin();
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), onBoton, FALLING);
}

void loop()
{
  if (!inicio) {
        randomSeed(analogRead(34)); 
        //randomSeed(micros());
        umbralHum = random(40, 61);  
        _device.welcomeAnimation(umbralHum);
        inicio = true;
        Serial.print("Umbral de humedad generado: ");
        Serial.println(umbralHum);
    }
    if (flagRiego)
    {
      digitalWrite(LED, HIGH);
      delay(400);
      digitalWrite(LED, LOW);
    }
  char buffer[64];
  float temp = _device.readTemp();
  float hum = _device.readHum();
  int valor = analogRead(POTE);
  int umbralTempSet = map(valor, 0, 4095, -40, 80);

  if (botonPresionado) {
      botonPresionado = false;

      pantallaActual++;
      if (pantallaActual >= totalPantallas) {
        pantallaActual = 0;
      }
      Serial.println(pantallaActual);
    }

    /*case 0:{
        //sprintf(buffer, "Temp: %.1f C\nHum: %.1f %%\nVal: %d\nRef: %d", temp, hum, valor, umbralTempSet);
        sprintf(buffer, "Temp: %.1f C\nRef: %d C\nVent: %s", temp, umbralTempSet, statusFan ? "ON" : "OFF");
        _device.showDisplay(buffer);
        break;
      }
      case 1:{
        sprintf(buffer, "Hum: %.1f C\nRND: %d %%\nRiego: %s", hum, umbralHum, statusRiego ? "ON" : "OFF");
        _device.showDisplay(buffer);
        break;
      }
    */
  switch(pantallaActual){
case 0:
    _device.showTempScreen(temp, umbralTempSet, statusFan);
    break;
  case 1:
    _device.showHumScreen(hum, umbralHum, statusRiego);
    break;
  }

  if (temp >= umbralTempSet )
    {
      if (!statusFan)
      {
        Serial.println("Temperatura alta, ventilador encendido");
      }
      statusFan = true;
      if (!flagRiego)
      {
        digitalWrite(LED, HIGH);
      }
      
    }
    else
    {
      digitalWrite(LED, LOW);
      if (statusFan)
      {
        Serial.println("Temperatura normal, ventilador apagado");
      }
      statusFan = false;
    }

  if(hum < umbralHum)
    {
      flagRiego = 1;
      if (!statusRiego)
      {
        Serial.println("Humedad baja, comenzando riego");
      }
      statusRiego = 1;
    }
    else
    {
      flagRiego = 0;
      if (statusRiego)
      {
        Serial.println("Humedad normal, no es necesario riego");
      }
      statusRiego = 0;
    }
  delay(10);
}

