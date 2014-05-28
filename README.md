datasift
========

sample code for persistent buffer exercise

source code and eclipse .project files

executable program: mbuf

requires shared libraries libzmq, libboost_thread, libboost_system, libboost_program_options

tested usng the stream_generator program

To run:

./mbuf -i tcp://*:4242 -o tcp://*:4243 -b <name of buffer file> -s <size in Gbs of buffer file>


