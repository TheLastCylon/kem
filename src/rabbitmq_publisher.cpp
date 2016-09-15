#include "rabbitmq_publisher.hpp"

//--------------------------------------------------------------------------------
RabbitMQPublisher::RabbitMQPublisher(const std::string  &_queue_name,
                                     const std::string  &_host,
                                     const int          &_port,
                                     const std::string  &_username,
                                     const std::string  &_password) :
  queue_name(_queue_name),
  host      (_host      ),
  port      (_port      ),
  username  (_username  ),
  password  (_password  ),
  connected (false)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  connect();
}

//--------------------------------------------------------------------------------
RabbitMQPublisher::~RabbitMQPublisher()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  disconnect();
}

//--------------------------------------------------------------------------------
void RabbitMQPublisher::connect()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  conn   = amqp_new_connection();
  socket = amqp_tcp_socket_new(conn);

  if(!socket) { throw AMQPError("Could not create socket"); }

  status = amqp_socket_open(socket, host.c_str(), port);

  if (status)  { throw AMQPError("Could not open socket"); }

  throw_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username.c_str(), password.c_str()), "Logging in");

  amqp_channel_open(conn, 1);

  throw_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");
  connected = true;
}

//--------------------------------------------------------------------------------
void RabbitMQPublisher::disconnect()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  connected = false;
  throw_on_amqp_error(amqp_channel_close     (conn, 1, AMQP_REPLY_SUCCESS), "Closing channel"   );
  throw_on_amqp_error(amqp_connection_close  (conn, AMQP_REPLY_SUCCESS)   , "Closing connection");
  throw_on_error     (amqp_destroy_connection(conn)                       , "Ending connection" );
}

//--------------------------------------------------------------------------------
void RabbitMQPublisher::publish_message(const std::string &message)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  if(status || !connected) {
    connect();
  }

  log << "Publishing message: " << message << kisscpp::manip::endl;

  amqp_bytes_t message_bytes;

  message_bytes.len   = message.size();
  message_bytes.bytes = const_cast<char*>(message.c_str());

  log << "Publishing message bytes --->: " << (char*)message_bytes.bytes << kisscpp::manip::endl;

  throw_on_error(amqp_basic_publish(conn,
                                    1,
                                    amqp_cstring_bytes("amq.direct"),
                                    amqp_cstring_bytes(queue_name.c_str()),
                                    1,
                                    0,
                                    NULL,
                                    message_bytes),
                 "Publishing");
}

//--------------------------------------------------------------------------------
void RabbitMQPublisher::throw_on_amqp_error(amqp_rpc_reply_t x, const std::string &context)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  log << context << kisscpp::manip::endl;

  switch(x.reply_type) {
    case AMQP_RESPONSE_NORMAL           : log << "All ok after : " << context << kisscpp::manip::endl;
                                          return;
                                          break;
    case AMQP_RESPONSE_NONE             : throw AMQPResponseNone            (context);
                                          break;
    case AMQP_RESPONSE_LIBRARY_EXCEPTION: throw AMQPResponseLibraryException(context,x);
                                          break;
    case AMQP_RESPONSE_SERVER_EXCEPTION :
      std::stringstream tmsg;
      switch(x.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
                                             amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
                                             tmsg << " server connection error " << m->reply_code << " message: " << (char *)m->reply_text.bytes;
                                             break;
                                           }
        case AMQP_CHANNEL_CLOSE_METHOD   : {
                                             amqp_channel_close_t    *m = (amqp_channel_close_t    *) x.reply.decoded;
                                             tmsg << " server channel error "    << m->reply_code << " message: " << (char *)m->reply_text.bytes;
                                             break;
                                           }
        default                          : tmsg << " unknown server error, method id: " << x.reply.id;
                                           break;
      }
      throw AMQPResponseServerException(context,tmsg.str());
      break;
  }
}

//--------------------------------------------------------------------------------
void RabbitMQPublisher::throw_on_error(int x, const std::string &context)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  log << context << kisscpp::manip::endl;

  if (x < 0) {
    std::stringstream tmsg;
    tmsg << "[" << context << "] " << amqp_error_string2(x);
    throw AMQPError(tmsg.str());
  } else {
    log << "All ok after : " << context << kisscpp::manip::endl;
  }
}

