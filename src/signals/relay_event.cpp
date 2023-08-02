#include "subsim/signals/relay_event.h"

std::vector<SubSim::signals::Relay *> SubSim::signals::RelayEvent::simulate() {
    m_net->setState(m_newState);
    return m_net->getOutputs();
}
 