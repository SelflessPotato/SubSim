#pragma once

#include <vector>

#include "net.h"
#include "relay.h"
#include "relay_event.h"

namespace SubSim {
namespace signals {

    class RelayController {
      public:
        RelayController(std::vector<Relay *> relays, std::vector<Net *> nets) : m_relays(relays), m_nets(nets) {}

        void simulate(std::vector<RelayEvent> initialEvents);

      private:
        std::vector<Relay *> m_relays;
        std::vector<Net *> m_nets;
    };

} // namespace signals

} // namespace SubSim
