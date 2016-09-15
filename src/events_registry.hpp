#ifndef _EVETNS_REGISTRY_HPP_
#define _EVETNS_REGISTRY_HPP_

#include <exception>
#include <sstream> 
#include <string> 
#include <map>

#include "subscriber.hpp"
#include "rabbitmq_registry.hpp"

//--------------------------------------------------------------------------------
class EmptySubscriberSetException : public std::exception
{
  public:
             EmptySubscriberSetException()                              { message = "No subscribers for event."; }
             EmptySubscriberSetException(const std::string &event_name) { std::stringstream ss; ss << "No subscribers for event: " << event_name; message = ss.str(); }
    virtual ~EmptySubscriberSetException() throw()                      {}

    const char* what() const throw() { return message.c_str(); }

  protected:
    std::string message;

  private:
};

//--------------------------------------------------------------------------------
class EmptyKSubscriberSetException : public EmptySubscriberSetException
{
  public:
    EmptyKSubscriberSetException()                              { message = "No KISSCPP subscribers for event."; }
    EmptyKSubscriberSetException(const std::string &event_name) { std::stringstream ss; ss << "No KISSCPP subscribers for event: " << event_name; message = ss.str(); }
};

//--------------------------------------------------------------------------------
class EmptyQSubscriberSetException : public EmptySubscriberSetException
{
  public:
    EmptyQSubscriberSetException()                              { message = "No RABBITMQ subscribers for event."; }
    EmptyQSubscriberSetException(const std::string &event_name) { std::stringstream ss; ss << "No RABBITMQ subscribers for event: " << event_name; message = ss.str(); }
};

//--------------------------------------------------------------------------------
class EventsRegistry
{
  public:
     EventsRegistry() {}
    ~EventsRegistry() {}

    void subscribe_kcpp (const std::string &eventName, const std::string &id) { k_subscribers[eventName].insert(id); };
    void subscribe_queue(const std::string &eventName, const std::string &id) { q_subscribers[eventName].insert(id); };

    StringSet getEventKSubscribers(const std::string &eventName)
    {
      if(k_subscribers.find(eventName) != k_subscribers.end()) {
        return k_subscribers[eventName];
      } else {
        throw EmptyKSubscriberSetException(eventName);
      }
    };

    StringSet getEventQSubscribers(const std::string &eventName)
    {
      if(q_subscribers.find(eventName) != q_subscribers.end()) {
        return q_subscribers[eventName];
      } else {
        throw EmptyQSubscriberSetException(eventName);
      }
    };


  protected:
  private:
    std::map<std::string, StringSet> k_subscribers;
    std::map<std::string, StringSet> q_subscribers;
};

#endif // _EVETNS_REGISTRY_HPP_
