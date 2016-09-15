// File  : handler_publish.hpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#ifndef _HANDLER_PUBLISH_HPP_
#define _HANDLER_PUBLISH_HPP_

#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>

#include <kisscpp/ptree_queue.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/request_handler.hpp>
#include <kisscpp/request_status.hpp>
#include <kisscpp/statskeeper.hpp>

#include "event.hpp"
#include "events_registry.hpp"

class PublishHandler : public kisscpp::RequestHandler
{
  public:
     PublishHandler(SharedSafeEventQ eq) :
       kisscpp::RequestHandler("publish", "Used to publish events.")
     {
       kisscpp::LogStream log(__PRETTY_FUNCTION__);
       q_events = eq;
     };

    ~PublishHandler() {};

    void run(const BoostPtree& request, BoostPtree& response);

  protected:

  private:
    SharedSafeEventQ q_events;
};

#endif // _HANDLER_PUBLISH_HPP_
