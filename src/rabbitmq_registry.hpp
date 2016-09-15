#ifndef _RABBITMQ_REGISTRY_HPP_
#define _RABBITMQ_REGISTRY_HPP_

#include <exception>
#include <sstream> 
#include <string> 
#include <map>


#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <kisscpp/logstream.hpp>

#include "string_set.hpp"
#include "rabbitmq_publisher.hpp"

typedef boost::shared_ptr<RabbitMQPublisher>                    SharedRabbitMQPublisher;
typedef std::map         <std::string, SharedRabbitMQPublisher> RabbitMQPublisherMap;

//--------------------------------------------------------------------------------
class UnRegisteredQueueNameException : public std::exception
{
  public:
             UnRegisteredQueueNameException()                              { message = "No registered queue for that name."; }
             UnRegisteredQueueNameException(const std::string &queue_name) { std::stringstream ss; ss << "No registered queue named: " << queue_name; message = ss.str(); }
    virtual ~UnRegisteredQueueNameException() throw()                      {}

    const char* what() const throw() { return message.c_str(); }

  private:
    std::string message;
};

//--------------------------------------------------------------------------------
class RabbitMQRegistry
{
  public:
     RabbitMQRegistry() {}
    ~RabbitMQRegistry() {}

    void add(const std::string &_queue_name,
             const std::string &_host,
             const int         &_port,
             const std::string &_username,
             const std::string &_password)
    {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);

      if(rabbitmq_publishers.find(_queue_name) == rabbitmq_publishers.end()) {
        SharedRabbitMQPublisher srmqp;
        srmqp.reset(new RabbitMQPublisher( _queue_name, _host, _port, _username, _password));
        rabbitmq_publishers[_queue_name] = srmqp;
      } else {
        log << "Already have details for queue: " << _queue_name << ". Not creating new queue item in registry." << kisscpp::manip::endl;
      }
    }

    void publish_message(const std::string &qname, const std::string &message)
    {
      if(rabbitmq_publishers.find(qname) != rabbitmq_publishers.end()) {
        rabbitmq_publishers[qname]->publish_message(message);
      } else {
        throw UnRegisteredQueueNameException(qname);
      } 
    }

  protected:
  private:
    RabbitMQPublisherMap rabbitmq_publishers;
};

#endif // _EVETNS_REGISTRY_HPP_
