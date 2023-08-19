#include "subsim/signals/relay_event.h"

#include "subsim/signals/relay.h"

std::vector<SubSim::signals::Net *> SubSim::signals::RelayEvent::execute() {
    return m_relay->updateState(m_connectionPoint, m_newState);
}
