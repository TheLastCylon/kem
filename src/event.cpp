#include "event.hpp"

//--------------------------------------------------------------------------------
std::string Event::toString()
{
  BoostPtree        inner;
  BoostPtree        outer;
  BoostPtree        notified_subscribers;
  std::stringstream tmp_string_storage;
  std::string       retval;

  inner.put("timestamp", timestamp);
  inner.put("name"     , name     );
  inner.put("payload"  , payload  );

  for(StringSetIter itr = notifiedSubscribers.begin(); itr != notifiedSubscribers.end(); ++itr) {
    BoostPtree subscriber_id;
    subscriber_id.put("",*itr);
    notified_subscribers.push_back(std::make_pair("",subscriber_id));
  }

  inner.add_child("notified-subscribers", notified_subscribers);

  outer.add_child("event",inner);

  boost::property_tree::write_json(tmp_string_storage,outer);

  retval = tmp_string_storage.str();

  return retval;
}

//--------------------------------------------------------------------------------
void Event::fromString(const std::string &istr)
{
  BoostPtree        pt;
  std::stringstream tmp_string_storage;

  tmp_string_storage << istr;

  std::cout << "STREAM : " << tmp_string_storage.str() << std::endl;

  boost::property_tree::read_json(tmp_string_storage, pt);

  timestamp = pt.get<time_t     >("event.timestamp");
  name      = pt.get<std::string>("event.name"     );
  payload   = pt.get<std::string>("event.payload"  );

  BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, pt.get_child("notified-subscribers")) {
    std::string subscriber_id = v.second.data();
    addNotifiedSubscriber(subscriber_id);
  }
}

//--------------------------------------------------------------------------------
//StringSet Event::getSubscribers2Notify(const StringSet &subscribers)
//{
//  return diffStringSet(subscribers, notifiedSubscribers);
//}

//--------------------------------------------------------------------------------
//StringSet Event::getQueues2Notify(const StringSet &queue_names)
//{
//  return diffStringSet(queue_names, notifiedQueues);
//}
