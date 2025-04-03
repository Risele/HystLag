#include "HystLag_Cpp_Test.h"

int main() {
    testSimpleUP();
	testSimpleDown();
	testInstantUpNoLag();
	testInstantDownNoLag();
	testHighLagUp();
	testLowLagDown();
	testBetweenNeverActivates();
	testRapidToggling();
	testFullCoverageSequence();
    return 0;
}