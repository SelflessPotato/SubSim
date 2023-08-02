#include "subsim/signals/relay_controller.h"

void SubSim::signals::RelayController::simulate(std::vector<RelayEvent> eventQueue) {
    std::vector<Relay *> relayQueue;

    while (!eventQueue.empty()) {
        for (auto event : eventQueue) {
            auto newRelays = event.simulate();
            relayQueue.insert(relayQueue.end(), newRelays.begin(), newRelays.end());
        }
        eventQueue.clear();

        for (auto relay : relayQueue) {
            auto newEvents = relay->updateAndCreateEvents();
            eventQueue.insert(eventQueue.end(), newEvents.begin(), newEvents.end());
        }
        relayQueue.clear();
    }
}
