#pragma once

#include <vector>

#include "net.h"
#include "relay_event.h"

namespace SubSim {
namespace signals {

    class Relay {
      public:
        enum class State {
            Picked,
            Dropped
        };

        State getState() const { return m_state; }

        std::vector<Net *> updateState(RelayConnectionPoint connectionPoint, Net::State newState);

        Net *getNetByConnectionPoint(RelayConnectionPoint point) const;
        void setNetByConnectionPoint(RelayConnectionPoint point, Net *net);
        Net::State getDrivenValueByNet(Net *net) const;
        RelayConnectionPoint getConnectionPointByNet(Net *net) const;

        // Set driven inputs and create the associated RelayEvents for simulation
        RelayEvent setExternallyDrivenInput(RelayConnectionPoint point, Net::State state);

        static bool isInputConnection(RelayConnectionPoint point) {
            return point == RelayConnectionPoint::CommonInput || point == RelayConnectionPoint::CoilInput;
        }

        static bool isOutputConnection(RelayConnectionPoint point) {
            return point == RelayConnectionPoint::FrontContact || point == RelayConnectionPoint::BackContact;
        }

        Net::State getFrontState() const;
        Net::State getBackState() const;

      protected:
        State m_state;
        Net::State m_externallyDrivenCommonInput = Net::State::Undriven;
        Net::State m_externallyDrivenCoilInput = Net::State::Undriven;
        Net *m_coilInput = nullptr;
        Net *m_commonInput = nullptr;
        Net *m_frontContacts = nullptr;
        Net *m_backContacts = nullptr;
    };

} // namespace signals

} // namespace SubSim
