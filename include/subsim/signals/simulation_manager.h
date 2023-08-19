#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "relay.h"

namespace SubSim {
namespace signals {

    class SimulationManager {
      public:
        SimulationManager() = default;
        SimulationManager(int numRelays);

        Relay *addRelay();

        void connect(Net *net, unsigned relayIndex, RelayConnectionPoint connectionPoint);
        void connect(
            unsigned relayIndex1,
            RelayConnectionPoint connectionPoint1,
            unsigned relayIndex2,
            RelayConnectionPoint connectionPoint2
        );

        void simulateStep();
        void simulate(std::queue<RelayEvent> initialEvents);

      private:
        std::vector<Relay *> m_relays;
        std::unordered_map<Net *, std::vector<Relay *>> m_nets;
        std::queue<RelayEvent> m_eventQueue;

        Relay *getRelay(unsigned relayIndex);
        Net::State resolveNet(Net *net);
    };

} // namespace signals
} // namespace SubSim
