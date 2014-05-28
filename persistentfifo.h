/*
 * persistentfifo.h
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#ifndef PERSISTENTFIFO_H_
#define PERSISTENTFIFO_H_

#include <string>

#include <boost/thread.hpp>
#include <fcntl.h>

class persistent_fifo
{
public:
  persistent_fifo( std::string const &filename, off_t max_off = 256*1024*1024);
  virtual
  ~persistent_fifo();

  int begin_store_message( size_t reserve_size);
  void commit_store_message();

  int retrieve_message( bool const dont_block = false);
  void consume_message();

private:

  boost::condition_variable _cond_empty;
  boost::condition_variable _cond_full;
  boost::mutex	_mutex;

  off_t	const _max_off;
  off_t	_reserve_end_off;

  // file descriptors for the underlying buffer file
  // order of construction is important so preserve order
  // of declaration
  int _fd;
  int _ofd;
  int _ifd;

  // persistent file header
  struct header
  {
    off_t _read_off;
    off_t _write_off;
    off_t _end_off;
    bool  _is_empty;

    header();
    bool read( int fd);
    void write( int fd);
  };

  header _header;


  void reserve_space( size_t size);
  bool is_space_in_buffer( size_t size);
  off_t get_start_off() {
    return sizeof _header;
  }
  bool is_space_in_buffer() {
    return _reserve_end_off <= _max_off;
  }
  void check_read_rewind();


  // defend inadvertent copying
  persistent_fifo( persistent_fifo const &);
  persistent_fifo &operator=( persistent_fifo const &);
};

#endif /* PERSISTENTFIFO_H_ */
