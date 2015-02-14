#ifndef CTP7Addresses_h
#define CTP7Addresses_h

#define BadAddress                        0xDEADBEEF

#ifdef EMBED

#ifdef STAGE2
#define ILinkBaseAddress                  0x64000000
#else
#define ILinkBaseAddress                  0x61000000
#endif

#define CRCBC0ResetAddress                0x62010800
#define OLinkBaseAddress                  0x60000000
#define TCDSBufferBaseAddress             0x63000000
#define DAQBufferBaseAddress              0x65000000

#define InputLinkRegistersBaseAddress     0x62000000
#define LinkAlignmentRegistersBaseAddress 0x62010000
#define InputCaptureRegistersBaseAddress  0x62010040
#define DAQSpyCaptureRegistersBaseAddress 0x62010080
#define DAQRegistersBaseAddress           0x62010100
#define AMC13RegistersBaseAddress         0x62010140
#define TCDSRegistersBaseAddress          0x62011000
#define TCDSMonitorRegistersBaseAddress   0x62011000
#define GTHRegistersBaseAddress           0x69000000
#define QPLLRegistersBaseAddress          0x69010000
#define MiscRegistersBaseAddress          0x6201FF00

#else

#define ILinkBaseAddress                  0
#define OLinkBaseAddress                  ILinkBaseAddress+NILinks*LinkBufSize
#define DAQBufferBaseAddress              OLinkBaseAddress+NOLinks*LinkBufSize
#define TCDSBufferBaseAddress             DAQBufferBaseAddress+DAQBufferSize
#define InputLinkRegistersBaseAddress     TCDSBufferBaseAddress+TCDSBufferSize
#define LinkAlignmentRegistersBaseAddress InputLinkRegistersBaseAddress+sizeof(InputLinkRegisters)
#define InputCaptureRegistersBaseAddress  LinkAlignmentRegistersBaseAddress+sizeof(LinkAlignmentRegisters)
#define DAQSpyCaptureRegistersBaseAddress InputCaptureRegistersBaseAddress+sizeof(InputCaptureRegisters)
#define DAQRegistersBaseAddress           DAQSpyCaptureRegistersBaseAddress+sizeof(DAQSpyCaptureRegisters)
#define AMC13RegistersBaseAddress         DAQRegistersBaseAddress+sizeof(DAQRegisters)
#define TCDSRegistersBaseAddress          AMC13RegistersBaseAddress+sizeof(AMC13Registers)
#define TCDSMonitorRegistersBaseAddress   TCDSRegistersBaseAddress+sizeof(TCDSRegisters)
#define GTHRegistersBaseAddress           TCDSMonitorRegistersBaseAddress+sizeof(TCDSMonitorRegisters)
#define QPLLRegistersBaseAddress          GTHRegistersBaseAddress+sizeof(GTHRegisters)*NILinks
#define MiscRegistersBaseAddress          QPLLRegistersBaseAddress+sizeof(QPLLRegisters)*(NILinks+NOLinks)/4

#endif

#endif
