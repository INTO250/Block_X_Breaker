#include "Arrow.h"

USING_NS_CC;

Arrow* Arrow::createArrow(const char* filename)
{
	Arrow* rua = new Arrow();
	if (rua && rua->initWithFile(filename))
	{
		rua->autorelease();
		return rua;
	}
	CC_SAFE_DELETE(rua);
	return nullptr;
}