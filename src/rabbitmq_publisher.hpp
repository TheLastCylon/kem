#ifndef _RABBITMQ_PUBLISHER_HPP_
#define _RABBITMQ_PUBLISHER_HPP_

#include <string>
#include <sstream>
#include <stdexcept>

#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include <kisscpp/logstream.hpp>

#include "cfg.hpp"
#include "stat.hpp"

//--------------------------------------------------------------------------------
class AMQPError : public std::runtime_error
{
  public:
    explicit AMQPError(const std::string& msg) : std::runtime_error(msg) {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);
      log << kisscpp::manip::debug_normal << msg << kisscpp::manip::endl;
    }
    virtual ~AMQPError() throw() {};
  protected:
  private:
};

//--------------------------------------------------------------------------------
class AMQPResponseNone             : public AMQPError {
  public:
    explicit AMQPResponseNone(const std::string &context) : AMQPError("[" + context + "] Missing RPC reply type!") {};
};

//--------------------------------------------------------------------------------
class AMQPResponseLibraryException : public AMQPError {
  public:
    explicit AMQPResponseLibraryException(const std::string &context, amqp_rpc_reply_t x) : AMQPError("[" + context + "] " + amqp_error_string2(x.library_error)) {};
};

//--------------------------------------------------------------------------------
class AMQPResponseServerException  : public AMQPError {
  public:
    explicit AMQPResponseServerException(const std::string &context, std::string message) : AMQPError("[" + context + "]" + message) {};
};

//--------------------------------------------------------------------------------
class RabbitMQPublisher
{
  public:
     RabbitMQPublisher(const std::string &_queue_name,
                       const std::string &_host,
                       const int         &_port,
                       const std::string &_username,
                       const std::string &_password);

    ~RabbitMQPublisher();

    std::string getQueue_name() const { return queue_name; }
    std::string getHost      () const { return host; }
    int         getPort      () const { return port; }
    std::string getUsername  () const { return username; }
    std::string getPassword  () const { return password; }

//    void setQueue_name(const std::string &val) { queue_name = val; }
//    void setHost      (const std::string &val) { host       = val; }
//    void setPort      (const int         &val) { port       = val; }
//    void setUsername  (const std::string &val) { username   = val; }
//    void setPassword  (const std::string &val) { password   = val; }

    void publish_message(const std::string &message);

  protected:
    void throw_on_amqp_error(amqp_rpc_reply_t x, const std::string &context);
    void throw_on_error     (int              x, const std::string &context);
    void connect            ();
    void disconnect         ();

  private:
    RabbitMQPublisher() {};

    std::string             queue_name;
    std::string             host;
    int                     port;
    std::string             username;
    std::string             password;

    int                     status;
    int                     message_count;
    bool                    connected;
    amqp_socket_t          *socket = NULL;
    amqp_connection_state_t conn;
};

#endif //_RABBIT_QUEUE_HPP_
