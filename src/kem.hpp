// File  : kem.hpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#ifndef _KEM_HPP_
#define _KEM_HPP_

#include <map>
#include <sstream>

#include <boost/thread.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/logic/tribool.hpp>

#include <kisscpp/server.hpp>
#include <kisscpp/client.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/ptree_queue.hpp>
#include <kisscpp/configuration.hpp>

#include "cfg.hpp"
#include "stat.hpp"

#include "event.hpp"
#include "handler_publish.hpp"
#include "handler_subscribe.hpp"
#include "subscriber.hpp"
#include "events_registry.hpp"

class KissEventManager : public kisscpp::Server
{
  public:
     KissEventManager(const std::string &instance,
                      const bool        &runAsDaemon);

    ~KissEventManager();

  protected:
    void processor_event_notifications();
    void constructQueues              ();
    void registerHandlers             ();
    void startThreads                 ();
    void loadSubscribers              ();

  private:
    SharedSafeEventQ           q_events;
    SharedSafeEventQ           q_events_error;
    std::string                queue_dir;
    unsigned int               queue_page_length;
    unsigned int               thread_count;
    bool                       running;

    EventsRegistry             events_registry;
    Subscribers                subscribers;

    kisscpp::RequestHandlerPtr publishHandler;
    kisscpp::RequestHandlerPtr subscribeHandler;
    boost::thread_group        threadGroup;
};

#endif //_KEM_HPP_

