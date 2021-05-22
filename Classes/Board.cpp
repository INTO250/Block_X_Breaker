#include "Board.h"

USING_NS_CC;

Board* Board::createBoard(const char* filename)
{
	Board* rua = new Board();
	if (rua && rua->initWithFile(filename))
	{
		rua->autorelease();
		return rua;
	}
	CC_SAFE_DELETE(rua);
	return nullptr;
}