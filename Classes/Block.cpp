#include "Block.h"

USING_NS_CC;

Block* Block::createBlock(const char* filename)
{
	Block* rua = new Block();
	if (rua && rua->initWithFile(filename))
	{
		rua->autorelease();
		return rua;
	}
	CC_SAFE_DELETE(rua);
	return nullptr;
}