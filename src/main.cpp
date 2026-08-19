#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  /*This syscall(socket) will return a number to the program k/a "file
   * descriptor". The line below mcan be read as - " Create an IPV4 TCP socket
   * using default protocol"*/

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  /*Checking for failure
   * std::cerr is CPP's standard error stream.
   * Errno is a special error indicator used by syscalls.
   * std::strerror(errno) converts the error number into human readable
   * message.*/

  if (server_fd == -1) {
    std::cerr << "socket() failed: " << std::strerror(errno) << '\n';
    return 1; // socket couldn't be created, so return 1
  }

  // Now, we describe the address we want to bind to
  /* sockaddr_in is a structure provided by the socket API for describing an
   * IPV4 address. sin_family -> IPV4 sin_port -> 8080 sin_addr.s_addr ->
   * 0.0.0.0 other fields -> 0
   * INADDR_ANY -> means bind to all local IPv4 interfaces.
   * htons(8080) converts the 16 bit value 8080 from host byte order to network
   * byte order*/

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(8080);

  // bind socket to 0.0.0.0:8080

  /* Associate the socket with the desired address.
   * reinterpret_cast<sockaddr(star)>(&server_address) -> means "treat this IPv4
   * address structure as generic socket-address type expected by bind()".
   * server_address type is sockaddr_in, but bind() expects sockaddr(star). We
   * aren't converting or copying anything. Just changing how the pointer is
   * interpreted*/

  if (bind(server_fd, reinterpret_cast<sockaddr *>(&server_address),
           sizeof(server_address)) == -1) {
    std::cerr << "bind() failed" << std::strerror(errno) << '\n';
    /* As we can see, bind() has 3 arguments. bind(which socket?, what address?,
     * size of address?)*/

    close(server_fd);
    return 1;
  }
  // Now, start listening for incoming connection
  /*The first argument in listen() is the file descriptor, and the second
   * argument is called 'backlog'. It controls how many incoming connections the
   * kernel can queue while the application isn't calling accept() enough.*/
  if (listen(server_fd, 10) == -1) {
    std::cerr << "listen() failed: " << std::strerror(errno) << '\n';

    close(server_fd);
    return 1;
  }
  std::cout << "Listening on port 8080...\n";

  // Wait for a client
  /* accept() is a blocking call. If no client has connected, program stops at
   * accept() and wait. accept() gives us a new socket specifically for
   * communicating with one client.
   * accept() needs client_address_length because it receives a generic address
   * structure (sockaddr(star)).*/
  sockaddr_in client_address{};
  socklen_t client_address_length = sizeof(client_address);

  int client_fd =
      accept(server_fd, reinterpret_cast<sockaddr *>(&client_address),
             &client_address_length);

  if (client_fd == -1) {
    std::cerr << "accept() failed: " << std::strerror(errno) << '\n';

    close(server_fd);
    return 1;
  }
  std::cout << "Client connected\n";

  // closing the client connection

  close(client_fd);
  close(server_fd);

  return 0;
}
