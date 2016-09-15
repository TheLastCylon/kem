AUTOMAKE_OPTIONS    = subdir-objects
AM_CPPFLAGS         = -DBOOST_SPIRIT_THREADSAFE $(DEPS_CFLAGS) $(BOOST_CFLAGS) $(KISSCPP_CFLAGS)
kem_LDADD           = $(DEPS_LIBS)   $(BOOST_LIBS)   $(KISSCPP_LIB) -lrabbitmq
bin_PROGRAMS        = kem
kem_SOURCES         = src/cfg.hpp \
                      src/event.cpp \
                      src/event.hpp \
                      src/events_registry.hpp \
                      src/handler_publish.cpp \
                      src/handler_publish.hpp \
                      src/handler_subscribe.cpp \
                      src/handler_subscribe.hpp \
                      src/kem.cpp \
                      src/kem.hpp \
                      src/rabbitmq_event_subscriptions.cpp \
                      src/rabbitmq_event_subscriptions.hpp \
                      src/rabbitmq_publisher.cpp \
                      src/rabbitmq_publisher.hpp \
                      src/rabbitmq_registry.hpp \
                      src/stat.hpp \
                      src/string_set.cpp \
                      src/string_set.hpp \
                      src/subscriber.hpp \
                      src/main.cpp
dist_noinst_SCRIPTS = autogen.sh
