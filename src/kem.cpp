// File  : kem.cpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#include "kem.hpp"

//--------------------------------------------------------------------------------
KissEventManager::KissEventManager(const std::string &instance,
                                   const bool        &runAsDaemon) :
  Server           (1, "kem", instance, runAsDaemon),
  queue_dir        (CFG->get<std::string >("app-queue-dir")),
  queue_page_length(CFG->get<unsigned int>("queue-page-length",1000)),
  thread_count     (CFG->get<unsigned int>("thread-count",1)),
  running          (true)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  log.setMaxBufferSize(0);


  constructQueues ();
  registerHandlers();
  loadSubscribers ();
  startThreads    ();
}

//--------------------------------------------------------------------------------
KissEventManager::~KissEventManager()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  running = false;
  stop();
  threadGroup.join_all();
}

//--------------------------------------------------------------------------------
void KissEventManager::constructQueues()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  q_events      .reset(new SafeEventQ("q_events"      , queue_dir, queue_page_length));
  q_events_error.reset(new SafeEventQ("q_events_error", queue_dir, queue_page_length));

  statQue("queues.q_events"      , q_events);
  statQue("queues.q_events_error", q_events_error);
}

//--------------------------------------------------------------------------------
void KissEventManager::registerHandlers()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  publishHandler  .reset(new PublishHandler  (q_events));
  subscribeHandler.reset(new SubscribeHandler(events_registry, subscribers)); 

  register_handler(publishHandler  );
  register_handler(subscribeHandler);
}

//--------------------------------------------------------------------------------
void KissEventManager::startThreads()
{
  for(unsigned int i = thread_count; i > 0; --i) {
    threadGroup.create_thread(boost::bind(&KissEventManager::processor_event_notifications, this));
  }
}

//--------------------------------------------------------------------------------
void KissEventManager::loadSubscribers()
{
  kisscpp::LogStream          log(__PRETTY_FUNCTION__);
  boost::property_tree::ptree subscribers_kisscpp;
  boost::property_tree::ptree subscribers_rabbitmq;
  std::string                 kisscpp_file  = CFG->get<std::string>("subscribers.kisscpp.config_file" ,"");
  std::string                 rabbitmq_file = CFG->get<std::string>("subscribers.rabbitmq.config_file","");

  if(kisscpp_file != "") {
    read_json(kisscpp_file, subscribers_kisscpp);
   
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, subscribers_kisscpp.get_child("subscribers")) {
      std::string event_name    = v.second.get<std::string>("event-name");
      std::string subscriber_id = subscribers.add(v.second.get<std::string>("host"), v.second.get<std::string>("port")); 
   
      events_registry.subscribe_kcpp(event_name, subscriber_id);
   
      log << "Added KISSCPP subscriber [" << subscriber_id << "] to event [" << event_name << "]" << kisscpp::manip::endl;
    }
  }

  if(rabbitmq_file != "") {
    read_json(rabbitmq_file, subscribers_rabbitmq);
   
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, subscribers_rabbitmq.get_child("subscribers")) {
      std::string event_name = v.second.get<std::string>("event-name");
      std::string queue_name = v.second.get<std::string>("queue_name");
   
      rabbitmq_registry.add(queue_name,
                            v.second.get<std::string>("host"),
                            v.second.get<int        >("port"),
                            v.second.get<std::string>("username"),
                            v.second.get<std::string>("password"));
   
      events_registry.subscribe_queue(event_name, queue_name);
   
      log << "Added RABBITMQ queue [" << queue_name << "] to event [" << event_name << "]" << kisscpp::manip::endl;
    }
  }
}

//--------------------------------------------------------------------------------
void KissEventManager::processor_event_notifications()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  SharedEvent        event;
  std::string        event_name;
  std::string        event_payload;
  std::string        event_payload_decode;
  StringSet          k_subscribers2notify;
  StringSet          k_subscribers;
  StringSet          q_subscribers2notify;
  StringSet          q_subscribers;
  bool               push_back_on_events = false;
  bool               push_back_on_error  = false;

  while(running) {
    if(q_events->empty()) {
      sleep(1);
    } else {
      try {
        event                = q_events->pop();
        event_name           = event->getName();

        try {
          k_subscribers        = events_registry.getEventKSubscribers(event_name);
          k_subscribers2notify = event->getSubscribers2Notify(k_subscribers);
          event_payload        = event->getPayload();

          for(StringSetIter itr = k_subscribers2notify.begin(); itr != k_subscribers2notify.end(); ++itr) {
            BoostPtree event_notification;
            BoostPtree event_notification_response;
         
            std::string thost = subscribers.getSubscriberHost(*itr);
            std::string tport = subscribers.getSubscriberPort(*itr);
         
            event_notification.put("kcm-cmd"      , "kem-event");
            event_notification.put("kcm-hst"      , thost);
            event_notification.put("kcm-prt"      , tport);
            event_notification.put("event.name"   , event_name);
            event_notification.put("event.payload", event_payload);
         
            try {
              log << "About to do client call. Host [" << thost << "] Port [" << tport << "]" << kisscpp::manip::endl;
              kisscpp::client requestSender(event_notification, &event_notification_response, 5);
              log << "Client call successfull." << kisscpp::manip::endl;
              event->addNotifiedSubscriber(*itr);
            } catch(kisscpp::RetryableCommsFailure &e) { log << "Retryable comms failure: " << e.what() << kisscpp::manip::endl; push_back_on_events = true;
            } catch(kisscpp::PerminantCommsFailure &e) { log << "Perminant comms failure: " << e.what() << kisscpp::manip::endl; push_back_on_error  = true; //q_events_error->push(event);
            } catch(std::exception                 &e) { log << "Exception: "               << e.what() << kisscpp::manip::endl; push_back_on_error  = true; //q_events_error->push(event);
            }
          }
        } catch(EmptyKSubscriberSetException &e) { log << "Exception: " << e.what() << kisscpp::manip::endl; }

        try {
          q_subscribers        = events_registry.getEventQSubscribers(event_name); 
          q_subscribers2notify = event->getQueues2Notify     (q_subscribers);
          event_payload_decode = event->getPayloadBase64Decode();

          for(StringSetIter itr = q_subscribers2notify.begin(); itr != q_subscribers2notify.end(); ++itr) {
            try {
              rabbitmq_registry.publish_message(*itr, event_payload_decode);
              event->addNotifiedQueue(*itr);
            } catch(kisscpp::RetryableCommsFailure &e) { log << "Retryable comms failure: " << e.what() << kisscpp::manip::endl; push_back_on_events = true;
            } catch(kisscpp::PerminantCommsFailure &e) { log << "Perminant comms failure: " << e.what() << kisscpp::manip::endl; push_back_on_error  = true; //q_events_error->push(event);
            } catch(std::exception                 &e) { log << "Exception: "               << e.what() << kisscpp::manip::endl; push_back_on_error  = true; //q_events_error->push(event);
            }
          }
        } catch(EmptyQSubscriberSetException &e) { log << "Exception: " << e.what() << kisscpp::manip::endl; }

      } catch(std::exception &e) { log << "Exception: " << e.what() << kisscpp::manip::endl; push_back_on_error  = true; //q_events_error->push(event);
      }

      if     (push_back_on_events) { q_events      ->push(event); }
      else if(push_back_on_error ) { q_events_error->push(event); }
    }
    sleep(0);
  }
}
