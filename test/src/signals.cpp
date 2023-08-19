#include <doctest/doctest.h>
#include <subsim/signals/simulation_manager.h>

#include <queue>

using namespace SubSim::signals;

TEST_CASE("Simulation Manager - Single Relay") {
    SimulationManager sim;

    auto relay = sim.addRelay();

    // First just test all the input combinations
    // Coil low, common low => front undriven, back low
    std::queue<RelayEvent> test1_events;
    test1_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::Low));
    test1_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::Low));

    sim.simulate(test1_events);
    CHECK(relay->getFrontState() == Net::State::Undriven);
    CHECK(relay->getBackState() == Net::State::Low);

    // Coil low, common high => front undriven, back high
    std::queue<RelayEvent> test2_events;
    test2_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::High));

    sim.simulate(test2_events);
    CHECK(relay->getFrontState() == Net::State::Undriven);
    CHECK(relay->getBackState() == Net::State::High);

    // Coil high, common low => front low, back low
    std::queue<RelayEvent> test3_events;
    test3_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::High));
    test3_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::Low));

    sim.simulate(test3_events);
    CHECK(relay->getFrontState() == Net::State::Low);
    CHECK(relay->getBackState() == Net::State::Undriven);

    // Coil high, common high => front high, back low
    std::queue<RelayEvent> test4_events;
    test4_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::High));

    sim.simulate(test4_events);
    CHECK(relay->getFrontState() == Net::State::High);
    CHECK(relay->getBackState() == Net::State::Undriven);

    // All input combinations have been tests
    // Do an additional test just to verify that a high signal "moves" when flipping the coil
    // Coil low, common high => front low, back high
    std::queue<RelayEvent> test5_events;
    test5_events.push(relay->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::Low));

    sim.simulate(test5_events);
    CHECK(relay->getFrontState() == Net::State::Undriven);
    CHECK(relay->getBackState() == Net::State::High);
}

TEST_CASE("Simulation Manager - Net with Multiple Drivers") {
    // This test will verify that the controller properly handles nets with multiple relays driving them
    // Topology is two relays with the front contacts tied together, separate common and coil inputs
    // Going from:
    //  - Relay 1: coil low,  common high (front undriven, back high)
    //  - Relay 2: coil high, common high (front high, back undriven)
    // To:
    //  - Relay 1: coil high, common high (front high, back undriven)
    //  - Relay 2: coil low,  common high (front undriven, back high)
    // Should result in the output starting high and remaining high

    SimulationManager sim;

    auto relay1 = sim.addRelay();
    auto relay2 = sim.addRelay();
    sim.connect(0, RelayConnectionPoint::FrontContact, 1, RelayConnectionPoint::FrontContact);
    sim.connect(0, RelayConnectionPoint::BackContact, 1, RelayConnectionPoint::BackContact);

    auto frontContacts = relay1->getNetByConnectionPoint(RelayConnectionPoint::FrontContact);
    auto backContacts = relay1->getNetByConnectionPoint(RelayConnectionPoint::BackContact);
    CHECK(frontContacts == relay2->getNetByConnectionPoint(RelayConnectionPoint::FrontContact));
    CHECK(backContacts == relay2->getNetByConnectionPoint(RelayConnectionPoint::BackContact));

    // Setup the initial state with all inputs low => all outputs low
    std::queue<RelayEvent> test1_events;
    test1_events.push(relay1->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::Low));
    test1_events.push(relay1->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::Low));
    test1_events.push(relay2->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::Low));
    test1_events.push(relay2->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::Low));

    sim.simulate(test1_events);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::Low);

    // Set up the first scenario from description above
    std::queue<RelayEvent> test2_events;
    test2_events.push(relay1->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::High));
    test2_events.push(relay2->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::High));
    test2_events.push(relay2->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::High));

    sim.simulate(test2_events);
    CHECK(frontContacts->getState() == Net::State::High);
    CHECK(backContacts->getState() == Net::State::High);
    CHECK(relay1->getDrivenValueByNet(frontContacts) == Net::State::Undriven);
    CHECK(relay1->getDrivenValueByNet(backContacts) == Net::State::High);
    CHECK(relay2->getDrivenValueByNet(frontContacts) == Net::State::High);
    CHECK(relay2->getDrivenValueByNet(backContacts) == Net::State::Undriven);

    // Switch the coil inputs, which shouldn't change the outputs at all
    std::queue<RelayEvent> test3_events;
    test3_events.push(relay1->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::High));
    test3_events.push(relay2->setExternallyDrivenInput(RelayConnectionPoint::CoilInput, Net::State::Low));

    sim.simulate(test3_events);
    CHECK(frontContacts->getState() == Net::State::High);
    CHECK(backContacts->getState() == Net::State::High);
    CHECK(relay1->getDrivenValueByNet(frontContacts) == Net::State::High);
    CHECK(relay1->getDrivenValueByNet(backContacts) == Net::State::Undriven);
    CHECK(relay2->getDrivenValueByNet(frontContacts) == Net::State::Undriven);
    CHECK(relay2->getDrivenValueByNet(backContacts) == Net::State::High);

    // Change only one of the relays. Then verify that the back contacts
    // remain high, since relay 1 will try to set it low, but relay 2
    // is still driving it high
    std::queue<RelayEvent> test4_events;
    test4_events.push(relay1->setExternallyDrivenInput(RelayConnectionPoint::CommonInput, Net::State::Low));
    ;

    sim.simulate(test4_events);
    CHECK(relay1->getState() == Relay::State::Picked);
    CHECK(relay2->getState() == Relay::State::Dropped);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::High);
    CHECK(relay1->getDrivenValueByNet(frontContacts) == Net::State::Low);
    CHECK(relay1->getDrivenValueByNet(backContacts) == Net::State::Undriven);
    CHECK(relay2->getDrivenValueByNet(frontContacts) == Net::State::Undriven);
    CHECK(relay2->getDrivenValueByNet(backContacts) == Net::State::High);
}
