// File  : main.cpp
// Author: Dirk J. Botha <bothadj@gmail.com>
// Date  : Thu Oct 29 13:24:36 SAST 2015

#include <boost/program_options.hpp>

#include "kem.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[])
{
  try {

    bpo::options_description desc("Options");

    desc.add_options()
      ("help,h"         , "Print help messages")
      ("console-mode,C" , "Start the application in console-mode")
      ("instance,I"     , bpo::value<std::string>()->required(), "Instance id for the process you wish to start up.");

    bpo::variables_map vm;

    try {
      bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm); // throws on error

      if(vm.count("help")) {
        std::cout << "Add some usage detail here." << std::endl;
        return 0;
      }

      bpo::notify(vm); // throws on error, so do after help in case there are any problems
    } catch(boost::program_options::required_option& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    } catch(boost::program_options::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

    KissEventManager app(vm["instance"].as<std::string>(), !vm.count("console-mode"));
    app.run();

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

