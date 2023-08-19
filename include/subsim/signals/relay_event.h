#pragma once

#include <vector>

#include "net.h"
#include "relay_connection_point.h"

namespace SubSim {
namespace signals {

    class Relay;

    class RelayEvent {
      public:
        RelayEvent(Relay *relay, RelayConnectionPoint connectionPoint, Net::State newState) :
            m_relay(relay),
            m_connectionPoint(connectionPoint),
            m_newState(newState) {}

        std::vector<Net *> execute();

      protected:
        Relay *m_relay;
        RelayConnectionPoint m_connectionPoint;
        Net::State m_newState;
    };

} // namespace signals

} // namespace SubSim
