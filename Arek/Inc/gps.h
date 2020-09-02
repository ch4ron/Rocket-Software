#ifndef GPS_H_
#define GPS_H_

// ---------------->> INCLUDES:
#include "main.h"
#include "config.h"
// ----------------<< INCLUDES

// ---------------->> DEFINES:
// Nazewnictwo:MODU�_PERYFERIUM_NAZWA
#define GPS_UART huart1

// ----------------<< DEFINES

#define GPS_PVT_valid_validDate_bm    (1 << 0)
#define GPS_PVT_valid_validTime_bm    (1 << 1)
#define GPS_PVT_valid_fullyResolved_bm    (1 << 2)
#define GPS_PVT_valid_validMag_bm    (1 << 3)

#define GPS_PVT_flags_gnssFixOK_bm    (1 << 0)
#define GPS_PVT_flags_diffSoln_bm     (1 << 1)
#define GPS_PVT_flags_headVehValid_bm (1 << 5)
#define GPS_PVT_falgs_carrSoln_bm     ((1 << 6) | (1 << 6))

#define GPS_PVT_flags2_confirmedAvai_bm    (1 << 5)
#define GPS_PVT_flags2_confirmedDate_bm    (1 << 6)
#define GPS_PVT_flags2_confirmedTime_bm    (1 << 7)

// ---------------->> OTHER:
// ----------------<< OTHER


// ---------------->> VARIABLES:
extern UART_HandleTypeDef GPS_UART;

typedef struct // UBX-NAV-PVT MESSAGE
{
	uint32_t iTOW;   		//[ms] GPS time of week of the navigation epoch.See the description of iTOW for details.
    uint16_t year;	        //[] Year (UTC)
    uint8_t month; 			//[] Month, range 1..12 (UTC)
    uint8_t day; 			//[] Day of month, range 1..31 (UTC)
    uint8_t hour;		    //[] Hour of day, range 0..23 (UTC)
    uint8_t min; 			//[] Minute of hour, range 0..59 (UTC)
    uint8_t sec; 			//[] Seconds of minute, range 0..60 (UTC)
    uint8_t valid;		    //[] Validity flags (see graphic below)
    uint32_t tAcc; 			//[ns] Time accuracy estimate (UTC)
    int32_t nano; 			//[ns] Fraction of second, range -1e9 .. 1e9 (UTC)
    uint8_t fixType; 		//[] GNSSfix Type:0: no fix 1: dead reckoning only 2: 2D-fix 3: 3D-fix 4: GNSS + dead reckoning combined5: time only fix
    uint8_t flags; 			//[] Fix status flags (see graphic below)
    uint8_t flags2; 		//[] Additional flags
    uint8_t num_of_sat; 	//[] Number of satellites used in Nav Solution
    int32_t longitude; 		//[deg*10^-7] multiply by 10^-7 to get value in deg
    int32_t latitude; 		//[deg*10^-7] multiply by 10^-7 to get value in deg
    int32_t height_elipsoid;//[mm] Height above ellipsoid
    int32_t height_sea_lvl; //[mm] in mm , divide by 1000 to get value in meters , Height above mean sea level
    uint32_t hAcc; 			//[mm] Horizontal accuracy estimate
    uint32_t vAcc; 			//[mm] Vertical accuracy estimate
    int32_t velN; 			//[mm/s] NED north velocity
    int32_t velE; 			//[mm/s] NED east velocity
    int32_t velD; 			//[mm/s] NED down velocity
    int32_t gSpeed; 		//[mm/s] Ground Speed (2-D)
    int32_t headMot; 		//[deg] Heading of motion (2-D)
    uint32_t sAcc; 			//[mm/s] Speed accuracy estimate
    uint32_t headAcc; 		//[deg] Heading accuracy estimate (both motionand vehicle)
    uint16_t pDOP; 			//[] Position DOP
    int32_t headVeh; 		//[deg] Heading of vehicle (2-D)
    int16_t magDec; 		//[deg] Magnetic declination
	uint16_t magAcc; 		//[deg] Magnetic declination accuracy
} GPS_NAV_PVT_Data_t;

typedef struct // UBX-NAV-PVT MESSAGE
{
	char latitude_designator;
	uint16_t latitude_deg;
	uint16_t latitude_min;
	uint16_t latitude_sec;

	char longitude_designator;
	uint16_t longitude_deg;
	uint16_t longitude_min;
	uint16_t longitude_sec;
} GPS_DMS_COORD_t;

// ----------------<< VARIABLES


// ---------------->> FUNC:
void GPS_init(void);
void GPS_poll_nav_pvt_msg (GPS_NAV_PVT_Data_t * GPS_Data_pointer);

GPS_DMS_COORD_t GPS_conv_raw_coord_to_dms (GPS_NAV_PVT_Data_t  GPS_Data);

uint8_t GPS_get_nav_pvt_flags2 (GPS_NAV_PVT_Data_t * GPS_Data_pointer, uint8_t flag_to_test);
uint8_t GPS_get_nav_pvt_flags (GPS_NAV_PVT_Data_t * GPS_Data_pointer, uint8_t flag_to_test);
uint8_t GPS_get_nav_pvt_valid (GPS_NAV_PVT_Data_t * GPS_Data_pointer, uint8_t flag_to_test);
// ----------------<< FUNC


#endif /* GPS_H_ */
