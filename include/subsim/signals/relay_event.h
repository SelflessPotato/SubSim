#pragma once

#include <vector>

#include "net.h"

namespace SubSim {
namespace signals {

    class Relay;

    class RelayEvent {
      public:
        RelayEvent(Net *net, Net::State newState) : m_net(net), m_newState(newState) {}

        std::vector<Relay *> simulate();

      protected:
        Net *m_net;
        Net::State m_newState;
    };

} // namespace signals

} // namespace SubSim

/*
 * Inputs change => Create initial events (Net changes)
 * Put events in event queue
 * For each event:
 *   - Change Net's state
 *   - Add each relay connected to the Net to a queue for simulation
 * For each relay:
 *   - Update state based on new value of Nets
 *   - If any output nets change, create an Event for it
 * Repeat event/relay queues until empty
 *
 * Psuedocode
 *
 * for input in inputs:
 *     if input != input_previous:
 *         event_queue.push_back(new Event(net, value))
 * while !event_queue.empty:
 *     for event in event_queue:
 *         event.net.state = event.newState
 *         for output in event.net.outputs:
 *             relay_queue.push_back(output)
 *     event_queue.clear()
 *     for relay in relay_queue:
 *         new_events = relay.createOutputEvents()
 *         for event in new_events:
 *             event_queue.push_back(event)
 *     relay_queue.clear()
 *
 */
