/*
 * pullersocket.cpp
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#include "pullersocket.h"

puller_socket::puller_socket( zmqpp::socket_t &skt, persistent_fifo &fifo)
: socket( skt)
, _fifo( fifo)
{
}

puller_socket::~puller_socket ()
{
}

bool
puller_socket::initialize()
{
//  nice( 10);
  return true;
}

void
puller_socket::do_work()
{
  zmqpp::message_t message;
  receive( message);

  size_t message_size = calc_message_size( message);

  int fd = _fifo.begin_store_message( message_size);

  write_message( fd, message);

  _fifo.commit_store_message();

  sched_yield();
}

size_t
puller_socket::calc_message_size( zmqpp::message_t const &message)
{
  size_t number_of_parts = message.parts();
  size_t message_size = sizeof number_of_parts;
  for ( size_t ii = 0; ii < number_of_parts; ++ii)
  {
    size_t part_size = message.size( ii);
    message_size += sizeof part_size + part_size;
  }
  return message_size;
}

void
puller_socket::write_message( int fd, zmqpp::message_t const &message)
{
  size_t number_of_parts = message.parts();

  ::write( fd, &number_of_parts, sizeof number_of_parts);

  for ( size_t ii = 0; ii < number_of_parts; ++ii)
  {
    size_t part_size = message.size( ii);
    ::write( fd, &part_size, sizeof part_size);;
    ::write( fd, message.raw_data( ii), part_size);
  }
}

