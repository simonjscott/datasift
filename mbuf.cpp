/*
 * mbuf.cpp
 *
 *  Created on: 23 May 2014
 *      Author: simon
 */

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include <zmqpp/zmqpp.hpp>

#include "pullersocket.h"
#include "pushersocket.h"

namespace po = boost::program_options;

class mbuf {
public:
	mbuf();
	virtual ~mbuf();
	int run( int argc, char *argv[]);

private:
	int read_options( int argc, char *argv[]);
	std::string _input_endpoint;
	std::string _output_endpoint;
	std::string _filename;
	int	_size_gb;
};

mbuf::mbuf()
{
}

mbuf::~mbuf()
{
}

int mbuf::read_options( int argc, char *argv[])
{
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h",     "produce help message")
    ("input,i",  po::value<std::string>(), "set which endpoint to connect to on input side of buffer")
    ("output,o", po::value<std::string>(), "set which endpoint to connect to on output side of buffer")
    ("buffer,b", po::value<std::string>(), "name of persistent buffer file")
    ("size,s", po::value<int>()->default_value(2), "maximum size of buffer in Gb")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("input"))
  {
    _input_endpoint =  vm["input"].as< std::string >();
    std::cout << "input endpoint: " << _input_endpoint << std::endl;
  }
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("output"))
  {
    _output_endpoint = vm["output"].as< std::string >();
    std::cout << "output endpoint: " <<  _output_endpoint << std::endl;
  }
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("buffer"))
  {
    _filename = vm["buffer"].as< std::string >();
    std::cout << "buffer: " <<  _filename << std::endl;
  }
  else
  {
      std::cout << desc << std::endl;
      return 1;
  }


  if (vm.count("size"))
  {
    _size_gb = vm["size"].as<int>();
    std::cout << "size (Gb): " <<  _size_gb << std::endl;
  }
  else
    {
      _size_gb = 2;
    }

  return 0;
}

int mbuf::run( int argc, char **argv)
{
  std::cout << "hi there" << std::endl;

  int result = read_options( argc, argv);
  if (0 != result)
  {
    return result;
  }

  try
  {
    zmqpp::context_t context;

    zmqpp::socket_t input_socket( context,  zmqpp::socket_type::pull);
    zmqpp::socket_t output_socket( context, zmqpp::socket_type::push);

    input_socket.bind( _input_endpoint);
    output_socket.bind( _output_endpoint);


    persistent_fifo fifo( _filename, static_cast<off_t>( _size_gb)*1024*1024*1024);

    puller_socket pullt( input_socket, fifo);
    pusher_socket pusht( output_socket, fifo);

    std::cout << "spawning worker threads" << std::endl;

    std::cout << "type something followed by <return> to exit" << std::endl;
    boost::thread puller( boost::ref(pullt));
    boost::thread pusher( boost::ref(pusht));

    std::string dummy;
    std::cin >> dummy;

    pullt.terminate();
    pusht.terminate();

    puller.join();
    pusher.join();
  }
  catch(std::exception const & e)
  {
    std::cerr << "caught std::exception, reason: " << e.what() << std::endl;
    result = 1;
  }


  return result;
}

int main( int argc, char *argv[])
{
  mbuf main;
  return main.run( argc, argv);
}
