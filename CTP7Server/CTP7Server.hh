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
#include <string>

#include <stdint.h>
#include "CTP7.hh"
#include "CTP7Addresses.hh"

#ifdef EMBED
#include "libmemsvc.h"
#else

// Below exists for debugging purpose on non embedded computers
typedef uint32_t *memsvc_handle_t;
int memsvc_open(memsvc_handle_t *handle);
int memsvc_close(memsvc_handle_t *handle);
const char *memsvc_get_last_error(memsvc_handle_t handle);

#define MEMSVC_MAX_WORDS	0x100000

int memsvc_read(memsvc_handle_t handle, uint32_t addr, uint32_t words, uint32_t *data);
int memsvc_write(memsvc_handle_t handle, uint32_t addr, uint32_t words, const uint32_t *data);

#endif

#define PAGE_INTS 256
#define PAGE_SIZE (PAGE_INTS * 4)

//MaxInputLinkAddress
#define maxILinkAddress ( ILinkBaseAddress + NILinks * LinkBufSize)
#define maxOLinkAddress ( OLinkBaseAddress + NOLinks * LinkBufSize)

class CTP7Server : public CTP7 {
    
public:
    
  CTP7Server();
  virtual ~CTP7Server();

  // Externally accessible functions to get/set on-board buffer
  bool printBuffer(uint32_t address, uint32_t numberOfValues, uint32_t * buffer);
    
  // Configuration

  bool getConfiguration(std::string o) {o = configuration; return true;}
  bool setConfiguration(std::string i) {configuration = i; return true;}

  uint32_t getValue(BufferType bufferType,
		    uint32_t addressOffset);

  bool setValue(BufferType bufferType,
		uint32_t addressOffset,
		uint32_t value);
    
  bool getValues(BufferType bufferType,
		 uint32_t startAddressOffset,
		 uint32_t numberOfValues,
		 uint32_t *buffer);
  
  bool setValues(BufferType bufferType,
		 uint32_t startAddressOffset,
		 uint32_t numberOfValues,
		 uint32_t *buffer);
  
  bool setPattern(BufferType bufferType,
		  uint32_t linkNumber,
		  uint32_t numberOfValues,
		  std::vector<uint32_t> values);

  bool setConstantPattern(BufferType bufferType,
			  uint32_t linkNumber,
			  uint32_t value);

  bool setIncreasingPattern(BufferType bufferType,
			    uint32_t linkNumber,
			    uint32_t startValue = 0,
			    uint32_t increment = 1);
    
  bool setDecreasingPattern(BufferType bufferType,
			    uint32_t linkNumber,
			    uint32_t startValue = (NIntsPerLink - 1),
			    uint32_t increment = 1);
    
  bool setRandomPattern(BufferType bufferType,
			uint32_t linkNumber,
			uint32_t randomSeed);

  bool getCaptureStatus(CaptureStatus *captureStatus);
  bool capture();
  bool softReset();
  bool counterReset();
  bool checkConnection(){return true;}; //these need to be added as methods, current implentation is incorrect
  bool hardReset(){return true;};//these need to be added as methods

  uint32_t processTCPMessage(void *iMessage,
			     void *oMessage,
			     uint32_t iMaxLength,
			     uint32_t oMaxLength,
			     uint32_t *dataArray=0);

  void logTimeStamp();
  
protected:
    
private:
    
  // Unnecessary methods are made private
  CTP7Server(const CTP7Server&);
  const CTP7Server& operator=(const CTP7Server&);
    
  // Helper functions

  bool errorMemSVC;
  /*
   * Enumerator for possible functions:
   */
    
  enum FunctionType{
    CheckConnection,
    GetConfiguration,
    SetConfiguration,
    HardReset,
    SoftReset,
    CounterReset,
    GetValue,
    SetValue,
    GetValues,
    SetValues,
    SetPattern,
    SetConstantPattern,
    SetIncreasingPattern,
    SetDecreasingPattern,
    SetRandomPattern,
    Capture,
    GetCaptureStatus,
    ERROR
  };
  
  bool parseMessage(char *iMessage,
		    char *function,
		    uint32_t &argc,
		    uint32_t *argv,
		    char *oMessage);

  bool getFunctionType(char *function, 
		       FunctionType &functionType);
    
  bool getData(uint32_t address, 
	       uint32_t numberOfValues, 
	       uint32_t *buffer);
    
  bool putData(uint32_t address, 
	       uint32_t numberOfValues, 
	       uint32_t *buffer);
    
  bool poke(uint32_t address, 
	    uint32_t value);
    
  uint32_t handlePatternData(void *iData, void *oData, 
			     uint32_t iSize, uint32_t oSize);
  void addMarkers();
  void addTrailer();

  void computeAddresses();
   
  uint32_t getAddress(BufferType b, uint32_t startAddressOffset = 0);
  uint32_t getMaxAddress(BufferType b);

  memsvc_handle_t memHandle;
    
  bool verbose;

  std::string configuration;
    
  uint32_t localBuffer[PAGE_INTS];
    
  uint32_t savedBufferType;
  uint32_t savedLinkNumber;
  uint32_t savedNumberOfValues;
  uint32_t nStatusInts;

  std::vector<InputLinkRegisters> inputLinkRegisterAddresses;
  LinkAlignmentRegisters linkAlignmentRegisterAddresses;
  InputCaptureRegisters inputCaptureRegisterAddresses;
  DAQSpyCaptureRegisters daqSpyCaptureRegisterAddresses;
  DAQRegisters daqRegisterAddresses;
  AMC13Registers amc13RegisterAddresses;
  TCDSRegisters tcdsRegisterAddresses;
  TCDSMonitorRegisters tcdsMonitorRegisterAddresses;
  std::vector<GTHRegisters> gthRegisterAddresses;
  std::vector<QPLLRegisters> qpllRegisterAddresses;
  MiscRegisters miscRegisterAddresses;

  std::vector<uint32_t> patternData;
    
};

#endif
