#include "HystLag_Cpp_Test.h"

int main() {
    testSimpleUP();
	testSimpleDown();
	testInstantUpNoLag();
	testInstantDownNoLag();
	testOnLagUp();
	testOffLagDown();
	testBetweenNeverActivates();
	testRapidToggling();
	testFullCoverageSequence();
    return 0;
}