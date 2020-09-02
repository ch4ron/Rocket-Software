/***************************************
|-- AUTHOR: ANDRZEJ LACZEWSKI
|-- WEBSITE: WWW.BYTECHLAB.COM
|-- PROJECT: MAGELLAN-1 NANO SATELITE
|-- MODULE: APRS func
|-- DESCRIPTION:
.................
***************************************/
#include "aprs.h"

extern GPS_NAV_PVT_Data_t GPS_Data_struct;

void APRS_send(void)
{
  struct s_address addresses[] =
  {
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)

	#ifdef DIGI_PATH1
    	{DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
	#endif

	#ifdef DIGI_PATH2
		{DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
	#endif

  };

// emz: modified this to get the size of the first address rather than the size of the struct itself, which fails
  AX25_send_header(addresses, sizeof(addresses)/sizeof(addresses[0]));

  AX25_send_byte('!'); // Data type identifier: Position without timestamp (no APRS messaging), or Ultimeter 2000 WX Station

  GPS_DMS_COORD_t DMS_coordinates = GPS_conv_raw_coord_to_dms (GPS_Data_struct);

  char GPS_ERROR_STRING [25];
  if (GPS_get_nav_pvt_flags(&GPS_Data_struct,GPS_PVT_flags_gnssFixOK_bm) == TRUE)
  {
	  sprintf(GPS_ERROR_STRING,"ERR-OK");
  }
  else
  {
	  sprintf(GPS_ERROR_STRING,"ERR-1");
  }

  char GPS_coord_string [100];
  sprintf(GPS_coord_string,"%02d%02d.%02d%c%c%03d%02d.%02d%c%c %s AGH Space Systems",DMS_coordinates.latitude_deg,DMS_coordinates.latitude_min,DMS_coordinates.latitude_sec,DMS_coordinates.latitude_designator,APRS_ROCKET_SYMBOL_P1,DMS_coordinates.longitude_deg,DMS_coordinates.longitude_min,DMS_coordinates.longitude_sec,DMS_coordinates.longitude_designator,APRS_ROCKET_SYMBOL_P2,GPS_ERROR_STRING);

  AX25_send_string(GPS_coord_string);

  //AX25_send_string("------- AGH SPACE SYSTEMS - ROCKET APRS TEST - AREK --------- ITS WORKING ! ");

  AX25_send_footer();
  AX25_flush_frame();
}

