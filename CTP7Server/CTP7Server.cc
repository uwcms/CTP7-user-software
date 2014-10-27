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
  
}

CTP7Server::~CTP7Server() {
  if(memsvc_close(&memHandle) != 0) {
    perror("Memory service close failed");
  }
}

//FIX ME PROBLEM WITH MEMORY SERVICE!
bool CTP7Server::getData(unsigned int address,
                         unsigned int numberOfValues,
                         unsigned int *buffer) {
  int wordsToGo = numberOfValues;
  int wordsDone = 0;
  
  while(wordsToGo > 0) {
    int words = min(wordsToGo, MEMSVC_MAX_WORDS);
    if(memsvc_read(memHandle, address, words, &buffer[wordsDone]) != 0) {
      //FIXME
      printf("Memory access failed: %s\n",memsvc_get_last_error(memHandle));
      return false;
    }
    wordsToGo -= words;
    wordsDone += words;
  }
  return true;
}

/*
 bool CTP7Server::getData(unsigned int address,
 unsigned int numberOfValues,
 unsigned int *buffer) {
 int wordsToGo = numberOfValues;
 int wordsDone = 0;
 unsigned int readBuffer;
 
 while(wordsToGo > 0) {
 //Set n words to read == 1 to cope with memsvc
 int words = 1;
 if(memsvc_read(memHandle, address+wordsDone*4, words, &readBuffer) != 0) {
 #ifdef EMBED
 printf("Memory access failed: %s\n",memsvc_get_last_error(memHandle));
 #endif
 return false;
 }
 
 buffer[wordsDone]=readBuffer;
 wordsToGo -= words;
 wordsDone += words;
 }
 return true;
 }
 */
bool CTP7Server::putData(unsigned int address,
                         unsigned int numberOfValues,
                         unsigned int *buffer) {
  
  int wordsToGo = numberOfValues;
  int wordsDone = 0;
  unsigned int writeBuffer = 0;
  
  while(wordsToGo > 0) {
    int words = 1;
    writeBuffer = buffer[wordsDone];
    
    if(memsvc_write(memHandle, address, words, &writeBuffer) != 0) {
#ifdef EMBED
      printf("Memory access failed: %s\n",memsvc_get_last_error(memHandle));
#endif
      std::cout<<"Memory access failed "<<std::endl;
      
      return false;
    }
    
    address = address + 4;
    wordsToGo -= words;
    wordsDone += words;
  }
  
  return true;
}

unsigned int CTP7Server::getAddress(BufferType b,
                                    unsigned int linkNumber,
                                    unsigned int addressOffset)
{
  unsigned int address = 0;
  if(b == registerBuffer)
    address = addressOffset;
  else if(b == inputBuffer) {
    if(linkNumber < NILinks && !(addressOffset % sizeof(int)) &&
       addressOffset < NIntsPerLink * sizeof(int)) {
      address = ILinkBaseAddress + addressOffset;
    }
    else
      return 0xDEADBEEF;
  }
  else if(b == outputBuffer) {
    if(linkNumber < NOLinks &&
       !(addressOffset % sizeof(int)) &&
       addressOffset < NIntsPerLink * sizeof(int)) {
      address = OLinkBaseAddress + addressOffset;
    }
    else
      return 0xDEADBEEF;
  }
  
  unsigned int buffer;
  
  if(getData(address, 1, &buffer))
    return buffer;
  
  return 0xDEADBEEF;
  
}

bool CTP7Server::setAddress(BufferType b,
                            unsigned int linkNumber,
                            unsigned int addressOffset,
                            unsigned int value)
{
  unsigned int address = 0;
  if(b == registerBuffer) {
    if( !(addressOffset % sizeof(int))) {
      address = addressOffset;
    }
    else
      return false;
  }
  else if(b == inputBuffer) {
    if(linkNumber < NILinks && !(addressOffset % sizeof(int)) &&
       addressOffset < NIntsPerLink * sizeof(int)) {
      address = ILinkBaseAddress + addressOffset;
    }
    else
      return false;
  }
  else if(b == outputBuffer) {
    if(linkNumber < NOLinks && !(addressOffset % sizeof(int)) &&
       addressOffset < NIntsPerLink * sizeof(int)) {
      address = OLinkBaseAddress + addressOffset;
    }
    else
      return false;
  }
  
  return putData(address, 1, &value);
}

bool CTP7Server::dumpContiguousBuffer(BufferType b,
                                      unsigned int linkNumber,
                                      unsigned int startAddressOffset,
                                      unsigned int numberOfValues,
                                      unsigned int *buffer)
{
  if(buffer == 0) buffer = localBuffer;
  
  unsigned int address;
  unsigned int maxAddress;
  
  if(b == inputBuffer) {
    address = getInputLinkAddress( linkNumber , startAddressOffset );
    maxAddress = maxILinkAddress;
  }
  else if(b == outputBuffer) {
    address = getOutputLinkAddress( linkNumber , startAddressOffset);
    maxAddress = maxOLinkAddress;
  }
  else if(b == registerBuffer) {
    address = RegBaseAddress + startAddressOffset;
    maxAddress = RegBaseAddress + RegBufSize;
  }
  else {
    address = startAddressOffset;
    maxAddress = 0xFFFFFFFF;
  }
  
  unsigned int endAddress = address + numberOfValues * 4;
  
  if(endAddress > maxAddress) return false;
  
  if( getData (address, numberOfValues, buffer) ) {
    // printBuffer(address, numberOfValues, buffer);
    return true;
  }
  return false;
}

bool CTP7Server::printBuffer(unsigned int address, unsigned int numberOfValues, unsigned int * buffer)
{
  if(numberOfValues > 1) {
    cout << showbase << internal << setfill('0') << setw(10) << hex
    << "Dump of data from address = "
    << address << endl;
  }
  for(unsigned int i = 0; i < numberOfValues; i++) {
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

bool CTP7Server::setConstantPattern(BufferType b,
                                    unsigned int linkNumber,
                                    unsigned int value)
{
  unsigned int address;
  if(b == inputBuffer && linkNumber < NILinks) {
    address =  getInputLinkAddress( linkNumber );
  }
  else if(b == outputBuffer && linkNumber < NOLinks) {
    address = getOutputLinkAddress( linkNumber );
  }
  else return false;
  
  for(unsigned int i = 0, j = 0; i < NIntsPerLink; i++) {
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
                                      unsigned int linkNumber,
                                      unsigned int startValue,
                                      unsigned int increment)
{
  unsigned int address;
  if(b == inputBuffer && linkNumber < NILinks) {
    address =  getInputLinkAddress( linkNumber );
  }
  else if(b == outputBuffer && linkNumber < NOLinks) {
    address = getOutputLinkAddress( linkNumber );
  }
  else return false;
  unsigned int value = startValue;
  for(unsigned int i = 0, j = 0; i < NIntsPerLink; i++) {
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
                                      unsigned int linkNumber,
                                      unsigned int startValue,
                                      unsigned int increment)
{
  
  unsigned int address;
  
  if(b == inputBuffer && linkNumber < NILinks) {
    address =  getInputLinkAddress( linkNumber );
  }
  else if(b == outputBuffer && linkNumber < NOLinks) {
    address = getOutputLinkAddress( linkNumber );
  }
  else return false;
  unsigned int value = startValue;
  for(unsigned int i = 0, j = 0; i < NIntsPerLink; i++) {
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
                            unsigned int linkNumber,
                            unsigned int nInts,
                            std::vector<unsigned int> values)
{
  
  unsigned int address;
  
  if(b == inputBuffer && linkNumber < NILinks) {
    address =  getInputLinkAddress( linkNumber );
  }
  else if(b == outputBuffer && linkNumber < NOLinks) {
    address = getOutputLinkAddress( linkNumber );
  }
  else return false;
  
  for(unsigned int i = 0 , j = 0; i < min((unsigned int) NIntsPerLink, (unsigned int) values.size()); i++) {
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
                                  unsigned int linkNumber,
                                  unsigned int randomSeed)
{
  unsigned int address;
  
  if(b == inputBuffer && linkNumber < NILinks) {
    address =  getInputLinkAddress( linkNumber );
  }
  else if(b == outputBuffer && linkNumber < NOLinks) {
    address = getOutputLinkAddress( linkNumber );
  }
  else return false;
  
  char state[32] = "This is the state of the random";
  char* initialState = initstate(randomSeed, state, 32);
  if(initialState == 0) {
    cerr << "Random number initialization failed?" << endl;
    return false;
  }
  for(unsigned int i = 0, j = 0; i < NIntsPerLink; i++) {
    localBuffer[j++] = rand();
    if(j == PAGE_INTS || i == (NIntsPerLink - 1)) {
      if(!putData(address, j, localBuffer)) return false;
      address += j*4;
      j = 0;
    }
  }
  return true;
}

bool parseMessage(char *iMessage,
                  unsigned int &argc,
                  unsigned int *argv,
                  char *oMessage)
{
  
  char *function = strtok(iMessage, "(");
  if(function == NULL) {
    strcpy(oMessage, "ERROR: Failed to parse message; Function is null");
    return false;
  }
  
  char *token;
  while(true) {
    token = strtok(NULL, ",)");
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
  //cout<<"argc "<<argc<<" argv[0]: " << argv[0] << " argv[1] "<< argv[1]<<endl;
  return true;
}

/* Since input pattern data could be rather large we need to handle
 * multiple packets.  The function is initialized with SetPattern()
 * message, which would have set nIntsExpected to be non-zero.
 * Subsequent processTCPMessage calls branch here so that nIntsExpected
 * are received in multiple calls.  Upon completion we set the pattern
 * and return
 */

unsigned int CTP7Server::handlePatternData(void* iData, void* oData,
                                           unsigned int iMaxLength,
                                           unsigned int oMaxLength)
{
  
  char *oMessage = (char *) oData;
  
  // First call setPattern(bufferType,Link,nInts), set savedNumberOfvalues
  // iData should be NULL here as we are only inputting a message
  if(iData == NULL) {
    patternData.clear();
    unsigned int *argv = (unsigned int*) oData;
    savedBufferType = argv[0];
    savedLinkNumber = argv[1];
    savedNumberOfValues = argv[2];
    return 0;
  }
  
  // Second Call sends a buffer of ints
  // iData should be non NULL here
  if(iData != NULL){
    
    unsigned int *intValues = (unsigned int*) iData;
    
    for(unsigned int i = 0; i < max(savedNumberOfValues, iMaxLength); i++) {
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

unsigned int CTP7Server::processTCPMessage(void *iData,
                                           void *oData,
                                           unsigned int iMaxLength,
                                           unsigned int oMaxLength,
                                           unsigned int *dataArray) {
  // Null terminated string interpretation
  char *iMessage = (char *) iData;
  char *oMessage = (char *) oData;
  
  // Presumed guilty until proven otherwise : )
  strcpy(oMessage, "ERROR_Message_Failed_to_Process");
  
  // If prior call determined that we are getting integer data handle it
  // TODO: Not crazy about this early return statement, integrate into the switch statement?
  if(savedNumberOfValues > 0) {
    return handlePatternData(iData, oData, iMaxLength/4, oMaxLength);
  }
  
  // Add null termination as we expect a message string
  iMessage[iMaxLength] = 0;
  
  // Decode message and handle it, for now presume function is unique with 10 char array
  
  char function[10];
  strncpy(function,iMessage,10);
  unsigned int argc = 0;
  unsigned int argv[6];
  
  //cout<<"iMessage: "<<iMessage<<" function: "<< function<<endl;
  
  if(!parseMessage(iMessage, argc, argv, oMessage)) {
    strcpy(oMessage, "ERROR_FAIL_TO_PARSE");
    return strlen(oMessage);
  }
  
  //Determine which function will be accessed
  functionType functionType = ERROR;
  unsigned int value = 0;
  
  if(!getFunctionType(function,functionType))
    return strlen(oMessage);
  
  //Special Buffer Length Return for Mem dumps
  unsigned int bufferLen = 0;
  
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
   * TODO:Make dump status more flexible to have options for
   * different sets of registers to dump
   */
  
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
      
    case(GetAddress):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else{
        value = getAddress((BufferType) argv[0], argv[1], argv[2]);
        sprintf(oMessage, "%X", value);
        //sprintf((char*)oData, "%X", value);
        //dataArray[0]=value;
      }
      break;
      
    case(GetRegister):
      if(argc != 1)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else{
        value = getRegister(argv[0]);
        sprintf(oMessage, "%X", value);
      }
      break;
      
    case(GetLinkID):
      if(argc != 3)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else{
        value = getAddress((BufferType) argv[0], 0, LinkIDBase + 4 * argv[2]);
        sprintf(oMessage, "%X", value);
        //sprintf((char*)oData, "%X", value);
        //dataArray[0]=value;
      }
      break;
      
    case(SetRegister):
      if(argc != 2 )
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(setRegister(argv[0], argv[1]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_GETTING_ADDRESS");
      break;
      
    case(SetAddress):
      if(argc != 4)
        strcpy(oMessage, "ERROR_WRONG_NARGS_FAILED_To_Set_Address");
      else
        if(setAddress((BufferType) argv[0], argv[1], argv[2], argv[3]))
          strcpy(oMessage, "SUCCESS");
        else
          strcpy(oMessage, "ERROR_WRONG_NARGS_FAILED_To_Set_Address");
      break;
      
    case(DumpContiguousBuffer):
      if(argc != 4)
        strcpy(oMessage, "ERROR_WRONG_NARGS");
      else
        if(argv[3] < oMaxLength / 4)
          if(dumpContiguousBuffer((BufferType) argv[0], argv[1], argv[2], argv[3],(unsigned int *) oData))
            bufferLen = (argv[3] * 4);
          else
            strcpy(oMessage, "ERROR: dumpContiguousBuffer failed mysteriously");
	else
	  strcpy(oMessage, "ERROR_WRONG_NINTS_IN_INPUT_MSG");
      if(bufferLen==0)
        bufferLen = strlen(oMessage);
      break;
      
      //Begin Set Pattern
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
      
      //Begin system operations, no need for arg check
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
      
    case(DumpStatusRegisters):
      //nStatusInts declared in header
      nStatusInts = 12;
      vec.reserve(nStatusInts);
      fillStatusVector(vec);
      //std::cout<<"returnStatusVector"<<std::endl;
      //for(unsigned int i = 0; i<vec.size() ; i++)
      //std::cout<<"vec.at("<<i<<") " <<vec.at(i)<<std::endl;

      if(dumpRegisterArray(vec,nStatusInts,(unsigned int *) oData))
        bufferLen = vec.size() * 4;
      else
        strcpy(oMessage, "FAILED_TO_DUMP_STATUS_REGISTERS");
      
      if(bufferLen==0)
        bufferLen = strlen(oMessage);
      break;
      
    case(ERROR):
      strcpy(oMessage, "INPUT_FUNCTION_NOTFOUND");
      break;
      
      //Note: We should never get to default; included only for completeness
    default:
      strcpy(oMessage, "INPUT_FUNCTION_NOTFOUND");
    }

  
  cout<<"oMessage "<< oMessage<< endl;
  oData = (void *) oMessage;
  
  //special return statement only for dumping large buffer
  if(functionType==DumpContiguousBuffer||functionType==DumpStatusRegisters)
    return bufferLen;
		
  return strlen(oMessage);
}

bool CTP7Server::capture(){
  if(!poke( CAPTURE    , 0x0 ))
    return false;
  if(!poke( CAPTURE    , 0x1 ))
    return false;
  if(!poke( CAPTURE    , 0x0 ))
    return false;
  return true;
}
/*
 * Counter Reset
 */
bool CTP7Server::counterReset()
{
  if(!poke( RX_CAPTURE_ENGINE_RESET_EN     , 0x1 ))
    return false;
  if(!poke( RX_CAPTURE_ENGINE_RESET_EN     , 0x0 ))
    return false;
  if(!poke( RX_CRC_ERROR_CNT_RST_EN        , 0x1 ))
    return false;
  if(!poke( RX_CRC_ERROR_CNT_RST_EN        , 0x0 ))
    return false;
  if(!poke( RX_DECODER_UNLOCKED_CNT_RST_EN , 0x1 ))
    return false;
  if(!poke( RX_DECODER_UNLOCKED_CNT_RST_EN , 0x0 ))
    return false;
  return true;
}

/*
 * Soft Reset for CTP7 Stage1 Firmware V1
 */

bool CTP7Server::softReset()
{
  //enable GTX RX RESET
  if(!poke( GT_RX_RESET_EN_CXP0 , 0xfff ))
    return false;
  if(!poke( GT_RX_RESET_EN_CXP1 , 0xfff ))
    return false;
  if(!poke( GT_RX_RESET_EN_CXP2 , 0xfff ))
    return false;
  
  //RESET QPLL
  if(!poke( QPLL_RESET          , 0x1ff ))
    return false;
  if(!poke( QPLL_RESET          , 0x0   ))
    return false;
  
  //disable GTX RX REST
  if(!poke( GT_RX_RESET_EN_CXP0 , 0x0   ))
    return false;
  if(!poke( GT_RX_RESET_EN_CXP1 , 0x0   ))
    return false;
  if(!poke( GT_RX_RESET_EN_CXP2 , 0x0   ))
    return false;
  
  return true;
}

/*
 * Mini-service to handle single poking operations
 */
bool CTP7Server::poke(unsigned int address, unsigned int value)
{
  unsigned int valuePtr = value;
  if(putData(address,1,&valuePtr))
    return true;
  return false;
}

/*
 * Get Function Type
 * This compares the received msg to the optional valid msg
 */

bool CTP7Server::getFunctionType(char function[10], functionType &functionType)
{
  strcat(function,"\0");
  std::cout<<"function :"<<function <<std::endl;
  functionType = ERROR;
  if(strncmp(function,      "getAddress", 5) == 0)
    functionType = GetAddress;
  else if(strncmp(function, "getLinkID", 8) == 0)
    functionType = GetLinkID;
  else if(strncmp(function,"capture",5) == 0)
    functionType = Capture;
  else if(strncmp(function, "getRegister", 10) == 0)
    functionType = GetRegister;
  else if(strncmp(function, "setPattern", 10) == 0)
    functionType = SetPattern;
  else if(strncmp(function, "setRegister", 10) == 0)
    functionType = SetRegister;
  else if(strncmp(function, "setAddress", 10) == 0)
    functionType = SetAddress;
  else if(strncmp(function, "dumpContiguousBuffer", 10) == 0)
    functionType = DumpContiguousBuffer;
  else if(strncmp(function, "setConstantPattern", 10) == 0)
    functionType = SetConstantPattern;
  else if(strncmp(function, "setIncreasingPattern", 10) == 0)
    functionType = SetIncreasingPattern;
  else if(strncmp(function, "setDecreasingPattern", 10) == 0)
    functionType = SetDecreasingPattern;
  else if(strncmp(function, "setRandomPattern", 10) == 0)
    functionType = SetRandomPattern;
  else if(strncmp(function, "setPattern", 10) == 0)
    functionType = SetPattern;
  else if(strncmp(function, "softReset", 10) == 0)
    functionType = SoftReset;
  else if(strncmp(function, "dumpStatus", 10) == 0)
    functionType = DumpStatusRegisters;
  else if(strncmp(function, "Hello", 5) == 0)
    functionType = CheckConnection;
  if(functionType==ERROR)
    std::cout<<"function type is not valid!!!"<<std::endl;
  
  return true;
}

// TODO: migrate to c++11 to make use of initialization list
// note: in normal c++ compiler it is possible to initialize
// an array with array[2]={1,2}

bool CTP7Server::fillStatusVector(std::vector<unsigned int> & vector)
{

  vector.push_back( DECODER_LOCKED_CXP0 );
  vector.push_back( DECODER_LOCKED_CXP1 );
  vector.push_back( DECODER_LOCKED_CXP2 );
  vector.push_back( CAPTURE_DONE_CXP0   );
  vector.push_back( CAPTURE_DONE_CXP1   );
  vector.push_back( CAPTURE_DONE_CXP2   );
  vector.push_back( TX_PRBS_SEL         );
  vector.push_back( RX_PRBS_SEL         );
  vector.push_back( GT_LOOPBACK         );
  vector.push_back( FW_DATE_CODE        );
  vector.push_back( FW_GIT_HASH         );
  vector.push_back( FW_GIT_HASH_DIRTY   );

  //std::cout<<"fillStatusVector"<<std::endl;
  //for(unsigned int i = 0; i<vector.size() ; i++)
  //std::cout<<"vector.at("<<i<<") " <<vector.at(i)<<std::endl;

  return true;
}

/*
 * Input: array of status registers
 * Output: array of their statuses
 */

bool CTP7Server::dumpRegisterArray(std::vector<unsigned int>& vectorOfRegisters,unsigned int nInts, unsigned int *buffer)
{

  buffer[0]=0xc0000000;
  for(unsigned int i = 1; i < nStatusInts; i++){
    buffer[i] = getRegister(vectorOfRegisters.at(i-1));
    //std::cout<<dec<<"vectorOfRegisters["<<i<<"] "<<hex<< vectorOfRegisters.at(i) <<" buffer "<< buffer[i] <<std::endl;
  }
  return true;
}


#ifndef EMBED

// This code is for testing on linux or MAC where there will be no libmemsvc.a

#define SIZE_IN_BYTES 0x80000
#define SIZE_IN_INTS SIZE_IN_BYTES / 4
#define maxAddress SIZE_IN_BYTES

int memsvc_open(memsvc_handle_t *handle) {
  *handle = new unsigned int [SIZE_IN_INTS];
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
  unsigned int startIndex = addr / 4;
  unsigned int endIndex = startIndex + words;
  
  if(startIndex > SIZE_IN_INTS || endIndex > SIZE_IN_INTS || words > MEMSVC_MAX_WORDS)
    return -1;
  
  for(unsigned int i = startIndex; i < endIndex; i++) {
    data[i - startIndex] = handle[i];
  }
  return 0;
}
int memsvc_write(memsvc_handle_t handle, uint32_t addr, uint32_t words, const uint32_t *data) {
  unsigned int startIndex = addr / 4;
  unsigned int endIndex = startIndex + words;
  if(startIndex > SIZE_IN_INTS || endIndex > SIZE_IN_INTS || words > MEMSVC_MAX_WORDS) {
    return -1;
  }
  for(unsigned int i = startIndex; i < endIndex; i++) {
    handle[i] = data[i - startIndex];
  }
  return 0;
}


#endif
