#ifndef CTP7Server_hh
#define CTP7Server_hh

// This class holds information necessary to communicate with the CTP7
// that it describes.  All access to the CTP7 is restricted to this
// class.  The base address will be exclusively mapped to the appropriate
// register and memory spaces of the Zynq.  The access to the address
// space is in terms of the offset in address from the base register and
// memory buffers.  The base address values themselves need not be
// revealed to the world.

#include <vector>
#include <stdint.h>
#include "CTP7.hh"
#include "CTP7Addresses.hh"

#ifdef EMBED
#include "libmemsvc.h"
#else

// Below exists for debugging purpose on non embedded computers
typedef unsigned int *memsvc_handle_t;
int memsvc_open(memsvc_handle_t *handle);
int memsvc_close(memsvc_handle_t *handle);
const char *memsvc_get_last_error(memsvc_handle_t handle);

#define MEMSVC_MAX_WORDS	0x3FFFFFFF

int memsvc_read(memsvc_handle_t handle, uint32_t addr, uint32_t words, uint32_t *data);
int memsvc_write(memsvc_handle_t handle, uint32_t addr, uint32_t words, const uint32_t *data);

#endif

#define PAGE_INTS 256
#define PAGE_SIZE (PAGE_INTS * 4)

#ifdef EMBED
#define ILinkBaseAddress   0x61000000
#define OLinkBaseAddress   0x60000000
#define RegBaseAddress     0x62000000
#else
#define ILinkBaseAddress 0
#define OLinkBaseAddress ILinkBaseAddress+NILinks*LinkBufSize
#define RegBaseAddress OLinkBaseAddress+NOLinks*LinkBufSize
#endif

//MaxInputLinkAddress
#define maxILinkAddress ( ILinkBaseAddress + NILinks * LinkBufSize)
#define maxOLinkAddress ( OLinkBaseAddress + NOLinks * LinkBufSize)

//Stage 2 links
#define S2ILinkBaseAddress 0x64000000
#define S2MaxILinkAddress  ( ILinkBaseAddress + S2NILinks * LinkBufSize)

class CTP7Server : public CTP7 {
    
public:
    
    CTP7Server();
    virtual ~CTP7Server();
    
   
    // Externally accessible functions to get/set on-board buffer
    bool printBuffer(unsigned int address, unsigned int numberOfValues, unsigned int * buffer);
    
    //Add a Check Link step here
    unsigned int getInputLinkAddress(unsigned int linkNumber, unsigned int startAddressOffset = 0){
        return (ILinkBaseAddress + linkNumber * LinkBufSize + startAddressOffset);
    }

    unsigned int getInputLinkAddressS2(unsigned int linkNumber, unsigned int startAddressOffset = 0){
        return (S2ILinkBaseAddress + linkNumber * LinkBufSize + startAddressOffset);
    }
    
    unsigned int getOutputLinkAddress(unsigned int linkNumber, unsigned int startAddressOffset = 0){
        return (OLinkBaseAddress + linkNumber * LinkBufSize + startAddressOffset);
    }
    
    unsigned int getAddress(BufferType bufferType,
                            unsigned int linkNumber,
                            unsigned int addressOffset);
    
    unsigned int getAddress(unsigned int addressOffset) {
        return getAddress(registerBuffer, 0, addressOffset);
    }
    
    unsigned int getRegister(unsigned int addressOffset) {
        return getAddress(addressOffset);
    }
    
    bool dumpContiguousBuffer(BufferType bufferType,
                              unsigned int linkNumber,
                              unsigned int startAddressOffset,
                              unsigned int numberOfValues,
                              unsigned int *buffer = 0);
    
    bool setAddress(BufferType bufferType,
                    unsigned int linkNumber,
                    unsigned int addressOffset,
                    unsigned int value);
    
    bool setAddress(unsigned int addressOffset,
                    unsigned int value) {
        return setAddress(registerBuffer, 0, addressOffset, value);
    }
    
    bool setRegister(unsigned int addressOffset, unsigned int value) {
        return setAddress(addressOffset, value);
    }
    
    bool setPattern(BufferType bufferType,
                    unsigned int linkNumber,
                    unsigned int nInts,
                    std::vector<unsigned int> values);
    
    bool setConstantPattern(BufferType bufferType,
                            unsigned int linkNumber,
                            unsigned int value);
    
    bool setIncreasingPattern(BufferType bufferType,
                              unsigned int linkNumber,
                              unsigned int startValue = 0,
                              unsigned int increment = 1);
    
    bool setDecreasingPattern(BufferType bufferType,
                              unsigned int linkNumber,
                              unsigned int startValue = (NIntsPerLink - 1),
                              unsigned int increment = 1);
    
    bool setRandomPattern(BufferType bufferType,
                          unsigned int linkNumber,
                          unsigned int randomSeed);
    bool capture();
    bool softReset();
    bool counterReset();

    unsigned int processTCPMessage(void *iMessage,
                                   void *oMessage,
                                   unsigned int iMaxLength,
                                   unsigned int oMaxLength,
                                   unsigned int *dataArray=0);
    
  bool fillStatusVector(std::vector<unsigned int>& vector);    
  bool fillTSStatusVector(std::vector<unsigned int>& vector);    
  void logTimeStamp();
  
protected:
    
private:
    
    // Unnecessary methods are made private
    CTP7Server(const CTP7Server&);
    const CTP7Server& operator=(const CTP7Server&);
    
    // Helper functions

    /*
     * Enumerator for possible functions:
     */
    
    enum functionType{
        GetLinkID,
        GetAddress,
        GetRegister,
        DumpContiguousBuffer,
        SetAddress,
        SetRegister,
        SetPattern,
        SetConstantPattern,
        SetIncreasingPattern,
        SetDecreasingPattern,
        SetRandomPattern,
        SoftReset,
        CounterReset,
        Capture,
        CheckConnection,
        DumpStatusRegisters,
        DumpTSStatusRegisters,
        DumpCRCErrors,
        DumpDecoderErrors,
        DumpAllLinkIDs,
        ERROR
    };

    bool dumpRegisterArray(std::vector<unsigned int>& vectorOfRegisters, unsigned int nInts, unsigned int *buffer);
    
    bool getFunctionType(char function[10], 
                         functionType &functionType);
    
    
    bool getData(unsigned int address, 
                 unsigned int numberOfValues, 
                 unsigned int *buffer);
    
    bool putData(unsigned int address, 
                 unsigned int numberOfValues, 
                 unsigned int *buffer);
    
    bool poke(unsigned int address, 
              unsigned int value);
    
    unsigned int handlePatternData(void *iData, void *oData, 
                                   unsigned int iSize, unsigned int oSize);
    void addMarkers();
    void addTrailer();

   
    memsvc_handle_t memHandle;
    
    bool verbose;
    
    unsigned int localBuffer[PAGE_INTS];
    
    unsigned int savedBufferType;
    unsigned int savedLinkNumber;
    unsigned int savedNumberOfValues;
    unsigned int nStatusInts;
  std::vector<unsigned int> vec;

    std::vector<unsigned int> patternData;
    
};

#endif
