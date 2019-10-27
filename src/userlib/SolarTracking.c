#include <stdio.h>
#include "SolarTracking.h"


void sunazzd(
	double hour_angle, /* <= input hour_angle ( decimal degrees)
	+ve west, -ve east */
	double decl, /* <= input the declination to sun (decimal degrees)
	+ve North, -ve South */
	double stn_lat_dec, /* <= input latitude of stn (e.g. –27.127)
	South is –ve, North is +ve. (decimal degrees) */
	int printdataswitch, /* <= input causes the ephemeris data to be printed.
	=1 for printing, =0 for no printing */
	double* azimuth, /* => returns a pointer to azimuth (decimal degrees) */
	double* zenith_calc /* => returns a pointer to zenith_calc (the zenith)
	angle to the sun from the centre of the earth)
	in decimal degrees */
)
{
	double deg2rad, rad2deg;
	/* some useful constants */
	deg2rad = MY_PI / 180;
	rad2deg = 180 / MY_PI;
	/* convert required quantities to radians */
	hour_angle = hour_angle * deg2rad;
	decl = decl * deg2rad;
	stn_lat_dec = stn_lat_dec * deg2rad;
	/* compute azimuth */
	{
		double numerator, denominator;
		numerator = -1 * sin(hour_angle);
		denominator = (tan(decl) * cos(stn_lat_dec)) - (sin(stn_lat_dec) * cos(hour_angle));
		if (denominator == 0)
		{
			if (numerator > 0)
			{
				*azimuth = (MY_PI / 2);
			}
			else if (numerator < 0)
			{
				*azimuth = (MY_PI * 3) / 2;
			}
			else if (numerator == 0)
			{
				*azimuth = 0;
			}
		}
		else {
			*azimuth = atan2(numerator, denominator);
			if (*azimuth < 0)
			{
				*azimuth = *azimuth + (2 * MY_PI);
			}
		}
	}
	/* compute zenith distance */
	{
		double cl;
		cl = (sin(stn_lat_dec) * sin(decl)) + (cos(stn_lat_dec) * cos(decl) * cos(hour_angle));
		*zenith_calc = acos(cl);
	}
	/* convert azimuth and zenith to degrees */
	*azimuth = *azimuth * rad2deg;
	*zenith_calc = *zenith_calc * rad2deg;
	/* convert original data back to degrees for printing out */
	hour_angle = hour_angle * rad2deg;
	decl = decl * rad2deg;
	stn_lat_dec = stn_lat_dec * rad2deg;
	/* Print out summary */
	if (printdataswitch)
	{
		printf("\n\nSummary of Calculations\n");
		printf(" \n");
		printf("Input Data :\n");
		printf(" \n");
		printf("Latitude =%lf\n", stn_lat_dec);
		printf("Hour Angle (ha) = %lf\n", hour_angle);
		printf("Declination = %lf\n", decl);
		printf("Calculated Data :\n");
		printf("-------------------------\n");
		printf("Azimuth = %lf\n", *azimuth);
		printf("Zenith_calc = %lf\n", *zenith_calc);
	}
}

void sunephem
(
	int year, /* <= input year (e.g. 1986) */
	int month, /* <= input month (e.g. 3) */
	int day, /* <= input day (e.g. 12) */
	double lstdt_dec, /* <= input local std time in hrs (e.g. 14.2341)
	(decimal hrs) */
	double time_zone_dec, /* <= input time zone (e.g. Perth WST = +8.00)*/
	double stn_lat_dec, /* <= input latitude of stn (e.g. -27.127)
	South is -ve, North is +ve. (decimal deg) */
	double stn_long_dec, /* <= input longitude of stn (e.g. 120.341)
	East is +ve, West is -ve (decimal deg)*/
	int printdataswitch, /* <= input causes the ephemeris data to be printed.
	= 1 for printing , =0 for no printing. */
	double* decl, /* => returns a pointer to decl (decimal deg) */
	double* equation_time, /* => returns a pointer to equation_time (decimal deg) */
	double* hour_angle, /* => returns a pointer to hour_angle (decimal deg) */
	double* sun_dist_au, /* => returns a pointer to earth sun distance in
	AU (astronomical units) */
	double* sun_semi_diam /* => returns a pointer to sun semi-diameter
	in decimal degrees */
)
{
	double ut_dec, julian_day_dec;
	double deg2rad, rad2deg;
	double n, L, g;
	double lambda, e, ra;
	/* some useful constants */
	deg2rad = MY_PI / 180;
	rad2deg = 180 / MY_PI;
	/* check the year - if after 2050, then the almanac data will need
	to be updated from the Astronomical Almanac */
	if ((year < 1950) || (year > 2050))
	{
		printf("\n\n * **ERROR * **\n");
		printf("This program is only good between 1950 and 2O5O\n\n");
		printf("You need to update the almanac for years outside\n");
		printf("that range\n");
		printf("... Aborting …\n\n");
		return;
	}
	/* calculate the julian day - use algorithm given by
	Montenbruck p33-34. Only consider dates on or after 15-10-1582 */
	{
		long int litem1, litem2, litem3, jdyear, jdmonth;
		double ditem1, ditem2;
		/* calculate the universal time */
		ut_dec = lstdt_dec - time_zone_dec;
		/* check that date is after 15-10-1582 - I realise the test for between 1950 -> 2050 above will also get this,
		but for completeness sake it is included */
		if (year <= 1582)
		{
			if (month <= 10)
			{
				if ((day + (ut_dec / 24)) < 15)
				{
					printf(" * **Error * **can’t correctly calculate dates before\n");
					printf(" 15 - 10 - 1582\n");
					printf("... aborting …\n\n\n");
					return;
				}
			}
		}
		jdyear = year;
		jdmonth = month;
		if (month <= 2)
		{
			jdyear = year - 1;
			jdmonth = month + 12;
		}
		ditem1 = (365.25 * jdyear);
		litem1 = (long int)ditem1;
		ditem2 = 30.6001 * (jdmonth + 1);
		litem2 = (long int)ditem2;
		litem3 = ((long int)(jdyear / 400)) - ((long int)(jdyear / 100));
		julian_day_dec = litem1 + litem2 + litem3 + 1720996.5 + day + (ut_dec / 24);
	}
	/*
	Calculate ephemeris data using formula given on page C24 of "The AstronomicaL Almanac ". These formula
	have a quoted accuracy of 0.01 degrees for declination and 0.1 minutes (time) for the equation of time. For a
	full discussion on uncertainty, see the sunerror.c module. They are only to be used for the period 1950-2050
	inclusive.
	n = Current JuL ian Day - Jul ian Day on J2000.0
	L = mean Longitude of the sun corrected for aberration
	g = mean anomaLy of the sun
	lambda = ecliptic Longitude
	e = obliquity of the ecliptic
	ra = right ascension
	decl = declination
	equation_time = true sun - mean sun
	= L - ra
	To update the formula after 2050, you just update the formulas for n, L, g, Lantda, e, set out below. The rest
	should still be the same (unLess there is some cataclysm - then you won’t care anyway).
	*/
	{
		long int num_revs_360;
		n = julian_day_dec - 2451545.0;
		L = 280.460 + (0.9856474 * n);
		g = 357.528 + (0.9856003 * n);
		e = 23.439 - (0.0000004 * n);
		/* put L and g in range 0->360 degrees */
		/* L first */
		num_revs_360 = (long int)(L / 360);
		if (L < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		L = L - (num_revs_360 * 360);
		/* Now g */
		num_revs_360 = (long int)(g / 360);
		if (g < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		g = g - (num_revs_360 * 360);
		lambda = L + (1.915 * sin(g * deg2rad)) + (0.020 * sin(2 * g * deg2rad));
		/* put lambda in interval O->36O degrees */
		num_revs_360 = (long int)(lambda / 360);
		if (lambda < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		lambda = lambda - (num_revs_360 * 360);
		ra = rad2deg * (atan(cos(e * deg2rad) * tan(lambda * deg2rad)));
		*decl = rad2deg * (asin(sin(e * deg2rad) * sin(lambda * deg2rad)));
		/* put ra in same quadrant as lambda */
		/* first put ra in 0->360 degrees */
		num_revs_360 = (long int)(ra / 360);
		if (ra < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		ra = ra - (num_revs_360 * 360);
		/* Now put ra in same quadrant as lambda */
		{
			long int quadlambda, quadra;
			quadlambda = (long int)lambda / 90;
			quadra = (long int)ra / 90;
			ra = ra + (quadlambda - quadra) * 90;
		}
		*equation_time = L - ra;
		/* put equation_time in 0->360 degrees */
		num_revs_360 = (long int)(*equation_time / 360);
		if (*equation_time < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		*equation_time = *equation_time - (num_revs_360 * 360);
		*hour_angle = (15 * ut_dec) + stn_long_dec + *equation_time - 180;
		*sun_dist_au = 1.00014 - (0.01671 * cos(g * deg2rad)) - (0.00014 * cos(2 * g * deg2rad));
		*sun_semi_diam = 0.267 / (*sun_dist_au);
		/* put *hour_angle in interval 0->360 degrees */
		num_revs_360 = (long int)(*hour_angle / 360);
		if (*hour_angle < 0)
		{
			num_revs_360 = num_revs_360 - 1;
		}
		*hour_angle = *hour_angle - (num_revs_360 * 360);
	}
	/* Print out summary */
	if (printdataswitch)
	{
		printf("\n\nSumnary of Calculations\n");
		printf("------------------------------------------------------\n");
		printf("Input Data : \n");
		printf(" \n");
		printf("Date = % 2d - % 2d - % 4d\n", day, month, year);
		printf("Local Std Time = % lf\n", lstdt_dec);
		printf("Time Zone = % lf\n", time_zone_dec);
		printf("Latitude = % lf\n", stn_lat_dec);
		printf("Longitude = % lf\n", stn_long_dec);
		printf("Calculated Data : \n");
		printf("----------------------------------- \n");
		printf("JD = % lf\n", julian_day_dec);
		printf("n = % lf\n", n);
		printf(" L = % lf\n", L);
		printf("g = % lf\n", g);
		printf("Lambda = % lf\n", lambda);
		printf("e = % lf\n", e);
		printf("ra = % lf\n", ra);
		printf("decl = % lf\n", *decl);
		printf("equation_time = % lf\n", *equation_time);
		printf("hour_angle = % lf\n", *hour_angle);
		printf("Sun_dist_au = % lf\n", *sun_dist_au);
		printf("Sun_semi_diam = % lf\n", *sun_semi_diam);
	}
}

void sunprezd
(
	int year, /* <= input year (e.g. 1986) */
	int month, /* <= input month (e.g. 3) */
	int day, /* <= input day (e.g. 12) */
	double time_zone_dec, /* <= input time zone (e.g. Perth UST = +8.00) */
	double stn_lat_dec, /* <= input latitude of stn (e.g. -27.127)
	South is -ve, North is +ve. (decimal deg) */
	double stn_long_dec, /* < input longitude of stn (e.g. 120.341)
	East is +ve, West is –ve (decimal deg) */
	int printdataswitch, /* <= input causes the output data to be printed.
	= 1 for printing , =0 for no printing. */
	int sun_pos, /* <= input sun position.
	Sun rise = -1; Sunset = 1 */
	double requ_zenith, /* <= input zenith angle for which the local
	standard time is required (decimal degrees) */
	double* lstdt_dec, /* => returns a pointer to local std time in hrs
	(e.g. 5.781, 17.812 in decimal hrs) that the
	requ_zenith occurrs. */
	double* sun_dist_au, /* > returns a pointer to distance to the sun
	for the day in question.
	(in AU where 1 AU = 149 597 870 000 metres. */
	double* sun_semi_diam /* => returns a pointer to sun semi-diameter
	in decimal degrees */
)
{
	double decl, equation_time, hour_angle, sun_semi_diem;
	double azimuth, zenith_calc;
	double correction;
	double new_lstdt_est;
	double tolerance_conv;
	long int icntr;
	/* check that sun_pos = either -1 or 1 */
	if (sun_pos != -1)
	{
		if (sun_pos != 1)
		{
			printf(" * **Error * **\n");
			printf("Variable sun_pos not specified correctly in call\n");
			printf("to sunprezd routine.\n");
			printf("... aborting ... \n");
			return;
		}
	}
	/* this gives a tolerance for the iteration */
	/* the units are decimal degrees - so 0.00001 decimal degrees
	= 0.54 seconds of time. Change if you want to. As the tolerance gets smaller, the number of
	iterations required to complete the job gets larger. */
	tolerance_conv = 0.00001;
	icntr = 0;
	/* initial estimae of local standard time - if the user set sun_pos = -1 (i.e. sun rise) then the initial
	approximation is 6 hrs (local standard time). Otherwise the user set sun_pos = 1 (i.e. sun set)
	so an initial guess is made of 18 hrs local time */
	*lstdt_dec = 12 + (sun_pos * 6);
	/* set the temporary variable to be used in the loop below */
	new_lstdt_est = *lstdt_dec;
	/* the iteration loop - hold in loop until the corrections are less than the variable tolerance_conv
	*/
	do
	{
		*lstdt_dec = new_lstdt_est;
		/* use local standard time estimate to get ephemeris data */
		/* Note: this routine returns a pointer to sun_dist_au. Since sun_dist_au is already a pointer in
		the sunprezd declaration, it is not prefixed by &. */
		sunephem(year, month, day, *lstdt_dec, time_zone_dec, stn_lat_dec, stn_long_dec, printdataswitch,
			&decl, &equation_time, &hour_angle, sun_dist_au, sun_semi_diam);
		/* use ephemeris data to calculate zenith distance for that estimated local standard time */
		sunazzd(hour_angle, decl, stn_lat_dec, printdataswitch, &azimuth, &zenith_calc);
		/* the sun moves (very) approximately 180 deg in 12 hours, which is 15 deg per hour. Thus we
		update the local standard time, by dividing the difference in zenith (i.e. required - estimated) by
		15. Typcally, about 8 to 9 iterations will be required to get within a tolerance_cony = 0.00001.
		This is not a major problem as it is a memory process and not disk based. The direction for the
		correction is per the sun_pos variable (i.e. sun rise/set) */
		correction = (requ_zenith - zenith_calc) * (sun_pos) / 15;
		/* update the Local standard time with a new estimate */
		new_lstdt_est = correction + *lstdt_dec;
		/* this is a work around to get the abs() value. In many c compilers abs() takes an int only. I have
		generally found the abs() function to be unsatisfactory in different C compilers. */
		if (correction < 0)
		{
			correction = -1 * correction;
		}
		icntr++;
	} while (correction > tolerance_conv);
	/* set local standard time to our Last estimate */
	*lstdt_dec = new_lstdt_est;
	/* Print out summary */
	if (printdataswitch)
	{
		printf("\n\nSummnary of Calculations\n");
		printf("-------------------------------- - \n");
		printf("Input Data : \n");
		printf("-------------- \n");
		printf("Date = % 2d - % 2d - % 4d\n", day, month, year);
		printf("Time Zone = % lf\n", time_zone_dec);
		printf("Latitude = % lf\n", stn_lat_dec);
		printf("Longitude = % lf\n", stn_long_dec);
		printf("Sun pos = % d\n", sun_pos);
		printf("Required Zenith = % lf\n", requ_zenith);
		printf("Calculated Data : \n");
		printf("---------------------- - \n");
		printf("Earth_Sun_dist(AU) = % Lf\n", *sun_dist_au);
		printf("Local Std time = % lf\n", *lstdt_dec);
		printf("This time is for ");
		if (sun_pos == -1)
		{
			printf("morning. \n\n");
		}
		else {
			printf("afternoon. \n\n");
		}
	}
}


void calculate_sunrise_and_fall(int year, int month, int day, double time_zone_dec,
	double stn_lat_dec, double stn_long_dec, double rise_zenith,
	double fall_zenith, double* lstdt_rise, double* lstdt_fall)
{
	double sun_dist_au, sun_semi_diam;

	sunprezd(year, month, day, time_zone_dec, stn_lat_dec, stn_long_dec,
		0, -1, rise_zenith, lstdt_rise, &sun_dist_au, &sun_semi_diam);
	sunprezd(year, month, day, time_zone_dec, stn_lat_dec, stn_long_dec,
		0, 1, fall_zenith, lstdt_fall, &sun_dist_au, &sun_semi_diam);
}

void calculate_current_sun_position(int year, int month, int day, double lstdt_dec,
	double time_zone_dec, double stn_lat_dec, double stn_long_dec, double* azimuth,
	double* zenith)
{
	double decl, equation_time, hour_angle, sun_semi_diam, sun_dist_au;

	sunephem(year, month, day, lstdt_dec, time_zone_dec, stn_lat_dec, stn_long_dec, 0,
		&decl, &equation_time, &hour_angle, &sun_dist_au, &sun_semi_diam);

	sunazzd(hour_angle, decl, stn_lat_dec, 0, azimuth, zenith);
}