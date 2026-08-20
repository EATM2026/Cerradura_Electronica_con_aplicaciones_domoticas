#include "temp.h"

DHT dht(DHTPIN, DHTTYPE);

void initTempSensor() {
    dht.begin();
}

float getTemperature() {
    float temp = dht.readTemperature();

    if (isnan(temp)) {
        return -999.0; 
    }

    return temp;
}