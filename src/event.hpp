#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include <sstream>
#include <string>
#include <ctime>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

using namespace boost::archive::iterators;

typedef transform_width< binary_from_base64<remove_whitespace<std::string::const_iterator> > , 8, 6 > BinaryType;
typedef base64_from_binary<transform_width<std::string::const_iterator,6,8> >                         Base64Type;

#include <kisscpp/threadsafe_persisted_queue.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/boost_ptree.hpp>

#include "subscriber.hpp"
#include "string_set.hpp"

//--------------------------------------------------------------------------------
class Event
{
  public:
     Event()                                                      : timestamp(time(NULL)), name("")   , payload("")       {}
     Event(const std::string &_name, const std::string &_payload) : timestamp(time(NULL)), name(_name), payload(_payload)
     {
       kisscpp::LogStream log(__PRETTY_FUNCTION__);
       log << "New event [" << name << "] with payload [" << payload << "]" << kisscpp::manip::endl;
     }
    ~Event() {}

    time_t        getTimestamp          () const { return timestamp;           }
    std::string   getName               () const { return name;                }
    std::string   getPayload            () const { return payload;             }
    std::string   getPayloadBase64Decode() const { std::string retval(BinaryType(payload.begin()), BinaryType(payload.end())); return retval; }
    StringSet     getNotifiedSubscribers() const { return notifiedSubscribers; }

    void          setName               (const std::string   &val) { name      = val; }
    void          setPayload            (const std::string   &val) { payload   = val; }
    void          addNotifiedSubscriber (const std::string   &val) { notifiedSubscribers.insert(val); }

    StringSet     getSubscribers2Notify (const StringSet &subscribers) { return diffStringSet(subscribers, notifiedSubscribers); }

    std::string   toString              ();
    void          fromString            (const std::string   &istr);

  private:
    time_t      timestamp;
    std::string name;
    std::string payload;

    StringSet   notifiedSubscribers;
};

//--------------------------------------------------------------------------------
class EventBase64Bicoder : public kisscpp::Base64BiCoder<Event>
{
  public:
     EventBase64Bicoder() {};
    ~EventBase64Bicoder() {};

    //--------------------------------------------------------------------------------
    boost::shared_ptr<std::string> encode(const boost::shared_ptr<Event> obj2encode)
    {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);
      std::string        tstr = obj2encode->toString();

      return encodeToBase64String(tstr);
    }

    //--------------------------------------------------------------------------------
    boost::shared_ptr<Event> decode(const std::string& str2decode)
    {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);

      log << "String 2 decode: " << str2decode << kisscpp::manip::endl;

      boost::shared_ptr<Event> retval;
      retval.reset(new Event());
      retval->fromString((*decodeFromBase64(str2decode)));

      return retval;
    }

  protected:
  private:
};

typedef kisscpp::ThreadsafePersistedQueue <Event, EventBase64Bicoder> SafeEventQ;
typedef boost::shared_ptr                 <SafeEventQ>                SharedSafeEventQ;
typedef boost::scoped_ptr                 <SafeEventQ>                ScopedSafeEventQ;
typedef boost::shared_ptr                 <Event>                     SharedEvent;

#endif // _EVENT_HPP_

