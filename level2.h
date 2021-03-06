#ifndef LEVEL2_H
#define LEVEL2_H
#include "level.h"
#include "pieceI.h"
#include "pieceZ.h"
#include "pieceS.h"
#include "pieceT.h"
#include "pieceO.h"
#include "pieceJ.h"
#include "pieceL.h"
#include <fstream>

class Piece;
class Cell;

class LevelTwo: public Level {
	int seed;
	public:
		LevelTwo(int s=0);
		Piece* nextPiece(std::vector<std::vector<std::unique_ptr<Cell>>> &t) override;
};

#endif
