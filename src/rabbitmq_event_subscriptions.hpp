#ifndef _RABITMQ_EVENT_SUBSCRIPTIONS_HPP_
#define _RABITMQ_EVENT_SUBSCRIPTIONS_HPP_

#include <string>

class RabbitMQEventSubscriptions
{
  public:
     RabbitMQEventSubscriptions(const std::string &file);
    ~RabbitMQEventSubscriptions();

  protected:
    void loadSubscriptions(const std::string &file);

  private:
     RabbitMQEventSubscriptions() {};
};

#endif // _RABITMQ_EVENT_SUBSCRIPTIONS_HPP_
