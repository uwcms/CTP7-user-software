#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 8192

#include "CTP7Server.hh"

class Mutex {
public:
    Mutex() {pthread_mutex_init(&_mutex, 0);}
    ~Mutex() {pthread_mutex_destroy(&_mutex);}

    void lock() {pthread_mutex_lock (&_mutex);}
    void unlock() {pthread_mutex_unlock (&_mutex);}

private:
    pthread_mutex_t _mutex;
};

Mutex myMutex;
CTP7Server ctp7;

void server(int *sdPtr) {

  int sd = *sdPtr;

  std::cout << "New server thread started for socket descriptor = " << sd << std::endl;

  // This server serves one client per thread
  // Note that the client must HANGUP to exit gracefully in a prompt fashion
  // Otherwise, a timeout (when it occurs) will cause the server to close the thread

  union {
    char incoming_char_buffer[MAX_BUFFER_SIZE];
    unsigned char incoming_data_buffer[MAX_BUFFER_SIZE];
    unsigned int incoming_uint_buffer[MAX_BUFFER_SIZE / 4];
  };

  union {
    char outgoing_char_buffer[MAX_BUFFER_SIZE];
    unsigned char outgoing_data_buffer[MAX_BUFFER_SIZE];
    unsigned int outgoing_uint_buffer[MAX_BUFFER_SIZE / 4];
  };

  while(true) {
    std::cout << "Waiting to receive data..."  << std::endl;

    ssize_t bytes_received = 
      recv(sd, incoming_data_buffer,MAX_BUFFER_SIZE, 0);

    // If no data arrives, the program will just wait here 
    // until some data arrives.

    if(bytes_received > 0) {

      ctp7.logTimeStamp();

      std::cout << "bytes received :" << bytes_received << std::endl ;

      if(strncmp(incoming_char_buffer, "HANGUP", 6) != 0) {

	myMutex.lock();
	int len = ctp7.processTCPMessage(incoming_data_buffer, 
					 outgoing_data_buffer, 
					 bytes_received,
					 MAX_BUFFER_SIZE);
	myMutex.unlock();

	std::cout << "Sending back a message of length ..." << len 
		  << std::endl;

	ssize_t bytes_sent = send(sd, outgoing_data_buffer, len, 0);
	std::cout << "bytes sent :" << bytes_sent << std::endl ;

      }
      else {
	close(sd);
	break;
      }
    }
    else if (bytes_received == 0) {
      std::cout << "host shut down." << std::endl ;
      close(sd);
      break;
    }
    else if (bytes_received == -1) {
      std::cout << "receive error!" << std::endl ;
      close(sd);
      break;
    }
  }
}

int main(int argc, char **argv)
{

  int status;
  struct addrinfo host_info; // The struct that getaddrinfo() fills up with data.
  struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

  // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
  // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
  // is created in c++, it will be given a block of memory. This memory is not nessesary
  // empty. Therefor we use the memset function to make sure all fields are NULL.
  memset(&host_info, 0, sizeof host_info);

  std::cout << "Setting up the structs..."  << std::endl;

  host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
  host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
  host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

  // Now fill up the linked list of host_info structs
  char port[256];
  strcpy(port, "5555");
  if(argc == 2) strcpy(port, argv[1]);
  status = getaddrinfo(NULL, port, &host_info, &host_info_list);
  // getaddrinfo returns 0 on succes, or some other value when an error occured.
  // (translated into human readable text by the gai_gai_strerror function).
  if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;


  std::cout << "Creating a socket..."  << std::endl;
  int socketfd ; // The socket descripter
  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
		    host_info_list->ai_protocol);
  if (socketfd == -1)  std::cout << "socket error " ;

  int a = 0x4000;
  if (setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, &a, sizeof(int)) == -1) {
    std::cerr << "Error setting socket opts" << std::endl;
  }
  if (setsockopt(socketfd, SOL_SOCKET, SO_SNDBUF, &a, sizeof(int)) == -1) {
    std::cerr << "Error setting socket opts" << std::endl;
  }
  
  std::cout << "Binding socket..."  << std::endl;
  // we use to make the setsockopt() function to make sure the port is 
  // not in use by a previous execution of our code. 
  // (see man page for more information)
  int yes = 1;
  status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1)  std::cout << "bind error" << std::endl ;

  std::cout << "Listen()ing for connections..."  << std::endl;
  status =  listen(socketfd, 5);
  if (status == -1)  std::cout << "listen error" << std::endl ;

  typedef void* (*PTHREAD_FUNC) (void *);
  pthread_t thread;

  while(true) {
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    int new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
      {
	std::cout << "listen error" << std::endl ;
      }
    else
      {
	std::cout << "Connection accepted. Using new socketfd : "  <<  new_sd 
		  << std::endl;
      }

    int status = pthread_create(&thread, NULL, PTHREAD_FUNC(server), &new_sd);
    if(status != 0) {
      std::cerr << "pthread_create returns " << status << std::endl;
      exit(EXIT_FAILURE);
    }

  }

  freeaddrinfo(host_info_list);
  close(socketfd);

  return 0 ;

}
