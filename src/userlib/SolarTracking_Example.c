//#include <stdio.h>
//#include "SolarTracking.h"
//
//int main()
//{
//	int year = 2019;
//	int month = 10;
//	int day = 19;
//	double time_zone_dec = 2;
//	double stn_lat_dec = 53.551085;
//	double stn_long_dec = 9.993682;
//	double rise_zenith = 90;
//	double fall_zenith = 90;
//	double lstdt_rise = 0;
//	double lstdt_fall = 0;
//	double zenith = 0;
//	double azimuth = 0;
//
//	printf("\n\n\n");
//
//	calculate_sunrise_and_fall(year, month, day, time_zone_dec,
//		stn_lat_dec, stn_long_dec, rise_zenith, fall_zenith,
//		&lstdt_rise, &lstdt_fall);
//
//	calculate_current_sun_position(year, month, day, lstdt_rise,
//		time_zone_dec, stn_lat_dec, stn_long_dec, &azimuth, &zenith);
//	printf("Sunrise: %lf\n", lstdt_rise);
//	printf("@ Zenith = %lf and Azimuth = %lf\n", zenith, azimuth);
//
//	calculate_current_sun_position(year, month, day, lstdt_fall,
//		time_zone_dec, stn_lat_dec, stn_long_dec, &azimuth, &zenith);
//	printf("Sunfall: %lf\n", lstdt_fall);
//	printf("@ Zenith = %lf and Azimuth = %lf\n", zenith, azimuth);
//
//	printf("\n\n\n");
//
//	return 0;
//}