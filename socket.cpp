/*
 * socket.cpp
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#include "socket.h"

socket::socket( zmqpp::socket_t &socket)
: _socket( socket)
, _running( true)
{
}

socket::~socket ()
{
}

void
socket::operator()()
{
  while (_running)
  {
    do_work();
  }
}
