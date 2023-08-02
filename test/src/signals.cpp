#include <doctest/doctest.h>
#include <subsim/signals/relay_controller.h>

#include <vector>

using namespace SubSim::signals;

TEST_CASE("Signal Controller Single Relay") {
    std::vector<Net *> nets;
    std::vector<Relay *> relays;

    auto coilInput = new Net();
    auto commonInput = new Net();
    auto frontContacts = new Net();
    auto backContacts = new Net();
    auto relay = new Relay(coilInput, commonInput, frontContacts, backContacts);

    nets.push_back(coilInput);
    nets.push_back(commonInput);
    nets.push_back(frontContacts);
    nets.push_back(backContacts);

    relays.push_back(relay);

    coilInput->setOutputs(relays);
    commonInput->setOutputs(relays);

    RelayController controller(relays, nets);

    // First just test all the input combinations
    // Coil low, common low => front low, back low
    std::vector<RelayEvent> test1_events;
    test1_events.push_back(RelayEvent(coilInput, Net::State::Low));
    test1_events.push_back(RelayEvent(commonInput, Net::State::Low));

    controller.simulate(test1_events);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::Low);

    // Coil low, common high => front low, back high
    std::vector<RelayEvent> test2_events;
    test2_events.push_back(RelayEvent(commonInput, Net::State::High));

    controller.simulate(test2_events);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::High);

    // Coil high, common low => front low, back low
    std::vector<RelayEvent> test3_events;
    test3_events.push_back(RelayEvent(coilInput, Net::State::High));
    test3_events.push_back(RelayEvent(commonInput, Net::State::Low));

    controller.simulate(test3_events);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::Low);

    // Coil high, common high => front high, back low
    std::vector<RelayEvent> test4_events;
    test4_events.push_back(RelayEvent(commonInput, Net::State::High));

    controller.simulate(test4_events);
    CHECK(frontContacts->getState() == Net::State::High);
    CHECK(backContacts->getState() == Net::State::Low);

    // All input combinations have been tests
    // Do an additional test just to verify that a high signal "moves" when flipping the coil
    // Coil low, common high => front low, back high
    std::vector<RelayEvent> test5_events;
    test5_events.push_back(RelayEvent(coilInput, Net::State::Low));

    controller.simulate(test5_events);
    CHECK(frontContacts->getState() == Net::State::Low);
    CHECK(backContacts->getState() == Net::State::High);
}
