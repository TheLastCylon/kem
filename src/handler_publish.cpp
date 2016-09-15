// File  : handler_publish.cpp
// Author: Dirk J. Botha <bothadj@gmail.com>

#include "handler_publish.hpp"

//--------------------------------------------------------------------------------
void PublishHandler::run(const BoostPtree& request, BoostPtree& response)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  SharedEvent t_event;
  std::string event_name;
  std::string event_payload;

  try {
    event_name    = request.get<std::string>("event.name");
    event_payload = request.get<std::string>("event.payload");

    t_event.reset(new Event(event_name, event_payload));

    q_events->push(t_event);

    response.put("kcm-sts", kisscpp::RQST_SUCCESS);
    kisscpp::StatsKeeper::instance()->increment("publish-success");

  } catch (std::exception &e) {
    log << "Exception: " << e.what() << kisscpp::manip::endl;
    response.put("kcm-sts", kisscpp::RQST_UNKNOWN);
    response.put("kcm-erm", e.what());
    kisscpp::StatsKeeper::instance()->increment("publish-fail");
  }
}
