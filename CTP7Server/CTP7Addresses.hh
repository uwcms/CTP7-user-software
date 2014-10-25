#ifndef CTP7Addresses_h
#define CTP7Addresses_h

#define QPLL_RESET          0x62000000
#define QPLL_LOCK           0x62000004

#define GT_RX_RESET_EN_CXP0 0x62000014 
#define GT_RX_RESET_EN_CXP1 0x62000024
#define GT_RX_RESET_EN_CXP2 0x62000034

//Counters
#define RX_CAPTURE_ENGINE_RESET_EN     0x62000100
#define RX_CRC_ERROR_CNT_RST_EN        0x62000108
#define RX_DECODER_UNLOCKED_CNT_RST_EN 0x6200010c

#define LinkIDBase          0x62000300
#define CAPTURE             0x62000140

#define GT_TX_RESET_ENABLE_CXP0	0x62000010
#define GT_TX_RESET_DONE_CXP0	0x62000018
#define GT_RX_RESET_DONE_CXP0	0x6200001C

#define GT_TX_RESET_ENABLE_CXP1	0x62000020
#define GT_TX_RESET_DONE_CXP1	0x62000028
#define GT_RX_RESET_DONE_CXP1	0x6200002C

#define GT_TX_RESET_ENABLE_CXP2	0x62000030
#define GT_TX_RESET_DONE_CXP2	0x62000038
#define GT_RX_RESET_DONE_CXP2	0x6200003C

#define TX_PRBS_SEL             0x62000040
#define RX_PRBS_SEL             0x62000044
#define GT_LOOPBACK             0x62000048

#define RX_CAPTURE_ENGINE_RESET_EN	0x62000100
#define TX_PATTERN_ENGINE_RESET_EN	0x62000104

#define RX_CRC_ERROR_CNT_RST_EN         0x62000108

#define PATTERN_GEN_BC0_EXT_EN	0x62000110

//Rising Edge Triggers Capture
#define CAPTURE_ARM             0x62000140

#define DECODER_LOCKED_CXP0     0x62000180
#define CAPTURE_DONE_CXP0       0x62000184

#define DECODER_LOCKED_CXP1     0x62000190
#define CAPTURE_DONE_CXP1       0x62000194

#define DECODER_LOCKED_CXP2     0x620001A0
#define CAPTURE_DONE_CXP2       0x620001A4

#define MMCM_RST_EN             0x62010000
#define MMCM_LOCKED             0x62010004

#define BC0_ERR                 0x62010014
#define BC0_LOCKED              0x62010018

//Firmware Date Code (Unix Epoch Format)
#define FW_DATE_CODE            0x6201FF00
//Firmware Git Hash Code
#define FW_GIT_HASH             0x6201FF04
//Firmware Git Hash Dirty Status bit
#define FW_GIT_HASH_DIRTY       0x6201FF08


#endif
