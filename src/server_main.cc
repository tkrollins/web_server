//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "server.h"
#include "config_parser.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.

    // create a server object, s, initialized with io_serice, 
    // and the port number entered in the command line
    // server s(io_service, atoi(argv[1])); // this line can specify port directly from command line
    
    NginxConfigParser parser;

    std::string listenPort;
    if(parser.Parse(argv[1]))
    {
      listenPort = parser.config->parameters[configParameters::LISTEN_PORT];
    }
    cout << "Listening Port: " << listenPort << endl;

    for (int i = 0; i < listenPort.length(); i++)
    {
      if (!(listenPort[i] >= '0' && listenPort[i] <= '9')) throw 11;
    }
    int intListenPort = stoi(listenPort);
    if (intListenPort >= 65536 || intListenPort < 0)
    {
      throw 10; // probably should return a exceptions class......
    }

    server s(io_service, intListenPort);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
