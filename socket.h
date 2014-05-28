/*
 * socket.h
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <zmqpp/zmqpp.hpp>

class socket
{
public:
  socket( zmqpp::socket_t &socket);
  virtual
  ~socket();

  void operator()();
  void terminate() { _running = false; }

protected:
  bool receive( zmqpp::message_t &message, bool const dont_block = false) {
    return _socket.receive( message, dont_block);
  }
  bool send( zmqpp::message_t &message, bool const dont_block = false) {
    return _socket.send( message, dont_block);
  }

private:
  bool             _running;
  zmqpp::socket_t &_socket;

  virtual
  bool initialize() {
    return true;
  }
  virtual
  void do_work() = 0;

  // defend inadvertent copying
  socket( socket const &);
  socket &operator=( socket const &);
};

#endif /* SOCKET_H_ */
