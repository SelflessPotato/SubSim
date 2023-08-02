#include "subsim/signals/relay.h"

std::vector<SubSim::signals::RelayEvent> SubSim::signals::Relay::updateAndCreateEvents() {
    Net::State coilState = m_coilInput->getState();
    Net::State commonState = m_commonInput->getState();

    if (coilState == Net::State::High) {
        m_state = State::Picked;
    } else if (coilState == Net::State::Low) {
        m_state = State::Dropped;
    }

    // Determine the proper state of the front and back contacts,
    // then create new events if anything has changed
    std::vector<RelayEvent> newEvents;
    Net::State newFront = (m_state == State::Picked) ? commonState : Net::State::Low;
    Net::State newBack = (m_state == State::Picked) ? Net::State::Low : commonState;

    if (newFront != m_frontContacts->getState()) {
        RelayEvent event(m_frontContacts, newFront);
        newEvents.push_back(event);
    }

    if (newBack != m_backContacts->getState()) {
        RelayEvent event(m_backContacts, newBack);
        newEvents.push_back(event);
    }

    return newEvents;
}
