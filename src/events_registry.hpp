#ifndef _EVETNS_REGISTRY_HPP_
#define _EVETNS_REGISTRY_HPP_

#include <exception>
#include <sstream> 
#include <string> 
#include <map>

#include "subscriber.hpp"

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
class EventsRegistry
{
  public:
     EventsRegistry() {}
    ~EventsRegistry() {}

    void subscribe_kcpp (const std::string &eventName, const std::string &id) { k_subscribers[eventName].insert(id); };

    StringSet getEventKSubscribers(const std::string &eventName)
    {
      if(k_subscribers.find(eventName) != k_subscribers.end()) {
        return k_subscribers[eventName];
      } else {
        throw EmptyKSubscriberSetException(eventName);
      }
    };

  protected:
  private:
    std::map<std::string, StringSet> k_subscribers;
};

#endif // _EVETNS_REGISTRY_HPP_
