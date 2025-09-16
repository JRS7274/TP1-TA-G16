# TP1-TA-G16
Este repositorio contiene el desarrollo del Trabajo Práctico N°1 de IoT de Tecnologías para la automatización
## Enunciado 
Mediante la plataforma Wokwi, diseñar un circuito con una ESP32 que simule un sistema de Control automático para un invernadero. 
### Requerimientos: 
Monitoreo de temperatura y humedad 
- Utilizar el sensor DHT22 para medir la temperatura y humedad dentro del invernadero. 
- Los valores deberán visualizarse en el OLED. Control de ventilación (temperatura) 
- El valor de referencia de temperatura se establecerá mediante el potenciómetro. 
- Si la temperatura medida supera el valor de referencia → encender un LED indicando que el sistema de ventilación se activó. 
Control de riego (humedad) 
- Al inicio del programa, el sistema deberá generar un umbral aleatorio de humedad mínima deseada en el rango [40 % : 60 %]. 
- Este valor deberá mostrarse en el OLED al iniciar el sistema.
- Si la humedad medida cae por debajo de ese umbral → encender el LED de manera intermitente (simulando riego). 
Interacción con botón 
- El botón permitirá alternar entre diferentes pantallas en el OLED: 
- Pantalla 1: Temperatura actual, referencia y estado de ventilación. 
- Pantalla 2: Humedad actual, umbral aleatorio y estado de riego. 
Puerto Serie 
- Registrar en el monitor serie todos los eventos del sistema: 
- Inicio del sistema mostrando el umbral aleatorio generado. 
- Cambios en el estado del ventilador (LED encendido/apagado). 
- Activación del riego (LED intermitente). 
### [OPCIONAL – Valor extra] 
Implementar un menú de opciones navegable desde el botón para: 
- Mostrar el estado completo del invernadero. 
- Modificar manualmente los valores de referencia (temperatura o humedad) desde el puerto Serie. 
- Forzar manualmente la activación/desactivación del sistema de ventilación o riego. 