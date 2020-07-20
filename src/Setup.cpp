#include "Setup.hpp"
#include "DMA.hpp"
#include "Filter.hpp"
#include "DMASetup.hpp"
#include "FilterSetup.hpp"
void Setup::SetupQueryAcceleration(int* volatile& memoryPointer, std::vector<int>& dbData, int recordSize, int recordCount)
{
	DMA dmaEngine(memoryPointer);
	DMASetup dmaSetup;
	dmaSetup.SetupDMAModule(recordCount, dbData, recordSize, dmaEngine);

	// Setup the filter module
	Filter filterModule(memoryPointer, 1);
	FilterSetup filterSetup;
	filterSetup.SetupFilterModule(filterModule);

	bool streamActive[16] = { false };
	streamActive[0] = true;
	dmaEngine.startInputController(streamActive);
	dmaEngine.startOutputController(streamActive);

	// Print out the contents of memory for debugging
	//std::cout << std::endl << "Memory contents:" << std::endl;
	//for (int i = 0; i < 1048576; i++) {
	//	if (memoryPointer[i] != -1) {
	//		std::cout << "Address:" << i << std::endl;
	//		std::cout << memoryPointer[i] << std::endl;
	//	}
	//}

	// check isInputControllerFinished and isOutputControllerFinished
}