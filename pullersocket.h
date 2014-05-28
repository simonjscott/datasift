/*
 * pullersocket.h
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#ifndef PULLERSOCKET_H_
#define PULLERSOCKET_H_

#include "socket.h"
#include "persistentfifo.h"

class puller_socket : public socket
{
public:
  puller_socket( zmqpp::socket_t &socket, persistent_fifo &fifo);
  virtual
  ~puller_socket();

private:
  persistent_fifo &_fifo;

  bool initialize();
  void do_work();
  size_t calc_message_size( zmqpp::message_t const &message);
  void write_message( int fd, zmqpp::message_t const &message);
};

#endif /* PULLERSOCKET_H_ */
