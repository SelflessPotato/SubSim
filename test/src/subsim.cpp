#include <doctest/doctest.h>
#include <subsim/subsim.h>

#include <string>

TEST_CASE("SubSim") { CHECK(SubSim::getName() == "SubSim"); }
