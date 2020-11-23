/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFM23_H__
#define __RFM23_H__

#include "main.h"
#include "stm32f4xx_hal.h"
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart5;

/////////////////////////////////////////////////////////////////////////////

#define PACKET_LENGTH 10





/////////////////////////////////////////////////////////////////////////////


// Register names
#define RH_RF22_REG_00_DEVICE_TYPE                         0x00
#define RH_RF22_REG_01_VERSION_CODE                        0x01
#define RH_RF22_REG_02_DEVICE_STATUS                       0x02
#define RH_RF22_REG_03_INTERRUPT_STATUS1                   0x03
#define RH_RF22_REG_04_INTERRUPT_STATUS2                   0x04
#define RH_RF22_REG_05_INTERRUPT_ENABLE1                   0x05
#define RH_RF22_REG_06_INTERRUPT_ENABLE2                   0x06
#define RH_RF22_REG_07_OPERATING_MODE1                     0x07
#define RH_RF22_REG_08_OPERATING_MODE2                     0x08
#define RH_RF22_REG_09_OSCILLATOR_LOAD_CAPACITANCE         0x09
#define RH_RF22_REG_0A_UC_OUTPUT_CLOCK                     0x0a
#define RH_RF22_REG_0B_GPIO_CONFIGURATION0                 0x0b
#define RH_RF22_REG_0C_GPIO_CONFIGURATION1                 0x0c
#define RH_RF22_REG_0D_GPIO_CONFIGURATION2                 0x0d
#define RH_RF22_REG_0E_IO_PORT_CONFIGURATION               0x0e
#define RH_RF22_REG_0F_ADC_CONFIGURATION                   0x0f
#define RH_RF22_REG_10_ADC_SENSOR_AMP_OFFSET               0x10
#define RH_RF22_REG_11_ADC_VALUE                           0x11
#define RH_RF22_REG_12_TEMPERATURE_SENSOR_CALIBRATION      0x12
#define RH_RF22_REG_13_TEMPERATURE_VALUE_OFFSET            0x13
#define RH_RF22_REG_14_WAKEUP_TIMER_PERIOD1                0x14
#define RH_RF22_REG_15_WAKEUP_TIMER_PERIOD2                0x15
#define RH_RF22_REG_16_WAKEUP_TIMER_PERIOD3                0x16
#define RH_RF22_REG_17_WAKEUP_TIMER_VALUE1                 0x17
#define RH_RF22_REG_18_WAKEUP_TIMER_VALUE2                 0x18
#define RH_RF22_REG_19_LDC_MODE_DURATION                   0x19
#define RH_RF22_REG_1A_LOW_BATTERY_DETECTOR_THRESHOLD      0x1a
#define RH_RF22_REG_1B_BATTERY_VOLTAGE_LEVEL               0x1b
#define RH_RF22_REG_1C_IF_FILTER_BANDWIDTH                 0x1c
#define RH_RF22_REG_1D_AFC_LOOP_GEARSHIFT_OVERRIDE         0x1d
#define RH_RF22_REG_1E_AFC_TIMING_CONTROL                  0x1e
#define RH_RF22_REG_1F_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE   0x1f
#define RH_RF22_REG_20_CLOCK_RECOVERY_OVERSAMPLING_RATE    0x20
#define RH_RF22_REG_21_CLOCK_RECOVERY_OFFSET2              0x21
#define RH_RF22_REG_22_CLOCK_RECOVERY_OFFSET1              0x22
#define RH_RF22_REG_23_CLOCK_RECOVERY_OFFSET0              0x23
#define RH_RF22_REG_24_CLOCK_RECOVERY_TIMING_LOOP_GAIN1    0x24
#define RH_RF22_REG_25_CLOCK_RECOVERY_TIMING_LOOP_GAIN0    0x25
#define RH_RF22_REG_26_RSSI                                0x26
#define RH_RF22_REG_27_RSSI_THRESHOLD                      0x27
#define RH_RF22_REG_28_ANTENNA_DIVERSITY1                  0x28
#define RH_RF22_REG_29_ANTENNA_DIVERSITY2                  0x29
#define RH_RF22_REG_2A_AFC_LIMITER                         0x2a
#define RH_RF22_REG_2B_AFC_CORRECTION_READ                 0x2b
#define RH_RF22_REG_2C_OOK_COUNTER_VALUE_1                 0x2c
#define RH_RF22_REG_2D_OOK_COUNTER_VALUE_2                 0x2d
#define RH_RF22_REG_2E_SLICER_PEAK_HOLD                    0x2e
#define RH_RF22_REG_30_DATA_ACCESS_CONTROL                 0x30
#define RH_RF22_REG_31_EZMAC_STATUS                        0x31
#define RH_RF22_REG_32_HEADER_CONTROL1                     0x32
#define RH_RF22_REG_33_HEADER_CONTROL2                     0x33
#define RH_RF22_REG_34_PREAMBLE_LENGTH                     0x34
#define RH_RF22_REG_35_PREAMBLE_DETECTION_CONTROL1         0x35
#define RH_RF22_REG_36_SYNC_WORD3                          0x36
#define RH_RF22_REG_37_SYNC_WORD2                          0x37
#define RH_RF22_REG_38_SYNC_WORD1                          0x38
#define RH_RF22_REG_39_SYNC_WORD0                          0x39
#define RH_RF22_REG_3A_TRANSMIT_HEADER3                    0x3a
#define RH_RF22_REG_3B_TRANSMIT_HEADER2                    0x3b
#define RH_RF22_REG_3C_TRANSMIT_HEADER1                    0x3c
#define RH_RF22_REG_3D_TRANSMIT_HEADER0                    0x3d
#define RH_RF22_REG_3E_PACKET_LENGTH                       0x3e
#define RH_RF22_REG_3F_CHECK_HEADER3                       0x3f
#define RH_RF22_REG_40_CHECK_HEADER2                       0x40
#define RH_RF22_REG_41_CHECK_HEADER1                       0x41
#define RH_RF22_REG_42_CHECK_HEADER0                       0x42
#define RH_RF22_REG_43_HEADER_ENABLE3                      0x43
#define RH_RF22_REG_44_HEADER_ENABLE2                      0x44
#define RH_RF22_REG_45_HEADER_ENABLE1                      0x45
#define RH_RF22_REG_46_HEADER_ENABLE0                      0x46
#define RH_RF22_REG_47_RECEIVED_HEADER3                    0x47
#define RH_RF22_REG_48_RECEIVED_HEADER2                    0x48
#define RH_RF22_REG_49_RECEIVED_HEADER1                    0x49
#define RH_RF22_REG_4A_RECEIVED_HEADER0                    0x4a
#define RH_RF22_REG_4B_RECEIVED_PACKET_LENGTH              0x4b
#define RH_RF22_REG_50_ANALOG_TEST_BUS_SELECT              0x50
#define RH_RF22_REG_51_DIGITAL_TEST_BUS_SELECT             0x51
#define RH_RF22_REG_52_TX_RAMP_CONTROL                     0x52
#define RH_RF22_REG_53_PLL_TUNE_TIME                       0x53
#define RH_RF22_REG_55_CALIBRATION_CONTROL                 0x55
#define RH_RF22_REG_56_MODEM_TEST                          0x56
#define RH_RF22_REG_57_CHARGE_PUMP_TEST                    0x57
#define RH_RF22_REG_58_CHARGE_PUMP_CURRENT_TRIMMING        0x58
#define RH_RF22_REG_59_DIVIDER_CURRENT_TRIMMING            0x59
#define RH_RF22_REG_5A_VCO_CURRENT_TRIMMING                0x5a
#define RH_RF22_REG_5B_VCO_CALIBRATION                     0x5b
#define RH_RF22_REG_5C_SYNTHESIZER_TEST                    0x5c
#define RH_RF22_REG_5D_BLOCK_ENABLE_OVERRIDE1              0x5d
#define RH_RF22_REG_5E_BLOCK_ENABLE_OVERRIDE2              0x5e
#define RH_RF22_REG_5F_BLOCK_ENABLE_OVERRIDE3              0x5f
#define RH_RF22_REG_60_CHANNEL_FILTER_COEFFICIENT_ADDRESS  0x60
#define RH_RF22_REG_61_CHANNEL_FILTER_COEFFICIENT_VALUE    0x61
#define RH_RF22_REG_62_CRYSTAL_OSCILLATOR_POR_CONTROL      0x62
#define RH_RF22_REG_63_RC_OSCILLATOR_COARSE_CALIBRATION    0x63
#define RH_RF22_REG_64_RC_OSCILLATOR_FINE_CALIBRATION      0x64
#define RH_RF22_REG_65_LDO_CONTROL_OVERRIDE                0x65
#define RH_RF22_REG_66_LDO_LEVEL_SETTINGS                  0x66
#define RH_RF22_REG_67_DELTA_SIGMA_ADC_TUNING1             0x67
#define RH_RF22_REG_68_DELTA_SIGMA_ADC_TUNING2             0x68
#define RH_RF22_REG_69_AGC_OVERRIDE1                       0x69
#define RH_RF22_REG_6A_AGC_OVERRIDE2                       0x6a
#define RH_RF22_REG_6B_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS 0x6b
#define RH_RF22_REG_6C_GFSK_FIR_FILTER_COEFFICIENT_VALUE   0x6c
#define RH_RF22_REG_6D_TX_POWER                            0x6d
#define RH_RF22_REG_6E_TX_DATA_RATE1                       0x6e
#define RH_RF22_REG_6F_TX_DATA_RATE0                       0x6f
#define RH_RF22_REG_70_MODULATION_CONTROL1                 0x70
#define RH_RF22_REG_71_MODULATION_CONTROL2                 0x71
#define RH_RF22_REG_72_FREQUENCY_DEVIATION                 0x72
#define RH_RF22_REG_73_FREQUENCY_OFFSET1                   0x73
#define RH_RF22_REG_74_FREQUENCY_OFFSET2                   0x74
#define RH_RF22_REG_75_FREQUENCY_BAND_SELECT               0x75
#define RH_RF22_REG_76_NOMINAL_CARRIER_FREQUENCY1          0x76
#define RH_RF22_REG_77_NOMINAL_CARRIER_FREQUENCY0          0x77
#define RH_RF22_REG_79_FREQUENCY_HOPPING_CHANNEL_SELECT    0x79
#define RH_RF22_REG_7A_FREQUENCY_HOPPING_STEP_SIZE         0x7a
#define RH_RF22_REG_7C_TX_FIFO_CONTROL1                    0x7c
#define RH_RF22_REG_7D_TX_FIFO_CONTROL2                    0x7d
#define RH_RF22_REG_7E_RX_FIFO_CONTROL                     0x7e
#define RH_RF22_REG_7F_FIFO_ACCESS                         0x7f

// These register masks etc are named wherever possible
// corresponding to the bit and field names in the RF-22 Manual
// RH_RF22_REG_00_DEVICE_TYPE                      0x00
#define RH_RF22_DEVICE_TYPE_RX_TRX                 0x08
#define RH_RF22_DEVICE_TYPE_TX                     0x07

// RH_RF22_REG_02_DEVICE_STATUS                    0x02
#define RH_RF22_FFOVL                              0x80
#define RH_RF22_FFUNFL                             0x40
#define RH_RF22_RXFFEM                             0x20
#define RH_RF22_HEADERR                            0x10
#define RH_RF22_FREQERR                            0x08
#define RH_RF22_LOCKDET                            0x04
#define RH_RF22_CPS                                0x03
#define RH_RF22_CPS_IDLE                           0x00
#define RH_RF22_CPS_RX                             0x01
#define RH_RF22_CPS_TX                             0x10

// RH_RF22_REG_03_INTERRUPT_STATUS1                0x03
#define RH_RF22_IFFERROR                           0x80
#define RH_RF22_ITXFFAFULL                         0x40
#define RH_RF22_ITXFFAEM                           0x20
#define RH_RF22_IRXFFAFULL                         0x10
#define RH_RF22_IEXT                               0x08
#define RH_RF22_IPKSENT                            0x04
#define RH_RF22_IPKVALID                           0x02
#define RH_RF22_ICRCERROR                          0x01

// RH_RF22_REG_04_INTERRUPT_STATUS2                0x04
#define RH_RF22_ISWDET                             0x80
#define RH_RF22_IPREAVAL                           0x40
#define RH_RF22_IPREAINVAL                         0x20
#define RH_RF22_IRSSI                              0x10
#define RH_RF22_IWUT                               0x08
#define RH_RF22_ILBD                               0x04
#define RH_RF22_ICHIPRDY                           0x02
#define RH_RF22_IPOR                               0x01

// RH_RF22_REG_05_INTERRUPT_ENABLE1                0x05
#define RH_RF22_ENFFERR                            0x80
#define RH_RF22_ENTXFFAFULL                        0x40
#define RH_RF22_ENTXFFAEM                          0x20
#define RH_RF22_ENRXFFAFULL                        0x10
#define RH_RF22_ENEXT                              0x08
#define RH_RF22_ENPKSENT                           0x04
#define RH_RF22_ENPKVALID                          0x02
#define RH_RF22_ENCRCERROR                         0x01

// RH_RF22_REG_06_INTERRUPT_ENABLE2                0x06
#define RH_RF22_ENSWDET                            0x80
#define RH_RF22_ENPREAVAL                          0x40
#define RH_RF22_ENPREAINVAL                        0x20
#define RH_RF22_ENRSSI                             0x10
#define RH_RF22_ENWUT                              0x08
#define RH_RF22_ENLBDI                             0x04
#define RH_RF22_ENCHIPRDY                          0x02
#define RH_RF22_ENPOR                              0x01

// RH_RF22_REG_07_OPERATING_MODE                   0x07
#define RH_RF22_SWRES                              0x80
#define RH_RF22_ENLBD                              0x40
#define RH_RF22_ENWT                               0x20
#define RH_RF22_X32KSEL                            0x10
#define RH_RF22_TXON                               0x08
#define RH_RF22_RXON                               0x04
#define RH_RF22_PLLON                              0x02
#define RH_RF22_XTON                               0x01

// RH_RF22_REG_08_OPERATING_MODE2                  0x08
#define RH_RF22_ANTDIV                             0xc0
#define RH_RF22_RXMPK                              0x10
#define RH_RF22_AUTOTX                             0x08
#define RH_RF22_ENLDM                              0x04
#define RH_RF22_FFCLRRX                            0x02
#define RH_RF22_FFCLRTX                            0x01

// RH_RF22_REG_0F_ADC_CONFIGURATION                0x0f
#define RH_RF22_ADCSTART                           0x80
#define RH_RF22_ADCDONE                            0x80
#define RH_RF22_ADCSEL                             0x70
#define RH_RF22_ADCSEL_INTERNAL_TEMPERATURE_SENSOR 0x00
#define RH_RF22_ADCSEL_GPIO0_SINGLE_ENDED          0x10
#define RH_RF22_ADCSEL_GPIO1_SINGLE_ENDED          0x20
#define RH_RF22_ADCSEL_GPIO2_SINGLE_ENDED          0x30
#define RH_RF22_ADCSEL_GPIO0_GPIO1_DIFFERENTIAL    0x40
#define RH_RF22_ADCSEL_GPIO1_GPIO2_DIFFERENTIAL    0x50
#define RH_RF22_ADCSEL_GPIO0_GPIO2_DIFFERENTIAL    0x60
#define RH_RF22_ADCSEL_GND                         0x70
#define RH_RF22_ADCREF                             0x0c
#define RH_RF22_ADCREF_BANDGAP_VOLTAGE             0x00
#define RH_RF22_ADCREF_VDD_ON_3                    0x08
#define RH_RF22_ADCREF_VDD_ON_2                    0x0c
#define RH_RF22_ADCGAIN                            0x03

// RH_RF22_REG_10_ADC_SENSOR_AMP_OFFSET            0x10
#define RH_RF22_ADCOFFS                            0x0f

// RH_RF22_REG_12_TEMPERATURE_SENSOR_CALIBRATION   0x12
#define RH_RF22_TSRANGE                            0xc0
#define RH_RF22_TSRANGE_M64_64C                    0x00
#define RH_RF22_TSRANGE_M64_192C                   0x40
#define RH_RF22_TSRANGE_0_128C                     0x80
#define RH_RF22_TSRANGE_M40_216F                   0xc0
#define RH_RF22_ENTSOFFS                           0x20
#define RH_RF22_ENTSTRIM                           0x10
#define RH_RF22_TSTRIM                             0x0f

// RH_RF22_REG_14_WAKEUP_TIMER_PERIOD1             0x14
#define RH_RF22_WTR                                0x3c
#define RH_RF22_WTD                                0x03

// RH_RF22_REG_1D_AFC_LOOP_GEARSHIFT_OVERRIDE      0x1d
#define RH_RF22_AFBCD                              0x80
#define RH_RF22_ENAFC                              0x40
#define RH_RF22_AFCGEARH                           0x38
#define RH_RF22_AFCGEARL                           0x07

// RH_RF22_REG_1E_AFC_TIMING_CONTROL               0x1e
#define RH_RF22_SWAIT_TIMER                        0xc0
#define RH_RF22_SHWAIT                             0x38
#define RH_RF22_ANWAIT                             0x07

// RH_RF22_REG_30_DATA_ACCESS_CONTROL              0x30
#define RH_RF22_ENPACRX                            0x80
#define RH_RF22_MSBFRST                            0x00
#define RH_RF22_LSBFRST                            0x40
#define RH_RF22_CRCHDRS                            0x00
#define RH_RF22_CRCDONLY                           0x20
#define RH_RF22_SKIP2PH                            0x10
#define RH_RF22_ENPACTX                            0x08
#define RH_RF22_ENCRC                              0x04
#define RH_RF22_CRC                                0x03
#define RH_RF22_CRC_CCITT                          0x00
#define RH_RF22_CRC_CRC_16_IBM                     0x01
#define RH_RF22_CRC_IEC_16                         0x02
#define RH_RF22_CRC_BIACHEVA                       0x03

// RH_RF22_REG_32_HEADER_CONTROL1                  0x32
#define RH_RF22_BCEN                               0xf0
#define RH_RF22_BCEN_NONE                          0x00
#define RH_RF22_BCEN_HEADER0                       0x10
#define RH_RF22_BCEN_HEADER1                       0x20
#define RH_RF22_BCEN_HEADER2                       0x40
#define RH_RF22_BCEN_HEADER3                       0x80
#define RH_RF22_HDCH                               0x0f
#define RH_RF22_HDCH_NONE                          0x00
#define RH_RF22_HDCH_HEADER0                       0x01
#define RH_RF22_HDCH_HEADER1                       0x02
#define RH_RF22_HDCH_HEADER2                       0x04
#define RH_RF22_HDCH_HEADER3                       0x08

// RH_RF22_REG_33_HEADER_CONTROL2                  0x33
#define RH_RF22_HDLEN                              0x70
#define RH_RF22_HDLEN_0                            0x00
#define RH_RF22_HDLEN_1                            0x10
#define RH_RF22_HDLEN_2                            0x20
#define RH_RF22_HDLEN_3                            0x30
#define RH_RF22_HDLEN_4                            0x40
#define RH_RF22_VARPKLEN                           0x00
#define RH_RF22_FIXPKLEN                           0x08
#define RH_RF22_SYNCLEN                            0x06
#define RH_RF22_SYNCLEN_1                          0x00
#define RH_RF22_SYNCLEN_2                          0x02
#define RH_RF22_SYNCLEN_3                          0x04
#define RH_RF22_SYNCLEN_4                          0x06
#define RH_RF22_PREALEN8                           0x01

// RH_RF22_REG_6D_TX_POWER                         0x6d
// https://www.sparkfun.com/datasheets/Wireless/General/RFM22B.pdf
#define RH_RF22_PAPEAKVAL                          0x80
#define RH_RF22_PAPEAKEN                           0x40
#define RH_RF22_PAPEAKLVL                          0x30
#define RH_RF22_PAPEAKLVL6_5                       0x00
#define RH_RF22_PAPEAKLVL7                         0x10
#define RH_RF22_PAPEAKLVL7_5                       0x20
#define RH_RF22_PAPEAKLVL8                         0x30
#define RH_RF22_LNA_SW                             0x08
#define RH_RF22_TXPOW                              0x07
#define RH_RF22_TXPOW_4X31                         0x08 // Not used in RFM22B
// For RFM22B:
#define RH_RF22_TXPOW_1DBM                         0x00
#define RH_RF22_TXPOW_2DBM                         0x01
#define RH_RF22_TXPOW_5DBM                         0x02
#define RH_RF22_TXPOW_8DBM                         0x03
#define RH_RF22_TXPOW_11DBM                        0x04
#define RH_RF22_TXPOW_14DBM                        0x05
#define RH_RF22_TXPOW_17DBM                        0x06
#define RH_RF22_TXPOW_20DBM                        0x07
// RFM23B only:
#define RH_RF22_RF23B_TXPOW_M8DBM                  0x00 // -8dBm
#define RH_RF22_RF23B_TXPOW_M5DBM                  0x01 // -5dBm
#define RH_RF22_RF23B_TXPOW_M2DBM                  0x02 // -2dBm
#define RH_RF22_RF23B_TXPOW_1DBM                   0x03 // 1dBm
#define RH_RF22_RF23B_TXPOW_4DBM                   0x04 // 4dBm
#define RH_RF22_RF23B_TXPOW_7DBM                   0x05 // 7dBm
#define RH_RF22_RF23B_TXPOW_10DBM                  0x06 // 10dBm
#define RH_RF22_RF23B_TXPOW_13DBM                  0x07 // 13dBm
// RFM23BP only:
#define RH_RF22_RF23BP_TXPOW_28DBM                 0x05 // 28dBm
#define RH_RF22_RF23BP_TXPOW_29DBM                 0x06 // 29dBm
#define RH_RF22_RF23BP_TXPOW_30DBM                 0x07 // 30dBm

// RH_RF22_REG_71_MODULATION_CONTROL2              0x71
#define RH_RF22_TRCLK                              0xc0
#define RH_RF22_TRCLK_NONE                         0x00
#define RH_RF22_TRCLK_GPIO                         0x40
#define RH_RF22_TRCLK_SDO                          0x80
#define RH_RF22_TRCLK_NIRQ                         0xc0
#define RH_RF22_DTMOD                              0x30
#define RH_RF22_DTMOD_DIRECT_GPIO                  0x00
#define RH_RF22_DTMOD_DIRECT_SDI                   0x10
#define RH_RF22_DTMOD_FIFO                         0x20
#define RH_RF22_DTMOD_PN9                          0x30
#define RH_RF22_ENINV                              0x08
#define RH_RF22_FD8                                0x04
#define RH_RF22_MODTYP                             0x30
#define RH_RF22_MODTYP_UNMODULATED                 0x00
#define RH_RF22_MODTYP_OOK                         0x01
#define RH_RF22_MODTYP_FSK                         0x02
#define RH_RF22_MODTYP_GFSK                        0x03


// RH_RF22_REG_75_FREQUENCY_BAND_SELECT            0x75
#define RH_RF22_SBSEL                              0x40
#define RH_RF22_HBSEL                              0x20
#define RH_RF22_FB                                 0x1f


void to_rx_mode(void);
uint8_t read(uint8_t a);
void write(uint8_t a, uint8_t b);
void RFM23_send(uint8_t * tx_buf);
void init_RFM22(void);
uint8_t read_inerrupt(void);
void SS_LoRa_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* __RFM23_H__ */
