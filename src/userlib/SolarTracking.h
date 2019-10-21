#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Constants rounded for 21 decimals. */
#define MY_PI 3.14159265358979323846

/* Program name : sunazzd.c
Purpose : To calculate the azimuth and zenith angles to the sun given declination, hour angle and
latitude. The zenith angle is from the centre of the earth. For some precise applications,
the routine sunzdobs should be used to correct the zenith for parallax and refraction.
See the notes in the sunzdobs routine. The azimuth returned is the so-called
astronomic azimuth (i.e. clockwise angle from the observer’s meridian). For some
applications, a correction will be required to get grid bearing (AIIG). This is discussed in
the technical report published by the Western Australian Department of Agriculture.
Author : M.L.Roderick
Western Australian Department of Agriculture
GIS Group
Date : 28-1-92
Language : ANSI Standard C
Compilation :
Limitations : None known
References :
General astronomy principles
Glassock, J.T.C. (1983). Lecture Notes - Land Surveying VI - Astronomy. Queensland Institute of
Technology, Brisbane, 1983.
Davis, R.E., Foote, F.E., Anderson, J.M. and Mikhail,E.M. (1981). Surveying, Theory and Practice.
McGraw-Hill, New York, 1981.
or any other numerical astronomy or surveying textbook.
General theory
Azimuth:
tan(Azimuth) = - sin(hour_angle)
-------------------------------------------------------
tan(decl)*cos(lat) - sin(lat)*cos(hour_angle)
where hour_angle = time since sun was on the observer’s 
meridian = UT + Long + equation_time - 180 (decimal degrees)
long = observer’s longitude
equation_time = true sun - mean sun
decl = declination of the sun
Lat = latitude of station
The hour_angle and declination are computed by the sunephemn routine, which is part of this package.
The above formula is a general solution for Azimuth of the Astronomical Triangle.
To solve for Azimuth it is best to use the atan2 function available in all ANSI standard C compilers.
Zenith distance
Note: Zenith Distance = 90 - elevation
cos(ZD) = sin(lat)*sin(decl) + cos(lat)*cos(decl)*cos(hour_angle)
This computes the “theoretical1’ Zenith Distance (zenith_calc) to the centre of the Sun from the centre of
the earth. For some applications, it will be necessary to correct the ZD (zenith_calc) to a value that would
have been observed on the earth’s surface, and affected by refraction. This is done using the sunzdobs
routine.
The routine sunzdobs (part of this package) can be used to correct the zenith distance calculated in this
routine, to derive an actual observed zenith distance.
The suns rays are refracted in the atmosphere. The actual zenith distance, is smaller than the
‘theoretical’ version. Atmospheric refraction, is a function of temperature, pressure, and the zenith
distance. It is a maximun at the horizon, (i.e. zenith = 90 deg). Typical values are:
34’ at Zenith of 90 deg.
5’ at Zenith of 80 deg.
1’ at Zenith of 45 deg.
< 1’ at Zenith less than 45 deg.
Whether you will want the theoretical (zenith_calc) or actual zenith will depend on the application. For
remote sensing studies, either will probably be sufficient, although you should use the actual value. The
difference will be minor for solar zenith angles < 70 degrees (say 2’).
For determining the sun rise/set times, the theoretical value will be appropriate. Typical zenith distances
for sun rise/set are given as 90 deg 50’ (i.e. you can see the sun 50’ below the horizon). This is made up
of 34’ refraction, and 16’ for the half-width of the sun (i.e. semi_diam_sun). The increased refraction near
the horizon,
is caused principally by the increased path length of solar radiation through the atmosphere, and
refraction of light rays through atmospheric boundary layers.
Revisions :
Version Date By Details
1.0
1.1
28-1-1992
15-02-92
mlr
mlr
The original subroutine
Removed uncertainty
calc’s and put them in a
separate routine called
sunerror.
*/
void sunazzd(double hour_angle, double sun_decl, double stn_lat_dec,
	int printdataswitch, double* azimuth, double* zenith_calc);

/* Program name : sunephem.c
Purpose : To calculate ephemeris data for the sun, given the observer’s position and date/time.
Author : M.L. Roderick
Western Australian Department of Agriculture
GIS Group.
Date : 28-1-92
Language : ANSI Standard C
Compilation :
Limitations : a) The Julian Date algorithm used (Montenbruck, p33-34) is suitable for all dates after
15-10-1582.
b) The ephemeris data for the sun is taken from “The Astronomical Almanac”. These
formula are approximate only for the period 1950-2050. After the year 2050,
please consult the almanac for updated formula.
c) The ephemeris data will predict the for the sun is taken from “The equation_time to
+- 0.1 minutes declination to +- 0.01 degrees (The Astronomic Almanac). The
accuracy of azimuth/zenith obtainable from these is discussed in a technical
bulletin by the WA Department of Agriculture.
References :
For the algorithm to compute Julian Dates;
Montenbruck,0. (1989). Practical Ephemeris Calculations Springer-Verlag, Berlin, 1989.
For the ephemeris formula;
Anon., The Astronomical Almanac , Nautical Almanac Office United States Naval Observatory and Her
Majesty’s Nautical Almanac Office Royal Greenwich Observatory, US Government Printing Office and
Her Majesty’s Stationery Office, (for the year 1984).
For data to cheek the ephemeris data;
Anon., The Star Almanac For Land Surveyors, Her Majesty’s Nautical Almanac Office, Her Majesty’s
Stationery Office, (for the year 1973).
General Astronomy Principles
Glassock, J.T.C. (1983). Lecture Notes - Land Surveying VI -Astronomy. Queensland Institute of
Technology, Brisbane, 1983.
Davis, R.E., Foote, F.E., Anderson, J.M. and Mikhail, E.M. (1981). Surveying, Theory and Practice.
McGraw-Hill, New York, 1981.
or any other numerical astronomy or surveying textbook.
Revisions :
Version Date By Details
1.0 28-1-1992 mlr The original subroutine
*/
void sunephem(int year, int month, int day, double lstdt_dec, double time_zone_dec, double stn_lat_dec,
	double stn_long_dec, int printdataswitch, double* decl, double* equation_time, double
	* hour_angle, double* sun_dist_au, double* sun_semi_diam);
 
/* Program name : sunprezd.c
Purpose : To calculate the Local standard time that a particular solar zenith distance
occurs. The user is able to set either the morning or afternoon time. This is
useful for computing sun rise/set times and also length of daylight for
agricultural applications.
Author : M.L. Roderick
Western Australian Department of Agriculture
GIS Group.
Date : 29-1-92
Language : ANSI Standard C
Compilation :
Limitations : a) The program calls the sunazzd and sunephem routines, which are part of
this package. Limitations for these routines will apply.
References :
See notes in sunazzd.c and sunephem.c.
General astronomy principles
Glassock, J.T.C. (1983). Lecture Notes - Land Surveying VI -Astronomy. Queensland
Institute of Technology, Brisbane, 1983.
Davis, R.E., Foote, F.E., Anderson, J.M. and Mikhail, E.M. (1981). Surveying, Theory and
Practice. McGraw-HiLL, New York, 1981.
or any other numerical astronomy or surveying textbook.
General theory
This routine finds the time that a particular zenith distance occurrs. The particular zenith
distance occurrs twice on each day (e.g. sun rise/set) and the user must nominate which
solution is sought (using the sun_pos variable).
The formula to find zenith distance is given by:
cos(ZD) = sin(Lat)*sin(decl) + cos(lat)*cos(decl)*cos(hour_angLe)
Note: Zenith Distance = 90 - elevation
and lat = observer’s latitude
dccl = sun’s declination
hour_angle = hour angle of the sun (west of the meridian is +ve).
= UT + Long + ET - 180
where UT = universal time
= Local Standard Time - Time Zone
Long = observer’s longitude
ET = equation of time
= true sun - mean sun
Obviously, we need to solve the above equation for hour_angle, and hence derive local
standard time. However, both ET and declination are functions of time, so a direct solution is
not possible.
Various mathematical methods (e.g. differentiation) can be used to solve for hour_angle with
minimal computing effort. An easier method is to use ‘grunt’ and iterate using successively
better approximations until a pre set tolerance is reached.
This is shown in the steps below:
1. Using initial estimate of local std time (i.e. 6am or 6pm) use the sunephem and sunazzd
routines to derive a zenith distance (say ZD[1]) for that time.
2. ZD[1] will not equal the required zenith distance, and the difference can be used to make
an estimate of the correct Local std time by the following:
correction = (requ_zenith - ZD[1])*(sun_pos)/15
where 15 is the (very) approximate number of degrees the sun moves in 1 hour. This
gives a new local std time of:
new_lstdt_est = correction + lstdt_dec
3. If the correction is Less than some tolerance then we have reached the result. Otherwise
repeat step 1 using new_lstdt_est from step 2 above until the correction is smaller than
the tolerance set.
The scheme could be improved using calculus, by partial differentiating the equation for
cos(ZD) [see equation i above], and using this as a basis for updating each estimate. However,
the solution described above and implemented is still correct, but it may require many more
iterations to reach a solution. This is not considered a problem, as the processing is memory
based.
The required solar zenith angle input will depend on the application. Some coninon examples
include:
- Sun rise/set is considered to occurr when the true centre of the sun is at zenith 90.833
degrees (i.e. 90 deg 50’). The 50’ is made up of refraction (34’) and the sun’s semidiameter
of 16’. The routine sunephem returns a value for the Sun’s semi-diameter.
This could be used instead of the approxiate value of 16’ above. The difference between
the values will not be significant for most applications. Thus at a predicted solar zenith of
90 deg 50’, the top of the sun will appear on the horizon of a flat landscape. It is noted
that topography can make a significant difference (i.e. shorter day length, as can
abnormal atmospheric conditions (i.e. affect refraction). The effect of topography will
vary, and the day length can be either shorter or longer depending on the particular
circumstances. To model this effect would require a DTM (digital terrain model), with
viewshed analysis capabilities, and the ability to calculate solar zenith (corrected for
parallax/ refraction). This program could be used, although it may be slow if it can’t be
linked into the DTM package.
- Civil twilight is the interval between sun rise/set and when the true position of the sun is 6
degrees below the horizon (i.e. zenith = 96 degreees).
- For various agricultural applications, it may be desireable to use experimental data to
define the zenith angle when a certain amount of solar radiation (i.e. energy) is available.
For example, see:
KeisLing,T.C. (1982). Calculation of the Length of the Day. Agronomy Journal 74,
pp758-759.
The techniques noted in this routine could be used to replace
Keisling’s calculations for day length. For example, if a certain solar radiation (important
to agricultural applications) occurred at say zenith < 93 deg. Then the local standard
time for a solar zenith of 93 degrees could be calculated for morning/afternoon and day
length is given by local std time (afternoon - morning).
It is important to note that all applications above consider the ‘true position’ of the sun.
This is equivalent to the zenith_calc variable returned from the sunazzd.c routines.
Earth-Sun distance
The routine returns the mean distance to the sun (mean_earth_sun_au) in AU
(astronomical units). 1 AU = i49 597 870 000 metres. This has been included for
potential agricultural applications. It may be feasible in agricultural applications to use
the earth sun distance to get an idea of variation in radiation by the sun. The earth sun
distance is computed in routine sunephem, and in this routine, the mean earth-sun
distance is taken.
Revisions :
Version Date By Details
1.0
1.1
29-1-1992
15-2-1992
mlr
mlr
The original subroutine
Update for changes in sunazzd and sunerror
routines.
*/

void sunprezd(int year, int month, int day, double time_zone_dec, double stn_lat_dec, double
	stn_long_dec, int printdataswitch, int sun_pos, double requ_zenith, double* lstdt_dec,
	double* sun_dist_au, double* sun_semi_diam);

void calculate_sunrise_and_fall(int year, int month, int day, double time_zone_dec,
	double stn_lat_dec, double stn_long_dec, double rise_zenith,
	double fall_zenith, double* lstdt_rise, double* lstdt_fall);

void calculate_current_sun_position(int year, int month, int day, double lstdt_dec,
	double time_zone_dec, double stn_lat_dec, double stn_long_dec, double* azimuth,
	double* zenith);