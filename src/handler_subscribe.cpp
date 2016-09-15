// File  : handler_subscribe.cpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#include "handler_subscribe.hpp"

//--------------------------------------------------------------------------------
void SubscribeHandler::run(const BoostPtree& request, BoostPtree& response)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  try {
    std::string subscriber_id = subscribers.add(request.get<std::string>("host"), request.get<std::string>("port")); 
    events_registry.subscribe_kcpp(request.get<std::string>("event-name"), subscriber_id);
    response.put("kcm-sts", kisscpp::RQST_SUCCESS);
    kisscpp::StatsKeeper::instance()->increment("subscribe-success");
  } catch (std::exception& e) {
    log << "Exception: " << e.what() << kisscpp::manip::endl;
    response.put("kcm-sts", kisscpp::RQST_UNKNOWN);
    response.put("kcm-erm", e.what());
    kisscpp::StatsKeeper::instance()->increment("subscribe-fail");
  }
}
