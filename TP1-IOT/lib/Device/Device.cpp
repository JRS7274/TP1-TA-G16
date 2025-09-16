#include "Device.h"

Device ::Device(int w, int h, int reset, int pinOLED, int model) : _display(w, h, &Wire, reset),
                                                                             _sensor(pinOLED, model)
{
}

void Device::begin()
{
    _display.begin(0x3c, true);
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _sensor.begin();
}

void Device::showDisplay(const char* text)
{
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.print(text);
    _display.display();
}

void Device::showMenu(int total, int selected, const char* items[]) {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);

    for (int i = 0; i < total; i++) {
        _display.setCursor(0, i * 10); // cada línea a 10px de altura
        if (i == selected) {
            _display.print("> ");       // cursor de selección
        } else {
            _display.print("  ");
        }
        _display.print(items[i]);      // mostrar el nombre de la opción
    }

    _display.display();
}


//Animación de bienvenida
void Device::welcomeAnimation(int randomNumber) {
    _display.clearDisplay();
    const char* msg = "Control invernadero";
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 20);

    for (int i = 0; msg[i] != '\0'; i++) {
        _display.print(msg[i]);
        _display.display();
        delay(75);
    }
    delay(1000);
    const char* msg2 = "Grupo 16";
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 30);

    for (int i = 0; msg2[i] != '\0'; i++) {
        _display.print(msg2[i]);
        _display.display();
        delay(75);
    }
    delay(1000);

    _display.clearDisplay();
    const char* msge = "Umbral minimo:";
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 20);

    for (int i = 0; msge[i] != '\0'; i++) {
        _display.print(msge[i]);
        _display.display();
        delay(150);
    }
    delay(500);

    _display.setTextSize(1);
    //_display.setCursor(_display.width()/2 - 15, _display.height()/2 - 8);
    _display.print(randomNumber);

    _display.display();
    delay(1000);

    _display.clearDisplay();
    _display.display();
}

// Pantalla de Temperatura
void Device::showTempScreen(float temp, int umbralTempSet, bool statusFan) {
  _display.clearDisplay();

  _display.setTextSize(1);
  _display.setCursor(20, 0);
  _display.print("== TEMPERATURA ==");

  _display.drawLine(0, 10, 127, 10, SH110X_WHITE);

  _display.setTextSize(2);
  _display.setCursor(10, 20);
  _display.printf("%.1f C", temp);

  _display.setTextSize(1);
  _display.setCursor(10, 50);
  _display.printf("Ref: %d C", umbralTempSet);
  _display.setCursor(80, 50);
  _display.printf("Fan: %s", statusFan ? "ON" : "OFF");

  _display.display();
}

// Pantalla de Humedad
void Device::showHumScreen(float hum, int umbralHum, bool statusRiego) {
  _display.clearDisplay();

  _display.setTextSize(1);
  _display.setCursor(35, 0);
  _display.print("== HUMEDAD ==");

  _display.drawLine(0, 10, 127, 10, SH110X_WHITE);

  _display.setTextSize(2);
  _display.setCursor(10, 20);
  _display.printf("%.1f %%", hum);

  _display.setTextSize(1);
  _display.setCursor(10, 50);
  _display.printf("Umbral: %d %%", umbralHum);
  _display.setCursor(90, 50);
  _display.printf("%s", statusRiego ? "ON" : "OFF");

  _display.display();
}


float Device::readTemp()
{
    return _sensor.readTemperature();
}

float Device::readHum()
{
    return _sensor.readHumidity();
}