/*
 * pushersocket.h
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#ifndef PUSHERSOCKET_H_
#define PUSHERSOCKET_H_

#include "socket.h"
#include "persistentfifo.h"

class pusher_socket : public socket
{
public:
  pusher_socket( zmqpp::socket_t &socket, persistent_fifo &fifo);
  virtual
  ~pusher_socket();

private:
  persistent_fifo &_fifo;

  void do_work();
  void read_message( int fd, zmqpp::message_t &message);

};

#endif /* PUSHERSOCKET_H_ */
