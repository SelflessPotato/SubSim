#include "subsim/signals/relay.h"

#include <stdexcept>

std::vector<SubSim::signals::Net *>
SubSim::signals::Relay::updateState(RelayConnectionPoint connectionPoint, Net::State newState) {
    if (connectionPoint == RelayConnectionPoint::CoilInput) {
        m_state = (newState == Net::State::High) ? State::Picked : State::Dropped;
    } else if (connectionPoint == RelayConnectionPoint::CommonInput) {
        if (m_externallyDrivenCoilInput != Net::State::Undriven) {
            m_state = (m_externallyDrivenCoilInput == Net::State::High) ? State::Picked : State::Dropped;
        }
    }

    std::vector<Net *> updatedNets;
    if (m_frontContacts != nullptr) {
        updatedNets.push_back(m_frontContacts);
    }
    if (m_backContacts != nullptr) {
        updatedNets.push_back(m_backContacts);
    }

    return updatedNets;
}

SubSim::signals::Net *SubSim::signals::Relay::getNetByConnectionPoint(RelayConnectionPoint point) const {
    switch (point) {
        case RelayConnectionPoint::CoilInput:
            return m_coilInput;
        case RelayConnectionPoint::CommonInput:
            return m_commonInput;
        case RelayConnectionPoint::FrontContact:
            return m_frontContacts;
        case RelayConnectionPoint::BackContact:
            return m_backContacts;
        default:
            throw std::runtime_error("Invalid connection point requested");
    }
}

void SubSim::signals::Relay::setNetByConnectionPoint(RelayConnectionPoint point, Net *net) {
    switch (point) {
        case RelayConnectionPoint::CoilInput:
            m_coilInput = net;
            break;
        case RelayConnectionPoint::CommonInput:
            m_commonInput = net;
            break;
        case RelayConnectionPoint::FrontContact:
            m_frontContacts = net;
            break;
        case RelayConnectionPoint::BackContact:
            m_backContacts = net;
            break;
    }
}

SubSim::signals::Net::State SubSim::signals::Relay::getDrivenValueByNet(Net *net) const {
    if (m_frontContacts == net) {
        return getFrontState();
    } else if (m_backContacts == net) {
        return getBackState();
    } else {
        return Net::State::Undriven;
    }
}

SubSim::signals::RelayConnectionPoint SubSim::signals::Relay::getConnectionPointByNet(Net *net) const {
    if (m_frontContacts == net) {
        return RelayConnectionPoint::FrontContact;
    } else if (m_backContacts == net) {
        return RelayConnectionPoint::BackContact;
    } else if (m_commonInput == net) {
        return RelayConnectionPoint::CommonInput;
    } else if (m_coilInput == net) {
        return RelayConnectionPoint::CoilInput;
    } else {
        throw std::runtime_error("Net is not connected to given relay");
    }
}

SubSim::signals::RelayEvent
SubSim::signals::Relay::setExternallyDrivenInput(RelayConnectionPoint point, Net::State state) {
    if (point == RelayConnectionPoint::CoilInput) {
        m_externallyDrivenCoilInput = state;
    } else if (point == RelayConnectionPoint::CommonInput) {
        m_externallyDrivenCommonInput = state;
    } else {
        throw std::runtime_error("Attempted to externally drive an invalid connection");
    }
    return RelayEvent(this, point, state);
}

SubSim::signals::Net::State SubSim::signals::Relay::getFrontState() const {
    if (m_state == State::Picked) {
        if (m_externallyDrivenCommonInput != Net::State::Undriven) {
            return m_externallyDrivenCommonInput;
        }
        if (m_commonInput != nullptr) {
            return m_commonInput->getState();
        }
    }
    return Net::State::Undriven;
}

SubSim::signals::Net::State SubSim::signals::Relay::getBackState() const {
    if (m_state == State::Dropped) {
        if (m_externallyDrivenCommonInput != Net::State::Undriven) {
            return m_externallyDrivenCommonInput;
        }
        if (m_commonInput != nullptr) {
            return m_commonInput->getState();
        }
    }
    return Net::State::Undriven;
}
