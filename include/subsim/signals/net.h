#pragma once

#include <vector>

namespace SubSim {
namespace signals {

    class Relay;

    class Net {
      public:
        enum class State {
            High,
            Low
        };

        Net() : m_state(State::Low) {}

        Net(State initialState) : m_state(initialState) {}

        State getState() const { return m_state; }

        void setState(State newState) { m_state = newState; }

        std::vector<Relay *> getOutputs() const { return m_outputs; }

        void setOutputs(std::vector<Relay *> outputs) { m_outputs = outputs; }

      protected:
        State m_state;
        std::vector<Relay *> m_outputs;
    };

} // namespace signals

} // namespace SubSim
