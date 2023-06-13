#ifndef DHT22_LIB_H
#define DHT22_LIB_H

#define DHT_OK             0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR  -2

void setDHTgpio(int gpio);
void errorHandler(int response);
int readDHT();
float getHumidity();
float getTemperature();
int getSignalLevel(int usTimeOut, bool state);

#endif
