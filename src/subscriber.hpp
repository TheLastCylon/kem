#ifndef _SUBSCRIBER_HPP_
#define _SUBSCRIBER_HPP_

#include <exception>
#include <sstream> 
#include <string> 
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include "string_set.hpp"

//--------------------------------------------------------------------------------
class Subscriber
{
  public:
     Subscriber(const std::string &h, const std::string &p)
     {
       setHost(h);
       setPort(p);
       setId  (h+p);
     };

     Subscriber(const std::string &h, const std::string &p, const std::string &i)
     {
       setHost(h);
       setPort(p);
       setId  (i);
     };

     Subscriber(Subscriber &s)
     {
       host = s.getHost();
       port = s.getPort();
       id   = s.getId  ();
     };

    ~Subscriber() {};

    std::string getHost() const                 { return host; }
    std::string getPort() const                 { return port; }
    std::string getId  () const                 { return id;   }

    void        setHost(const std::string &val) { host = boost::algorithm::to_lower_copy(val); }
    void        setPort(const std::string &val) { port = boost::algorithm::to_lower_copy(val); }
    void        setId  (const std::string &val) { id   = boost::algorithm::to_lower_copy(val); }

  protected:

  private:
    Subscriber() : host(""), port(""), id("") {};

    std::string host;
    std::string port;
    std::string id;
};

typedef boost::shared_ptr<Subscriber>                    SharedSubscriber;
typedef std::set         <std::string>                   SubscriberSet;
typedef std::set         <std::string>::iterator         SubscriberSetIter;
typedef boost::shared_ptr<StringSet>                     SharedSubscriberSet;
typedef std::map         <std::string, SharedSubscriber> SubscriberMap;


//--------------------------------------------------------------------------------
class SubscriberNotFoundException : public std::exception
{
  public:
             SubscriberNotFoundException()                                 { message = "No such subscriber."; }
             SubscriberNotFoundException(const std::string &subscriber_id) { std::stringstream ss; ss << "No such subscriber: " << subscriber_id; message = ss.str(); }
    virtual ~SubscriberNotFoundException() throw()                         {}

    const char* what() const throw() { return message.c_str(); }

  private:
    std::string message;
};

//--------------------------------------------------------------------------------
class Subscribers
{
  public:
     Subscribers() {};
    ~Subscribers() {};

    std::string add(const std::string &host,
                    const std::string &port)
    {
      SharedSubscriber ss;

      ss.reset(new Subscriber(host, port));

      subscribers[ss->getId()] = ss;

      return subscribers[ss->getId()]->getId();
    };

    std::string getSubscriberHost(const std::string &subscriber_id) { if(subscribers.find(subscriber_id) != subscribers.end()) { return subscribers[subscriber_id]->getHost(); } else { throw SubscriberNotFoundException(subscriber_id); } }
    std::string getSubscriberPort(const std::string &subscriber_id) { if(subscribers.find(subscriber_id) != subscribers.end()) { return subscribers[subscriber_id]->getPort(); } else { throw SubscriberNotFoundException(subscriber_id); } }

  protected:
  private:
    SubscriberMap subscribers;
};

#endif// _SUBSCRIBER_HPP_
