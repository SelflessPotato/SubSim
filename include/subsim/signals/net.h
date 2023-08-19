#pragma once

#include <vector>

namespace SubSim {
namespace signals {

    class Net {
      public:
        enum class State {
            High,
            Low,
            Undriven
        };

        State getState() const { return m_state; }

        void setState(State newState) { m_state = newState; }

      protected:
        State m_state = State::Undriven;
    };

} // namespace signals

} // namespace SubSim
