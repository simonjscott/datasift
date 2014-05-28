/*
 * persistentfifo.cpp
 *
 *  Created on: 27 May 2014
 *      Author: simon
 */

#include "persistentfifo.h"

persistent_fifo::persistent_fifo( std::string const &filename, off_t max_off)
: _max_off( max_off)
, _reserve_end_off( 0)
, _fd( ::open( filename.c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR))
, _ofd( ::open( filename.c_str(), O_WRONLY))
, _ifd( ::open( filename.c_str(), O_RDONLY))
{
  if (-1 == _fd || -1 == _ofd || -1 == _ifd)
    {
      std::stringstream	ss;
      ss << "failed to open file: " << strerror( errno) << std::endl;
      throw std::runtime_error( ss.str());
    }

  if (!_header.read( _fd))
    {
      _header.write( _fd);
    }

  if (-1 == ::lseek( _ofd, _header._write_off, SEEK_SET)
      || -1 == ::lseek( _ifd, _header._read_off, SEEK_SET))
    {
      std::stringstream	ss;
      ss << "unexpected problem with file: " << strerror( errno) << std::endl;
      throw std::runtime_error( ss.str());
    }
}

persistent_fifo::~persistent_fifo()
{
}

persistent_fifo::header::header()
: _read_off( sizeof *this)
, _write_off( sizeof *this)
, _end_off( -1l)
, _is_empty( true)
{
}

bool
persistent_fifo::header::read( int fd)
{
  return -1 != ::read( fd, this, sizeof *this);
}

void
persistent_fifo::header::write( int fd)
{
  if (-1 == ::lseek( fd, 0, SEEK_SET)
      || -1 == ::write( fd, this, sizeof *this))
    {
      std::stringstream	ss;
      ss << "failed to write file: " << strerror( errno) << std::endl;
      throw std::runtime_error( ss.str());
    }
}

int
persistent_fifo::begin_store_message( size_t reserve_size)
{
  reserve_space( reserve_size);

  if (!is_space_in_buffer())
    {
      // must rewind to beginning of file
      _header._end_off = _header._write_off;
      _header._write_off = get_start_off();
      ::lseek( _ofd, get_start_off(), SEEK_SET);
      _header.write( _fd);

      // now must wait for space to be available there
      reserve_space( reserve_size);
      std::cout << "rewind writer: " << lseek( _ofd, 0, SEEK_CUR)
	  << "end_off: " << _header._end_off
	  << "read_off: " << _header._read_off << std::endl;
    }
  return _ofd;
}

void
persistent_fifo::reserve_space( size_t size)
{
  _reserve_end_off = _header._write_off + static_cast<off_t>( size);

  // may need to block here while wait for reader to catch up
  boost::unique_lock<boost::mutex> lock( _mutex);
  while (_reserve_end_off > _header._read_off &&
      _header._write_off <= _header._read_off && !_header._is_empty)
    {
      _cond_full.wait( lock);
    }
}


void
persistent_fifo::commit_store_message()
{
  off_t off = ::lseek( _ofd, 0, SEEK_CUR);
  if (off > _reserve_end_off)
    {
      throw std::logic_error( "message overrun");
    }

  boost::lock_guard<boost::mutex> lock( _mutex);
  _header._write_off = off;
  _header._is_empty = false;
  _header.write( _fd);
  _cond_empty.notify_one();
}

int
persistent_fifo::retrieve_message( bool const dont_block)
{
  boost::unique_lock<boost::mutex> lock( _mutex);
  while (_header._is_empty)
    {
      if (dont_block) {
	  return -1;
      }
      _cond_empty.wait( lock);
    }

  // while we were waiting the writer may have rewound
  check_read_rewind();

  return _ifd;
}

void
persistent_fifo::consume_message()
{
  boost::unique_lock<boost::mutex> lock( _mutex);
  _header._read_off = ::lseek( _ifd, 0, SEEK_CUR);
  check_read_rewind();
  if (_header._read_off == _header._write_off)
    {
      _header._is_empty = true;
    }
  _header.write( _fd);

  _cond_full.notify_one();
}

void
persistent_fifo::check_read_rewind()
{
  if (_header._read_off == _header._end_off)
    {
      _header._read_off = ::lseek( _ifd, get_start_off(), SEEK_SET);
      std::cout << "rewind reader: " << lseek( _ifd, 0, SEEK_CUR) << std::endl;
      _header.write( _fd);
    }
}
