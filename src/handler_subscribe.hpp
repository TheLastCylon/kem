// File  : handler_subscribe.hpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#ifndef _HANDLER_SUBSCRIBE_HPP_
#define _HANDLER_SUBSCRIBE_HPP_

#include <sstream>
#include <string>
#include <ctime>

#include <boost/algorithm/string.hpp>

#include <kisscpp/ptree_queue.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/request_handler.hpp>
#include <kisscpp/request_status.hpp>
#include <kisscpp/statskeeper.hpp>

#include "events_registry.hpp"
#include "subscriber.hpp"

class SubscribeHandler : public kisscpp::RequestHandler
{
  public:
     SubscribeHandler(EventsRegistry &er,
                      Subscribers    &s) :
       kisscpp::RequestHandler("subscribe", "Used to add an application to the notification list of an event."),
       events_registry(er),
       subscribers(s)
     {
       kisscpp::LogStream log(__PRETTY_FUNCTION__);
     };

    ~SubscribeHandler() {};

    void run(const BoostPtree& request, BoostPtree& response);

  protected:

  private:
    EventsRegistry events_registry;
    Subscribers    subscribers;
};

#endif // _HANDLER_SUBSCRIBE_HPP_

