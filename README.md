datasift
========

sample code for persistent buffer exercise

source code and eclipse .project files

executable program: mbuf

requires shared libraries libzmq, libboost_thread, libboost_system, libboost_program_options
(supplied in lib directory of repository. extend LD_LIBRARY_PATH to include this directory whn running mbuf)

compiled using g++ 4.8.2

tested usng the stream_generator program

To run:

./mbuf -i tcp://*:4242 -o tcp://*:4243 -b <name of buffer file> -s <size in Gbs of buffer file>


