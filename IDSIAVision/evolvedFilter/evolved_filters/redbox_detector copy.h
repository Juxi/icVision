// Author: Juxi Leitner <juxi.leitner@gmail.com>
#include <icVision/icFilterModule.h>

#typedef icImage GpImage

class RedDetector : public icFilterModule {
protected:
	virtual void setUsedInputs() { ;	
public:	
	RedDetector() { setName("RedDetector"); }
		
	icImage* runFilter() {
		icImage* in = InputImages[1];		// red
		icImage* out = in->threshold(64);	// cvThreshold
		delete(in);
		return out;
	}
	
	void RedBoxDetector::setUsedInputs() {
		UsedInputs.Add(0);	
		UsedInputs.Add(1);
		UsedInputs.Add(2);	
		UsedInputs.Add(3);		
	}
	
};