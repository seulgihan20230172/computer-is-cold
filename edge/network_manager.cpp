#include "network_manager.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>

#include "opcode.h"
using namespace std;

NetworkManager::NetworkManager() 
{
  this->sock = -1;
  this->addr = NULL;
  this->port = -1;
}

NetworkManager::NetworkManager(const char *addr, int port)
{
  this->sock = -1;
  this->addr = addr;
  this->port = port;
}

void NetworkManager::setAddress(const char *addr)
{
  this->addr = addr;
}

const char *NetworkManager::getAddress()
{
  return this->addr;
}

void NetworkManager::setPort(int port)
{
  this->port = port;
}

int NetworkManager::getPort()
{
  return this->port;
}

int NetworkManager::init()
{
	struct sockaddr_in serv_addr;

	this->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (this->sock == FAILURE)
  {
    cout << "[*] Error: socket() error" << endl;
    cout << "[*] Please try again" << endl;
    exit(1);
  }

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(this->addr);
	serv_addr.sin_port = htons(this->port);

	if (connect(this->sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == FAILURE)
  {
    cout << "[*] Error: connect() error" << endl;
    cout << "[*] Please try again" << endl;
    exit(1);
  }
	
  cout << "[*] Connected to " << this->addr << ":" << this->port << endl;

  return sock;
}

// TODO: You should revise the following code
int NetworkManager::sendData(uint8_t *data, int dlen)
{
  int sock, tbs, sent, offset;
  // int num, jlen;
  unsigned char opcode;
  uint8_t n[dlen];
  //uint8_t *p;

  sock = this->sock;
  // Example) data (processed by ProcessManager) consists of:
  // Example) min_temp(1 byte) || min_humid(1 byte) || min_power(2 byte) || monthcycle(2 byte) || discomfort_index(2 byte) || CDD(2 byte) || HDD(2 byte) || month(1 byte)
  // Example) edge -> server: opcode (OPCODE_DATA, 1 byte)
  opcode = OPCODE_DATA;
  tbs = 1; offset = 0;
  while (offset < tbs)
  {
    sent = write(sock, &opcode + offset, tbs - offset);
    if (sent > 0)
      offset += sent;
  }
  assert(offset == tbs);

  // Example) edge -> server
  tbs = dlen; offset = 0;
  memcpy(n, data,dlen); //save 8 length data to n
  while (offset < tbs)
  {
    sent = write(sock, n + offset, tbs - offset);
    if (sent > 0)
      offset += sent;
  }
  assert(offset == tbs);

  return 0;
}

// TODO: Please revise or implement this function as you want. You can also remove this function if it is not needed
uint8_t NetworkManager::receiveCommand() 
{
  int sock;
  uint8_t opcode;
  //uint8_t *p;

  sock = this->sock;
  opcode = OPCODE_WAIT;

  while (opcode == OPCODE_WAIT)
    read(sock, &opcode, 1);

  assert(opcode == OPCODE_DONE || opcode == OPCODE_QUIT) ;

  return opcode;
}

int main() {
    // Create a NetworkManager instance
    NetworkManager networkManager;

    // Set address and port
    const char* addr = "127.0.0.1"; // Example address
    int port = 8080; // Example port
    networkManager.setAddress(addr);
    networkManager.setPort(port);

    // Initialize and connect to the server
    int sock = networkManager.init();
    
    // Additional logic for sending/receiving data can be added here
    
    // Close the socket when done
    close(sock);

    return 0;
}
