/*
 * nmea_converter.c
 *
 *  Created on: May 12, 2020
 *      Author: markus
 */

#include "nmea_converter.h"

/**
* @brief Implements the $POV NMEA Sentence for pressure data
* @param sentence char pointer for created string
* @param static_pressure
* @param dynamic_pressure
* @param tek_pressure // not implemented yet !!
* @return result
*
* Implementation of the properitary NMEA sentence for AKF Glidecomputer
* \n
*
*     $POV,P,static_pressure,Q,dynamic_pressure*CRC
*       |  |        |        |         |
*       1  2        3        4         5
*
*     1: $P            		Properitary NMEA Sentence
*        OV         		Manufacturer Code: OpenVario
*
*     2: P               	Code for static pressure in hPa
*
*     3: static_pressure    Format: 1013.34
*                     		Range: 0..1500
*
*     4: Q                  Code for dynamic pressure in Pa
*
*     5: dynamic_pressure   Format: 12.34
* 							Range: -999 .. 999
*
*
* @date 23.02.2014 born
*
*/

bool Compose_Pressure_POV_slow(char *sentence, float static_pressure, float dynamic_pressure)
{
	int length;
	bool success = true;

	// check static_pressure input value for validity
	if ((static_pressure < 0) || (static_pressure > 2000))
	{
		static_pressure = 9999;
		success = false;
	}

	// check dynamic_pressure input value for validity
	/// @todo add useful range for dynamic_pressure vales !!
	if ((dynamic_pressure < -999.0) || (dynamic_pressure > 9998.0))
	{
		dynamic_pressure = 9999;
		success = false;
	}

	// compose NMEA String
	length = sprintf(sentence, "$POV,P,%+07.2f,Q,%+05.2f", static_pressure, (dynamic_pressure));

	// Calculate NMEA checksum and add to string
	sprintf(sentence + length, "*%02X\n", NMEA_checksum(sentence));

	return (success);
}

/**
* @brief Implements the $POV NMEA Sentence for fast data
* @param sentence char pointer for created string
* @param TE vario
* @return result
*
* Implementation of the properitary NMEA sentence for AKF Glidecomputer
* \n
*
*     $POV,E,TE_vario*CRC
*       |  |    |      |
*       1  2    3      4
*
*     1: $P            		Properitary NMEA Sentence
*        OV         		Manufacturer Code: OpenVario
*
*     2: E               	Code for TE vario in m/s
*
*     3: TE vario           Format: 3.4
*
* @date 09.03.2014 born
*
*/

int Compose_Pressure_POV_fast(char *sentence, float te_vario)
{
	int length;
	int success = 1;

	// check te_vario input value for validity
	if ((te_vario < -50) || (te_vario > 50))
	{
		te_vario = 99;
		success = 10;
	}

	// compose NMEA String
	length = sprintf(sentence, "$POV,E,%+05.2f", te_vario);

	// Calculate NMEA checksum and add to string
	sprintf(sentence + length, "*%02X\n", NMEA_checksum(sentence));

	return (success);
}

/**
* @brief Implements the $POV NMEA Sentence for voltage data
* @param sentence char pointer for created string
* @param Battery Voltage
* @return result
*
* Implementation of the properitary NMEA sentence for AKF Glidecomputer
* \n
*
*     $POV,V,Bat_voltage*CRC
*       |  |      |       |
*       1  2      3       4
*
*     1: $P            		Properitary NMEA Sentence
*        OV         		Manufacturer Code: OpenVario
*
*     2: V               	Code for Battery Voltage in V
*
*     3: Battery Voltage           Format: 12.02
*
* @date 13.03.2016 born
*
*/

int Compose_Voltage_POV(char *sentence, float voltage)
{
	int length;
	int success = 1;

	// check voltage input value for validity
	if ((voltage < 2.) || (voltage > 20.))
	{
		voltage = 0.;
		success = 10;
	}

	// compose NMEA String
	length = sprintf(sentence, "$POV,V,%+05.2f", voltage);

	// Calculate NMEA checksum and add to string
	sprintf(sentence + length, "*%02X\n", NMEA_checksum(sentence));

	return (success);
}

/**
* @brief Implements the NMEA Checksum
* @param char* NMEA string
* @return int Calculated Checksum for string
*
* The NMEA Checksum is calulated over the NMEA string between $ and *\n
* The checksum is a hexadecimal number representing the XOR of all bytes
*/

unsigned char NMEA_checksum(char *string)
{
  unsigned char value=0;
	int i=1;
	int l;

	l = strlen(string);

	for (; i < l; i++)
	{
    value ^= string[i];
  }
  return value;
}

