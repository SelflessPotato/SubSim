#include "subsim/signals/simulation_manager.h"

#include <algorithm>
#include <stdexcept>

SubSim::signals::SimulationManager::SimulationManager(int numRelays) : m_relays(numRelays, nullptr) {
    for (int i = 0; i < numRelays; i++) {
        m_relays.push_back(new Relay());
    }
}

SubSim::signals::Relay *SubSim::signals::SimulationManager::addRelay() {
    auto newRelay = new Relay();
    m_relays.push_back(newRelay);
    return newRelay;
}

void SubSim::signals::SimulationManager::connect(Net *net, unsigned relayIndex, RelayConnectionPoint connectionPoint) {
    auto relay = getRelay(relayIndex);

    // Disconnect from the originally connected Net first, if there was an existing connection
    // TODO: Should this instead combine the nets into one?
    auto originallyConnectedNet = getRelay(relayIndex)->getNetByConnectionPoint(connectionPoint);
    if (originallyConnectedNet != nullptr) {
        auto &originallyConnectedRelays = m_nets[originallyConnectedNet];
        auto position = std::find(originallyConnectedRelays.begin(), originallyConnectedRelays.end(), relay);
        if (position != originallyConnectedRelays.end()) {
            originallyConnectedRelays.erase(position);
        }
    }

    // Create the connection in the Relay, then note the connection in the manager
    relay->setNetByConnectionPoint(connectionPoint, net);
    m_nets[net].push_back(relay);
}

void SubSim::signals::SimulationManager::connect(
    unsigned relayIndex1,
    RelayConnectionPoint connectionPoint1,
    unsigned relayIndex2,
    RelayConnectionPoint connectionPoint2
) {
    // Add connections to existing Nets, if they exist, rather than creating additional Nets
    Net *net = getRelay(relayIndex1)->getNetByConnectionPoint(connectionPoint1);

    if (net != nullptr) {
        // TODO: This will overwrite the original Net connected to relay 2, if it exists
        connect(net, relayIndex2, connectionPoint2);
    } else {
        net = getRelay(relayIndex2)->getNetByConnectionPoint(connectionPoint2);
        if (net != nullptr) {
            // TODO: This will overwrite the original Net connected to relay 1, if it exists
            connect(net, relayIndex1, connectionPoint1);
        } else {
            net = new Net();
            connect(net, relayIndex1, connectionPoint1);
            connect(net, relayIndex2, connectionPoint2);
        }
    }
}

void SubSim::signals::SimulationManager::simulateStep() {
    // Simulate the Relays to get updated driving values
    std::unordered_set<Net *> updatedNets;
    while (!m_eventQueue.empty()) {
        auto event = m_eventQueue.front();
        m_eventQueue.pop();
        auto netsFromEvent = event.execute();
        for (auto net : netsFromEvent) {
            updatedNets.emplace(net);
        }
    }

    // Resolve each of the (potentially) updated nets, and queue additional events if there were actual updates
    for (auto net : updatedNets) {
        auto initialState = net->getState();
        auto newState = resolveNet(net);
        if (initialState != newState) {
            net->setState(newState);
            for (auto relay : m_nets[net]) {
                auto point = relay->getConnectionPointByNet(net);
                if (Relay::isInputConnection(point)) {
                    RelayEvent newEvent(relay, point, newState);
                    m_eventQueue.push(newEvent);
                }
            }
        }
    }
}

void SubSim::signals::SimulationManager::simulate(std::queue<RelayEvent> initalEvents) {
    m_eventQueue = initalEvents;
    while (!m_eventQueue.empty()) {
        simulateStep();
    }
}

SubSim::signals::Relay *SubSim::signals::SimulationManager::getRelay(unsigned relayIndex) {
    if (relayIndex > m_relays.size()) {
        throw std::runtime_error("Requested Relay out of range");
    }

    return m_relays[relayIndex];
}

SubSim::signals::Net::State SubSim::signals::SimulationManager::resolveNet(Net *net) {
    for (const auto relay : m_nets[net]) {
        if (relay->getDrivenValueByNet(net) == Net::State::High) {
            return Net::State::High;
        }
    }
    return Net::State::Low;
}
