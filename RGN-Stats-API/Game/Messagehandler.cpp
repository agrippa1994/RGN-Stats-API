#include "Messagehandler.h"
#include "Game.h"

#define READ(X, Y) BITSTREAM_READ(bsIn, X, Y);
#define WRITE(X) bsOut << X;


void GetStats(CBitStream& bsIn, CBitStream& bsOut)
{
	bsOut << getStats();
}