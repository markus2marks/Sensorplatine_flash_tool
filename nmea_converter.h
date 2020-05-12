/*
 * nmea_converter.h
 *
 *  Created on: May 12, 2020
 *      Author: markus
 */

#ifndef NMEA_CONVERTER_H_
#define NMEA_CONVERTER_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool Compose_Pressure_POV_slow(char *sentence, float static_pressure, float dynamic_pressure);
int Compose_Pressure_POV_fast(char *sentence, float te_vario);
int Compose_Voltage_POV(char *sentence, float voltage);
unsigned char NMEA_checksum(char *string);

#endif /* NMEA_CONVERTER_H_ */
