#pragma once

#include <vector>

#include "relay_event.h"

namespace SubSim {
namespace signals {

    class Net;

    class Relay {
      public:
        enum class State {
            Picked,
            Dropped
        };

        Relay(Net *coilInput, Net *commonInput, Net *frontContacts, Net *backContacts) :
            m_coilInput(coilInput),
            m_commonInput(commonInput),
            m_frontContacts(frontContacts),
            m_backContacts(backContacts) {}

        State getState() const { return m_state; }

        std::vector<RelayEvent> updateAndCreateEvents();

      protected:
        State m_state;
        Net *m_coilInput;
        Net *m_commonInput;
        Net *m_frontContacts;
        Net *m_backContacts;
    };

} // namespace signals

} // namespace SubSim
