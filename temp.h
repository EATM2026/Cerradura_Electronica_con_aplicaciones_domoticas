#ifndef TEMP_H
#define TEMP_H

#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

void initTempSensor();
float getTemperature();

#endif 