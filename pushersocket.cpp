/*
 * pushersocket.cpp
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#include "pushersocket.h"

pusher_socket::pusher_socket( zmqpp::socket_t &skt, persistent_fifo &fifo)
: socket( skt)
, _fifo( fifo)
{
}

pusher_socket::~pusher_socket ()
{
}

void
pusher_socket::do_work()
{
  int fd = _fifo.retrieve_message();

  zmqpp::message_t message;
  read_message( fd, message);
  if (send( message))
    {
      _fifo.consume_message();
    }
}

void
pusher_socket::read_message( int fd, zmqpp::message_t &message)
{
  off_t offset = ::lseek( fd, 0, SEEK_CUR);
  try
  {
    size_t number_of_parts;

    ::read( fd, &number_of_parts, sizeof number_of_parts);

    if (number_of_parts != 3)
      {
	throw std::runtime_error("bad read");
      }

    std::string buf;
    for ( size_t ii = 0; ii < number_of_parts; ++ii)
    {
      size_t part_size;
      ::read( fd, &part_size, sizeof part_size);
      buf.resize( part_size);
      ::read( fd, &buf[0], part_size);

      message.add( buf, part_size);
    }
  } catch (std::exception const &x) {
      std::cout << "read from: " << offset << std::endl;
      throw;
  }
}
