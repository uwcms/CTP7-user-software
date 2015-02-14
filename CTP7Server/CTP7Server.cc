#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

using namespace std;

#include "CTP7.hh"
#include "CTP7Server.hh"

CTP7Server::CTP7Server() : verbose(false),
			   savedBufferType(0),
			   savedLinkNumber(0),
			   savedNumberOfValues(0) {
  if(memsvc_open(&memHandle) != 0) {
    perror("Memory service connect failed");
    exit(1);
  }
  computeAddresses();
}

CTP7Server::~CTP7Server() {
  if(memsvc_close(&memHandle) != 0) {
    perror("Memory service close failed");
  }
}

void CTP7Server::computeAddresses() {
  // Local array of maximum size needed of all register groups
  uint32_t nAddresses = sizeof(GTHRegisters) / sizeof(uint32_t);
  uint32_t address[nAddresses];  
  // InputLinkRegister Addresses
  nAddresses= sizeof(InputLinkRegisters) / sizeof(uint32_t);
  for(uint32_t iLink = 0; iLink < NILinks; iLink++) {
    for(uint32_t i = 0; i < nAddresses; i++) {
      address[i] = InputLinkRegistersBaseAddress + (iLink * nAddresses + i) * sizeof(uint32_t);
    }
    InputLinkRegisters a;
    memcpy(&a.LINK_STATUS_REG, &address[0], sizeof(InputLinkRegisters));
    inputLinkRegisterAddresses.push_back(a);
  }
  // LinkAlignmentRegister Addresses
  nAddresses = sizeof(LinkAlignmentRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = LinkAlignmentRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&linkAlignmentRegisterAddresses.LINK_ALIGN_REQ_REG, &address[0], sizeof(LinkAlignmentRegisters));
  // InputCaptureRegister Addresses
  nAddresses = sizeof(InputCaptureRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = InputCaptureRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&inputCaptureRegisterAddresses.CAPTURE_REQ_REG, &address[0], sizeof(InputCaptureRegisters));
  // DAQSpyCaptureRegister Addresses
  nAddresses = sizeof(DAQSpyCaptureRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = DAQSpyCaptureRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&daqSpyCaptureRegisterAddresses.DAQ_SPY_CAPTURE_REQ_REG, &address[0], sizeof(DAQSpyCaptureRegisters));
  // DAQRegister Addresses
  nAddresses = sizeof(DAQRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = DAQRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&daqRegisterAddresses.DAQ_L1A_DELAY_LINE_VALUE_REG, &address[0], sizeof(DAQRegisters));
  // AMC13Register Addresses
  nAddresses = sizeof(AMC13Registers) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = AMC13RegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&amc13RegisterAddresses.AMC13_LINK_READY_REG, &address[0], sizeof(AMC13Registers));
  // TCDSRegister Addresses
  nAddresses = sizeof(TCDSRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = TCDSRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&tcdsRegisterAddresses.BC_CLOCK_RST_REG, &address[0], sizeof(TCDSRegisters));
  // TCDSMonitorRegister Addresses
  nAddresses = sizeof(TCDSMonitorRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = TCDSMonitorRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&tcdsMonitorRegisterAddresses.TCDS_MON_CAPTURE_MASK_REG, &address[0], sizeof(TCDSMonitorRegisters));
  // GTHRegister Addresses
  nAddresses= sizeof(GTHRegisters) / sizeof(uint32_t);
  for(uint32_t iLink = 0; iLink < NILinks; iLink++) {
    for(uint32_t i = 0; i < nAddresses; i++) {
      address[i] = GTHRegistersBaseAddress + (iLink * nAddresses + i) * sizeof(uint32_t);
    }
    GTHRegisters a;
    memcpy(&a.GTH_STAT_REG, &address[0], sizeof(GTHRegisters));
    gthRegisterAddresses.push_back(a);
  }
  // QPLLRegister Addresses
  nAddresses= sizeof(QPLLRegisters) / sizeof(uint32_t);
  for(uint32_t iQPLL = 0; iQPLL < NILinks / 4; iQPLL++) {
    for(uint32_t i = 0; i < nAddresses; i++) {
      address[i] = QPLLRegistersBaseAddress + (iQPLL * nAddresses + i) * sizeof(uint32_t);
    }
    QPLLRegisters a;
    memcpy(&a.QPLL_STAT_REG, &address[0], sizeof(QPLLRegisters));
    qpllRegisterAddresses.push_back(a);
  }
  // MiscRegister Addresses
  nAddresses = sizeof(MiscRegisters) / sizeof(uint32_t);
  for(uint32_t i = 0; i < nAddresses; i++) {
    address[i] = MiscRegistersBaseAddress + i * sizeof(uint32_t);
  }
  memcpy(&miscRegisterAddresses.C_DATE_CODE_REG, &address[0], sizeof(MiscRegisters));
}

uint32_t CTP7Server::getAddress(BufferType bufferType, uint32_t offset) {
  switch(bufferType) {
  case(inputBuffer):
    return ILinkBaseAddress + offset;
  case(outputBuffer):
    return OLinkBaseAddress + offset;
  case(daqBuffer):
    return DAQBufferBaseAddress + offset;
  case(tcdsBuffer):
    return TCDSBufferBaseAddress + offset;
  case(inputLinkRegisters):
    return InputLinkRegistersBaseAddress + offset;
  case(linkAlignmentRegisters):
    return LinkAlignmentRegistersBaseAddress + offset;
  case(inputCaptureRegisters):
    return InputCaptureRegistersBaseAddress + offset;
  case(daqSpyCaptureRegisters):
    return DAQSpyCaptureRegistersBaseAddress + offset;
  case(daqRegisters):
    return DAQRegistersBaseAddress + offset;
  case(amc13Registers):
    return AMC13RegistersBaseAddress + offset;
  case(tcdsRegisters):
    return TCDSRegistersBaseAddress + offset;
  case(tcdsMonitorRegisters):
    return TCDSMonitorRegistersBaseAddress + offset;
  case(gthRegisters):
    return GTHRegistersBaseAddress + offset;
  case(qpllRegisters):
    return QPLLRegistersBaseAddress + offset;
  case(miscRegisters):
    return MiscRegistersBaseAddress + offset;
  case(unnamed):
    return offset; // This is super dangerous, but we use it for kludging
  }
  return 0;
}

uint32_t CTP7Server::getMaxAddress(BufferType bufferType) {
  switch(bufferType) {
  case(inputBuffer):
    return ILinkBaseAddress + NILinks * LinkBufSize+1;
  case(outputBuffer):
    return OLinkBaseAddress + NOLinks * LinkBufSize+1;
  case(daqBuffer):
    return DAQBufferBaseAddress + NIntsInDAQBuffer * sizeof(uint32_t)+1;
  case(tcdsBuffer):
    return TCDSBufferBaseAddress + NIntsInTCDSBuffer * sizeof(uint32_t)+1;
  case(inputLinkRegisters):
    return InputLinkRegistersBaseAddress + NILinks * sizeof(InputLinkRegisters)+1;
  case(linkAlignmentRegisters):
    return LinkAlignmentRegistersBaseAddress + sizeof(LinkAlignmentRegisters)+1;
  case(inputCaptureRegisters):
    return InputCaptureRegistersBaseAddress + sizeof(InputCaptureRegisters)+1;
  case(daqSpyCaptureRegisters):
    return DAQSpyCaptureRegistersBaseAddress + sizeof(DAQSpyCaptureRegisters)+1;
  case(daqRegisters):
    return DAQRegistersBaseAddress + sizeof(DAQRegisters)+1;
  case(amc13Registers):
    return AMC13RegistersBaseAddress + sizeof(AMC13Registers)+1;
  case(tcdsRegisters):
    return TCDSRegistersBaseAddress + sizeof(TCDSRegisters)+1;
  case(tcdsMonitorRegisters):
    return TCDSMonitorRegistersBaseAddress + sizeof(TCDSMonitorRegisters)+1;
  case(gthRegisters):
    return GTHRegistersBaseAddress + (NILinks + NOLinks) * sizeof(GTHRegisters)+1;
  case(qpllRegisters):
    return QPLLRegistersBaseAddress + (NILinks + NOLinks) * sizeof(QPLLRegisters)+1;
  case(miscRegisters):
    return MiscRegistersBaseAddress + sizeof(MiscRegisters)+1;
  case(unnamed):
    // This is super dangerous, but we use it for kludging
    return MEMSVC_MAX_WORDS * sizeof(uint32_t)+1;
  }
  return 0;
}

// Memory Access
// TODO: Send more info sent to client when memsvc access fails

bool CTP7Server::getData(uint32_t address,
                         uint32_t numberOfValues,
                         uint32_t *buffer) {
  int wordsToGo = numberOfValues;
  int wordsDone = 0;
  
  while(wordsToGo > 0) {
    //int words = min(wordsToGo, MEMSVC_MAX_WORDS);
    int words = 1; 
    if(memsvc_read(memHandle, address, words, &buffer[wordsDone]) != 0) {
      printf("Memory access failed: %s\n",memsvc_get_last_error(memHandle));
      errorMemSVC = true;
      return false;
    }
    address = address + words*4;
    wordsToGo -= words;
    wordsDone += words;
  }
  
  return true;
}

// Memory Access
bool CTP7Server::putData(uint32_t address,
                         uint32_t numberOfValues,
                         uint32_t *buffer) {
  
  int wordsToGo = numberOfValues;
  int wordsDone = 0;
  uint32_t writeBuffer = 0;
  
  while(wordsToGo > 0) {
    int words = 1;
    writeBuffer = buffer[wordsDone];
    if(memsvc_write(memHandle, address, words, &writeBuffer) != 0) {
#ifdef EMBED
      printf("Memory access failed: %s\n",memsvc_get_last_error(memHandle));
#endif
      errorMemSVC = true;
      return false;
    }
    
    address = address + words*4;
    wordsToGo -= words;
    wordsDone += words;
  }
  
  return true;
}

/*
 * Mini-service to handle single poking operations
 */

bool CTP7Server::poke(uint32_t address, uint32_t value)
{
  uint32_t valuePtr = value;
  if(putData(address,1,&valuePtr))
    return true;
  return false;
}

// Message Handling

bool CTP7Server::parseMessage(char *iMessage,
			      char *function,
			      uint32_t &argc,
			      uint32_t *argv,
			      char *oMessage) {
  
  function = strtok(iMessage, "(");
  if(function == NULL) {
    strcpy(oMessage, "ERROR: Failed to parse message; Function is null");
    return false;
  }
  
  if(strcmp(function, "SetConfiguration") == 0) {
    // Copy the content between "(" and ")" to configuration string
    // It is returned using GetConfiguration
    // There is no local use of this configuration
    // It is just provided for user convenience
    char *token = strtok(NULL, ")");
    setConfiguration(token);
  }
  else {
    while(true) {
      char *token = strtok(NULL, ",)");
      if(token != NULL) {
	if(sscanf(token, "%X", &argv[argc]) == 1) {
	  argc++;
	}
	else {
	  strcpy(oMessage, "ERROR: Failed to parse message; Function = ");
	  strcat(oMessage, function);
	  sprintf(&oMessage[strlen(oMessage)], " ; argc = %d; ", argc);
	  strcat(oMessage, " and Token = ");
	  strcat(oMessage, token);
	  return false;
	}
      }
      else {
	break;
      }
    }
  }

  return true;

}


/*
 * Process TCP Message
 *
 * After receiving a message this method is designed to interpret
 * the msg, perform the proper operation and then either send back
 * a success/failure or return requested data
 *
 * TODO: return more memservice errors
 */

uint32_t CTP7Server::processTCPMessage(void *iData,
				       void *oData,
				       uint32_t iMaxLength,
				       uint32_t oMaxLength,
				       uint32_t *dataArray) {
  //
  errorMemSVC = false;

  // Null terminated string interpretation
  char *iMessage = (char *) iData;
  char *oMessage = (char *) oData;
  
  // Presumed guilty until proven otherwise : )
  strcpy(oMessage, "ERROR_Message_Failed_to_Process");
  
  // If prior call determined that we are getting integer data handle it
  if(savedNumberOfValues > 0) {
    return handlePatternData(iData, oData, iMaxLength/4, oMaxLength);
  }
  
  // Add null termination as we expect a message string
  iMessage[iMaxLength] = 0;

  char *function = iMessage;
  uint32_t argc = 0;
  uint32_t argv[6];
  if(!parseMessage(iMessage, function, argc, argv, oMessage)) {
    strcpy(oMessage, "ERROR_FAIL_TO_PARSE");
    return strlen(oMessage);
  }
  
  //Determine which function will be accessed
  function = iMessage;
  FunctionType functionType = ERROR;
  if(!getFunctionType(function, functionType))
    return strlen(oMessage);
  
  //Special Buffer Length Return for Mem dumps
  uint32_t bufferLen = 0;
  
  if(functionType==ERROR)
    std::cout<<"FunctionType == ERROR!!"<<std::endl;
  
  /*
   * Switch statement to process the input message
   * and run it through the proper function
   * Order (or subset there of):
   * oMessage is presumed failed until SUCCESS
   * Check NArgs, if wrong return with error NArgs, if correct
   * perform action, if success return SUCCESS via oMessage
   *
   * Guidelines: Each operation has a check to see if it was
   * successful, if not successful, return an error message
   *
   */
  
  CaptureStatus captureStatus = Idle;
  switch(functionType)
    {
      
    case(CheckConnection):
      strcpy(oMessage, "HelloToYou!");
      break;
      
    case(Capture):
      if(!capture())
        strcpy(oMessage, "ERROR_CAPTURING");
      else
        strcpy(oMessage, "SUCCESS");
      break;

    case(GetCaptureStatus):
      if(!getCaptureStatus(&captureStatus))
        strcpy(oMessage, "ERROR_RETRIEVING_CAPTURE_STATUS");
      else
        sprintf(oMessage, "%X" , captureStatus);
      break;
  
    case(GetConfiguration):
      strcpy(oMessage, configuration.c_str());
      break;

    case(SetConfiguration):
      strcpy(oMessage, "SUCCESS");
      break;

    case(SetConstantPattern):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(setConstantPattern((BufferType) argv[0], argv[1], argv[2]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_SETTING_PATTERN");
      break;
      
    case(SetIncreasingPattern):
      if(argc != 4)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(setIncreasingPattern((BufferType) argv[0], argv[1], argv[2], argv[3]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_SETTING_PATTERN");
      break;
      
    case(SetDecreasingPattern):
      if(argc != 4)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(setDecreasingPattern((BufferType) argv[0], argv[1], argv[2], argv[3]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_SETTING_PATTERN");
      break;
      
    case(SetRandomPattern):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(setRandomPattern((BufferType) argv[0], argv[1], argv[2]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_SETTING_PATTERN");
      break;
      
    case(SetPattern):
      if(argc < 2)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(handlePatternData(NULL, (void *) &argv, 0, sizeof(argv)) == 0)
          strcpy(oMessage, "READY_FOR_PATTERN_DATA");
        else
          strcpy(oMessage, "FAILED__NOT_READY_FOR_PATTERN_DATA");
      break;
      
    case(SoftReset):
      if(softReset())
        strcpy(oMessage, "SUCCESS");
      else
        strcpy(oMessage, "FAILED_SOFT_RESET");
      break;
      
    case(CounterReset):
      if(counterReset())
        strcpy(oMessage, "SUCCESS");
      else
        strcpy(oMessage, "FAILED_COUNTER_RESET");
      break;
      
    case(GetValue):
      if(argc != 2)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else {
	uint32_t value = getValue((BufferType) argv[0], argv[1]);
	sprintf(oMessage, "%X", value);
      }
      break;

    case(SetValue):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS_FAILED_To_Set_Value");
      else
	if(setValue((BufferType) argv[0], argv[1], argv[2]))
	  strcpy(oMessage, "SUCCESS");
	else
	  strcpy(oMessage, "ERROR_WRONG_NINTS_IN_INPUT_MSG");
      break;
      
    case(GetValues):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(argv[2] < oMaxLength / 4)
          if(getValues((BufferType) argv[0], argv[1], argv[2], (uint32_t *) oData))
            bufferLen = (argv[2] * 4);
          else
            strcpy(oMessage, "ERROR: getData failed mysteriously");
	else
	  strcpy(oMessage, "ERROR_WRONG_NINTS_IN_INPUT_MSG");
      if(bufferLen==0) bufferLen = strlen(oMessage);
      break;
      
    case(SetValues):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(argv[2] < oMaxLength / 4)
          if(setValues((BufferType) argv[0], argv[1], argv[2], (uint32_t *) oData))
            bufferLen = (argv[2] * 4);
          else
            strcpy(oMessage, "ERROR: setData failed mysteriously");
	else
	  strcpy(oMessage, "ERROR_WRONG_NINTS_IN_INPUT_MSG");
      if(bufferLen==0) bufferLen = strlen(oMessage);
      break;
      
    case(ERROR):
      strcpy(oMessage, "INPUT_FUNCTION_NOTFOUND");
      break;
      
      //Note: We should never get to default; included only for completeness
    default:
      strcpy(oMessage, "INPUT_FUNCTION_NOTFOUND");

    }
  
  //special return statement only for dumping large buffers, a finite use
  if(!errorMemSVC && (functionType==GetValues))
    return bufferLen;
  
  if(errorMemSVC)
    strcpy(oMessage, "ERROR_WITH_MEMSVC");

  cout<<"oMessage "<< oMessage<< endl;
  oData = (void *) oMessage;
  		
  return strlen(oMessage);
}

/*
 * Initiate Capture
 */

bool CTP7Server::capture(){
  if(!poke(inputCaptureRegisterAddresses.CAPTURE_REQ_REG    , 0x1 ))
    return false;
  return true;
}

bool CTP7Server::getCaptureStatus(CaptureStatus *captureStatus){
  return getData(inputCaptureRegisterAddresses.CAPTURE_MODE_REG, 1, (uint32_t*) captureStatus);
}

/*
 * Counter Reset
 */
bool CTP7Server::counterReset()
{
  return true;
}

/*
 * Soft Reset for CTP7 Stage1 Firmware Gen 2
 * Resets Links and Errors
 */

bool CTP7Server::softReset()
{

  for(uint32_t i = 0; i < gthRegisterAddresses.size(); i++){
    std::cout<<"i: "<<std::hex<< gthRegisterAddresses.at(i).GTH_RST_REG<<std::endl;
    if(!poke( gthRegisterAddresses.at(i).GTH_RST_REG , 0x3))
       return false;
  }

  sleep(1); 

  if(!poke( LinkAlignmentRegistersBaseAddress , 0x1 )){
    return false; 
  }

  sleep(1); 
  
  if(!poke( CRCBC0ResetAddress , 0x3 )){
    return false;
  }
  
  if(getValue(linkAlignmentRegisters,0x8)!=0){
    return false;
  }
  
  return true;
}


/*
 * Get Function Type
 * This compares the received msg to the optional valid msg
 */

bool CTP7Server::getFunctionType(char* function, FunctionType &functionType)
{
  strcat(function,"\0");
  std::cout<<"function :"<<function <<std::endl;
  functionType = ERROR;
  if(strcmp(function,      "getValue") == 0)
    functionType = GetValue;
  else if(strcmp(function, "getValues") == 0)
    functionType = GetValues;
  else if(strcmp(function, "setValue") == 0)
    functionType = SetValue;
  else if(strcmp(function, "setValues") == 0)
    functionType = SetValues;
  else if(strcmp(function, "capture") == 0)
    functionType = Capture;
  else if(strcmp(function, "getConfiguration") == 0)
    functionType = GetConfiguration;
  else if(strcmp(function, "getCaptureStatus") == 0)
    functionType = GetCaptureStatus;
  else if(strcmp(function, "setConfiguration") == 0)
    functionType = SetConfiguration;
  else if(strcmp(function, "setConstantPattern") == 0)
    functionType = SetConstantPattern;
  else if(strcmp(function, "setIncreasingPattern") == 0)
    functionType = SetIncreasingPattern;
  else if(strcmp(function, "setDecreasingPattern") == 0)
    functionType = SetDecreasingPattern;
  else if(strcmp(function, "setRandomPattern") == 0)
    functionType = SetRandomPattern;
  else if(strcmp(function, "setPattern") == 0)
    functionType = SetPattern;
  else if(strcmp(function, "softReset") == 0)
    functionType = SoftReset;
  else if(strcmp(function, "counterReset") == 0)
    functionType = CounterReset;
  else if(strcmp(function, "Hello") == 0)
    functionType = CheckConnection;
  if(functionType == ERROR)
    std::cout<<"function type is not valid!!!"<<std::endl;
  
  return true;
}

uint32_t CTP7Server::getValue(BufferType b,
			      uint32_t addressOffset) {
  if(addressOffset % sizeof(uint32_t)) return false;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  uint32_t value;
  if(getData(address, 1, &value)) return value;
  return 0xDEADBEEF;
}

bool CTP7Server::setValue(BufferType b,
			  uint32_t addressOffset,
			  uint32_t value) {
  if(addressOffset % sizeof(uint32_t)) return false;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  return putData(address, 1, &value);
}

bool CTP7Server::getValues(BufferType b,
			   uint32_t startAddressOffset,
			   uint32_t numberOfValues,
			   uint32_t *buffer) {
  if(startAddressOffset % sizeof(uint32_t)) return false;
  if(buffer == 0) buffer = localBuffer;
  uint32_t address = getAddress(b, startAddressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + numberOfValues * 4;
  if(endAddress > getMaxAddress(b)) return false;
  return getData(address, numberOfValues, buffer);
}

bool CTP7Server::setValues(BufferType b,
			   uint32_t startAddressOffset,
			   uint32_t numberOfValues,
			   uint32_t *values) {
  if(startAddressOffset % sizeof(uint32_t)) return false;
  uint32_t address = getAddress(b, startAddressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + numberOfValues * 4;
  if(endAddress > getMaxAddress(b)) return false;
  for(uint32_t i = 0 , j = 0; i < numberOfValues; i++) {
    localBuffer[j++] = values[i];
    //only write if these conditions are satisfied
    //we only want to write at most a page at a time
    if(j == PAGE_INTS || j == numberOfValues ){
      if(!putData(address, j, localBuffer)) {
	std::cout<<"Error putting data!!"<<std::endl;
	return false;
      }
      address += j*4;
      j = 0;
    }
  }
  return true;
}

bool CTP7Server::printBuffer(uint32_t address, uint32_t numberOfValues, uint32_t * buffer)
{
  if(numberOfValues > 1) {
    cout << showbase << internal << setfill('0') << setw(10) << hex
	 << "Dump of data from address = "
	 << address << endl;
  }
  for(uint32_t i = 0; i < numberOfValues; i++) {
    if(numberOfValues == 1) {
      cout << showbase << internal << setfill('0') << setw(10) << hex
	   << "Content of address " << address << " = "
	   << buffer[i] << endl;
    }
    else {
      cout << noshowbase << internal << setfill('0') << setw(8) << hex
	   << buffer[i] << " ";
      if(((i + 1) % 8) == 0) cout << endl;
    }
  }
  return true;
}


/*
 * --------------- Pattern Methods ---------------
 */

/* Since input pattern data could be rather large we need to handle
 * multiple packets.  The function is initialized with SetPattern()
 * message, which would have set nIntsExpected to be non-zero.
 * Subsequent processTCPMessage calls branch here so that nIntsExpected
 * are received in multiple calls.  Upon completion we set the pattern
 * and return
 */

uint32_t CTP7Server::handlePatternData(void* iData, void* oData,
				       uint32_t iMaxLength,
				       uint32_t oMaxLength)
{
  
  char *oMessage = (char *) oData;
  
  // For first call iData should be NULL here as we are only inputting a message
  if(iData == NULL) {
    patternData.clear();
    uint32_t *argv = (uint32_t*) oData;
    savedBufferType = argv[0];
    savedLinkNumber = argv[1];
    savedNumberOfValues = argv[2];
    return 0;
  }
  
  // Second call on sends a buffer of ints
  // iData should be non NULL here
  
  if(iData != NULL){
    uint32_t *intValues = (uint32_t*) iData;
    for(uint32_t i = 0; i < max(savedNumberOfValues, iMaxLength); i++) {
      patternData.push_back(intValues[i]);
    }

    //Adjust Saved Number Of Values
    //-->This will check to see if more values need to be sent
    savedNumberOfValues -= max(savedNumberOfValues, iMaxLength);
    
    if(savedNumberOfValues == 0) {
      if(setPattern((BufferType) savedBufferType, savedLinkNumber, savedNumberOfValues, patternData))
        strcpy(oMessage, "SUCCESS");
      else
        strcpy(oMessage, "ERROR");
    }
    else
      strcpy(oMessage, "SEND_MORE_PATTERN_DATA");
    
  }
  
  return strlen(oMessage);
}

bool CTP7Server::setConstantPattern(BufferType b,
                                    uint32_t linkNumber,
                                    uint32_t value)
{
  uint32_t addressOffset = linkNumber * LinkBufSize;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + LinkBufSize;
  if(endAddress > getMaxAddress(b)) return false;
  for(uint32_t i = 0, j = 0; i < NIntsPerLink; i++) {
    localBuffer[j++] = value;
    if(j == PAGE_INTS || i == (NIntsPerLink -1)) {
      if(!putData(address, j, localBuffer)) return false;
      address += j*4;
      j = 0;
    }
  }
  return true;
}

bool CTP7Server::setIncreasingPattern(BufferType b,
                                      uint32_t linkNumber,
                                      uint32_t startValue,
                                      uint32_t increment) {
  uint32_t addressOffset = linkNumber * LinkBufSize;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + LinkBufSize;
  if(endAddress > getMaxAddress(b)) return false;
  uint32_t value = startValue;
  for(uint32_t i = 0, j = 0; i < NIntsPerLink; i++) {
    localBuffer[j++] = value;
    if(j == PAGE_INTS || i == (NIntsPerLink - 1)) {
      if(!putData(address, j, localBuffer)) return false;
      address += j*4;
      j = 0;
    }
    value += increment;
  }
  return true;
}

bool CTP7Server::setDecreasingPattern(BufferType b,
                                      uint32_t linkNumber,
                                      uint32_t startValue,
                                      uint32_t increment)
{
  uint32_t addressOffset = linkNumber * LinkBufSize;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + LinkBufSize;
  if(endAddress > getMaxAddress(b)) return false;
  uint32_t value = startValue;
  for(uint32_t i = 0, j = 0; i < NIntsPerLink; i++) {
    localBuffer[j++] = value;
    if(j == PAGE_INTS || i == (NIntsPerLink - 1)) {
      if(!putData(address, j, localBuffer)) return false;
      address += j*4;
      j = 0;
    }
    value -= increment;
  }
  return true;
}

bool CTP7Server::setPattern(BufferType b,
                            uint32_t linkNumber,
			    uint32_t numberOfValues,
                            std::vector<uint32_t> values) {
  uint32_t addressOffset = linkNumber * LinkBufSize;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  if(numberOfValues != values.size()) return false;
  uint32_t endAddress = address + min((uint32_t) NIntsPerLink, (uint32_t) values.size()) * 4;
  if(endAddress > getMaxAddress(b)) return false;
  for(uint32_t i = 0 , j = 0; i < min((uint32_t) NIntsPerLink, (uint32_t) values.size()); i++) {
    localBuffer[j++] = values[i];
    //only write if these conditions are satisfied
    //we only want to write at most a page at a time
    if(j == PAGE_INTS || j == values.size() ){
      if(!putData(address, j, localBuffer)) {
	std::cout<<"Error putting data!!"<<std::endl;
	return false;
      }
      address += j*4;
      j = 0;
    }
  }
  return true;
}

bool CTP7Server::setRandomPattern(BufferType b,
                                  uint32_t linkNumber,
                                  uint32_t randomSeed)
{
  uint32_t addressOffset = linkNumber * LinkBufSize;
  uint32_t address = getAddress(b, addressOffset);
  if(address == BadAddress) return false;
  uint32_t endAddress = address + LinkBufSize;
  if(endAddress > getMaxAddress(b)) return false;
  static bool first = true;
  static char state[32] = "This is the state of the random";
  if(first) {
    uint32_t seed = 0xDEADBEEF;
    if(randomSeed != 0) seed = randomSeed;
    char* initialState = initstate(seed, state, 32);
    if(initialState == 0) {
      cerr << "Random number initialization failed?" << endl;
      return false;
    }
    first = false;
  }
  else {
    // If the random seed is non-zero, use definite random pattern
    // Otherwise, use whatever the state is left at so that
    // repeated calls yield random patterns
    if(randomSeed != 0) srandom(randomSeed);
  }
  for(uint32_t i = 0, j = 0; i < NIntsPerLink; i++) {
    localBuffer[j++] = rand();
    if(j == PAGE_INTS || i == (NIntsPerLink - 1)) {
      if(!putData(address, j, localBuffer)) return false;
      address += j*4;
      j = 0;
    }
  }
  return true;
}

void CTP7Server::logTimeStamp()
{
  string res = "";

  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  // Wednesday, March 19, 2014 01:06:18 PM
  strftime (buffer,80,"%A, %B %d, %Y %r",timeinfo);

  std::cout<<buffer<<std::endl;
  return ;
}

#ifndef EMBED

// This code is for testing on linux or MAC where there will be no libmemsvc.a

#define SIZE_IN_INTS MEMSVC_MAX_WORDS
#define SIZE_IN_BYTES SIZE_IN_INTS * sizeof(uint32_t)
#define maxAddress SIZE_IN_BYTES

int memsvc_open(memsvc_handle_t *handle) {
  *handle = new uint32_t [SIZE_IN_INTS];
  if(*handle == 0) {
    return -1;
  }
  return 0;
}

int memsvc_close(memsvc_handle_t *handle) {
  delete *handle;
  return 0;
}



// Local implentation of memsvc routines for testing purpose
int memsvc_read(memsvc_handle_t handle, uint32_t addr, uint32_t words, uint32_t *data) {
  uint32_t startIndex = addr / 4;
  uint32_t endIndex = startIndex + words;
  
  if(startIndex > SIZE_IN_INTS || endIndex > SIZE_IN_INTS || words > MEMSVC_MAX_WORDS)
    return -1;
  
  for(uint32_t i = startIndex; i < endIndex; i++) {
    data[i - startIndex] = handle[i];
  }
  return 0;
}
int memsvc_write(memsvc_handle_t handle, uint32_t addr, uint32_t words, const uint32_t *data) {
  uint32_t startIndex = addr / 4;
  uint32_t endIndex = startIndex + words;
  if(startIndex > SIZE_IN_INTS || endIndex > SIZE_IN_INTS || words > MEMSVC_MAX_WORDS) {
    return -1;
  }
  for(uint32_t i = startIndex; i < endIndex; i++) {
    handle[i] = data[i - startIndex];
  }
  return 0;
}
const char* memsvc_get_last_error(memsvc_handle_t handle) {
  return "undefined";
}

#endif
