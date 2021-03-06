#include "grid.h"
#include "piece.h"
#include "pieceI.h"
#include "pieceO.h"
#include "pieceT.h"
#include "pieceZ.h"
#include "pieceS.h"
#include "pieceJ.h"
#include "pieceL.h"
#include "pieceStar.h"
#include "level0.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"
#include "subject.h"
#include "state.h"
#include "graphicdisplay.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

void setmap(vector<vector<unique_ptr<Cell>>> &mp, Position p, vector<Observer*> disp) {
	for (int i=0; i < 18; i++) {
                vector<unique_ptr<Cell>> temp;
                for (int j=0; j < 11; j++) {
                        unique_ptr<Cell> tp {new Cell{i, j, p}};
			for (int k=0; k<disp.size(); k++) {
				tp->attach(disp[k]);
			}
                        temp.push_back(move(tp));
                }
                mp.push_back(move(temp));
        }
	for (int i=0; i < 18; i++) {
                for (int j=0; j < 11; j++) {
                        Cell* l = j!=0?mp[i][j-1].get():nullptr;
                        Cell* r = j!=10?mp[i][j+1].get():nullptr;
                        Cell* u = i!=0?mp[i-1][j].get():nullptr;
                        Cell* d = i!=17?mp[i+1][j].get():nullptr;
                        (mp[i][j])->setDir(l, r, u, d);
                }
        }
}

void setnext(vector<vector<unique_ptr<Cell>>> &mp, Position p, vector<Observer*> disp) {
        for (int i=0; i < 4; i++) {
                vector<unique_ptr<Cell>> temp;
                for (int j=0; j < 11; j++) {
                        unique_ptr<Cell> tp {new Cell{i, j, p}};
			for (int k=0; k<disp.size(); k++) {
				tp->attach(disp[k]);
			}
                        temp.push_back(move(tp));
                }
                mp.push_back(move(temp));
        }
}

Level* Grid::makeLevel(int n, int p) {
	if (n==0) {
		string s = p==1?this->scf1:this->scf2;
		return new LevelZero(s);
	}
	else if (n==1) {
		return new LevelOne(this->seed);
	}
	else if (n==2) {
		return new LevelTwo(this->seed);
	}
	else if (n==3) {
		return new LevelThree(this->seed);
	}
	else if (n==4) {
		return new LevelFour(this->seed);
	}
}

Grid::Grid(int l1, int l2, bool grph, string scf1, string scf2, int seed) {
	ifstream temp{"highscore.txt"};
	string tp;
	temp >> tp;
	istringstream sock1(tp);
	sock1 >> this->highscore1;
	temp >> tp;
	istringstream sock2(tp);
	sock2 >> this->highscore2;
	this->seed = seed;
	vector<Observer*> disp;
	(this->td).reset(new TextDisplay);
	disp.push_back(td.get());
	this->gd = nullptr;
	if (grph) {
		(this->gd).reset(new GraphicDisplay(l1, this->highscore1, l2, this->highscore2));
		disp.push_back(gd.get());
	}
	setmap(this->map1, Position::MainMap1, disp);
	setmap(this->map2, Position::MainMap2, disp);
	setnext(this->nxtmap1, Position::NextMap1, disp);
	setnext(this->nxtmap2, Position::NextMap2, disp);
	this->lev1 = l1;
	this->lev2 = l2;
	this->scf1 = scf1;
	this->scf2 = scf2;
	(this->lp1).reset(makeLevel(lev1, 1));
	(this->lp2).reset(makeLevel(lev2, 2));
	(this->nxtpc1).reset(lp1->nextPiece(nxtmap1));
	(this->nxtpc2).reset(lp2->nextPiece(nxtmap2));
	this->curscore1 = 0;
	this->curscore2 = 0;
	this->uns1 = 0;
	this->uns2 = 0;
	this->bl1 = false;
	this->bl2 = false;
	this->hv1 = false;
	this->hv2 = false;
}

void repeatprinter(ostream &out, string s, int rep, int spc) {
	for (int m=0; m < rep; m++) {
                out << s;
        }
        for (int m=0; m < spc; m++) {
                out << " ";
        }
        for (int m=0; m < rep;  m++) {
                out << s;
        }
	out << endl;
}

ostream &operator<<(ostream &out, const Grid &g) {
	out << "\033c" << endl;
	out << "Level: " << g.lev1 << "        Level: " << g.lev2 << endl;
	out << "Score: " << g.curscore1 << "        Score: " << g.curscore2 << endl;
	out << "H Scr: " << g.highscore1 << "        H Scr: " << g.highscore2 << endl;
	repeatprinter(out, "-", 11, 5);
	g.td->printMain(out);
	repeatprinter(out, "-", 11, 5);
	out << "Next:           Next:" << endl;
	g.td->printNext(out);
	return out;
}

Grid::~Grid() {
	ofstream temp;
	temp.open("highscore.txt");
	temp << this->highscore1 << endl;
	temp << this->highscore2 << endl;
	temp.close();
}

void Grid::nextBlock(int p) {
	if (p==1) {
		this->nxtpc1->transferPiece(nxtmap1, map1);
		move1.push_back(move(this->nxtpc1));
		this->nxtpc1.reset(lp1->nextPiece(nxtmap1));
	}
	else {
		this->nxtpc2->transferPiece(nxtmap2, map2);
		move2.push_back(move(this->nxtpc2));
		this->nxtpc2.reset(lp2->nextPiece(nxtmap2));
	}
}

void  Grid::rotateBlock(int p, bool clock) {
	if (p==1) {
		move1[move1.size()-1]->rotate(clock);
	}
	else {
		move2[move2.size()-1]->rotate(clock);
	}
}

void Grid::moveBlock(int p, string dir, int c) {
	if (p==1) {
		move1[move1.size()-1]->movepos(dir, c);
	}
	else {
		move2[move2.size()-1]->movepos(dir, c);
	}
}

int Grid::dropBlock(int p) {
	int rows = 0;
        if (p==1) {
		if (bl1) {
			for (int i=3; i<12; i++) {
				for (int j=3; j<9; j++) {
					(this->map1)[i][j]->removeblind();
				}
			}
			bl1 = false;
		}
                move1[move1.size()-1]->drop();
		++uns1;
		for (int j=17; j>=0;) {
			bool clear = true;
			for (int i=0; i < 11; i++) {
				if (!(map1[j][i]->getbl())) { clear = false; }
			}
			if (clear) {
				curscore1 += (lev1 + 1) * (lev1 + 1);
				for (int i=0; i < move1.size(); i++) {
					move1[i]->rowdel(j);
				}
				uns1 = 0;
				++rows;
				if (curscore1 > highscore1) {
					highscore1 = curscore1;
				}
			}
			else {
				--j;
			}
		}
		if ((lev1==4)&&(uns1>=5)) {
			unique_ptr<Piece> tp{new PieceStar(map1)};
			move1.push_back(move(tp));
			uns1 = 0;
		}
        }
        else {
		if (bl2) {
			for (int i=3; i<12; i++) {
				for (int j=3; j<9; j++) {
					(this->map2)[i][j]->removeblind();
				}
			}
			bl2 = false;
		}
                move2[move2.size()-1]->drop();
		for (int j=17; j>=0;) {
			bool clear = true;
			for (int i=0; i < 11; i++) {
                        	if (!(map2[j][i]->getbl())) { clear = false; }
                	}
			if (clear) {
				curscore2 += (lev2 + 1) * (lev2 + 1);
                		for (int i=0; i < move2.size(); i++) {
                       			move2[i]->rowdel(j);
                		}
				uns2 = 0;
				++rows;
				if (curscore2 > highscore2) {
					highscore2 = curscore2;
				}
			}
			else {
				--j;
			}
		}
		if ((lev2==4)&&(uns2>=5)) {
			unique_ptr<Piece> tp{new PieceStar(map2)};
                        move2.push_back(move(tp));
                        uns2 = 0;
                }
	}
	if ((this->gd!=nullptr)&&(rows>0)) {
		this->gd->updateStats(lev1, curscore1, highscore1, lev2, curscore2, highscore2);
	}
	return rows;
}

void Grid::chngLevel(int p, bool up, int c) {
	if (p==1) {
		bool cont = true;
		while ((cont)&&(c>0)) {
			if (((lev1==4)&&up)||((lev1==0)&&(!up))) {
				cont = false; 
			}
			else {
				lev1 = up?lev1+1:lev1-1;
			}
			--c;
		}
		lp1.reset(makeLevel(lev1, p));
	}
	else {
		bool cont = true;
		while ((cont)&&(c>0)) {
			if (((lev2==4)&&up)||((lev2==0)&&(!up))) {
				cont = false;
			}
			else {
				lev2 = up?lev2+1:lev2-1;
			}
			--c;
		}
		lp2.reset(makeLevel(lev2, p));
	}
	if (this->gd!=nullptr) {
		this->gd->updateStats(lev1, curscore1, highscore1, lev2, curscore2, highscore2);
	}
}

int Grid::getWinner() {
	if (curscore1==curscore2) {
		return 0;
	}
	return curscore1>curscore2?1:2;
}

int Grid::getLevel(int p) {
	return p==1?lev1:lev2;
}

void Grid::bldPlay(int p) {
	if (p==1) {
		bl1 = true;
		for (int i=3; i<12; i++) {
			for (int j=3; j<9; j++) {
				(this->map1)[i][j]->makeblind();
			}
		}
		return;
	}
	bl2 = true;
	for (int i=3; i<12; i++) {
		for (int j=3; j<9; j++) {
			(this->map2)[i][j]->makeblind();
		}
	}
}

void Grid::forceNext(int p, string pc) {
	if (p==1) {
		move1.pop_back();
	}
	else {
		move2.pop_back();
	}
	unique_ptr<Piece> nw;
	if (pc=="I") {
                nw.reset(new PieceI(p==1?map1:map2));
        }
        else if (pc=="O") {
                nw.reset(new PieceO(p==1?map1:map2));
        }
        else if (pc=="T") {
                nw.reset(new PieceT(p==1?map1:map2));
        }
        else if (pc=="S") {
                nw.reset(new PieceS(p==1?map1:map2));
        }
        else if (pc=="Z") {
                nw.reset(new PieceZ(p==1?map1:map2));
        }
        else if (pc=="J") {
                nw.reset(new PieceJ(p==1?map1:map2));
        }
        else if (pc=="L") {
                nw.reset(new PieceL(p==1?map1:map2));
        }
	p==1?move1.push_back(move(nw)):move2.push_back(move(nw));
}

bool Grid::getHeavy(int p) {
	return p==1?hv1:hv2;
}

void Grid::setHeavy(int p, bool h) {
	if (p==1) {
		hv1 = h;
		return;
	}
	hv2 = h;
}

void Grid::chngRandom(int p, string fl) {
	if (p==1) {
		if (lev1<3) { return; }
		if (lev1==3) {
			lp1.reset(new LevelThree(0, fl));
		}
		else {
			lp1.reset(new LevelFour(0, fl));
		}
	}
	else {
		if (lev2<3) { return; }
		if (lev2==3) {
			lp2.reset(new LevelThree(0, fl));
		}
		else {
			lp2.reset(new LevelFour(0, fl));
		}
	}
}

void Grid::restartGame(int l1, int l2) {
	for (int i=move1.size()-1; i>=0; i--) {
		move1.pop_back();
        }
        for (int j=move2.size()-1; j>=0; j--) {
		move2.pop_back();
        }
	(this->nxtpc1).reset(nullptr);
        (this->nxtpc2).reset(nullptr);
	(this->td).reset(new TextDisplay);
	vector<Observer*> disp;
	disp.push_back(td.get());
	if (this->gd != nullptr) {
		gd.reset(new GraphicDisplay(l1, this->highscore1, l2, this->highscore2));
		disp.push_back(gd.get());
	}
	for (int i=0; i<18; i++) {
		for (int j=0; j<11;j++) {
			map1[i][j]->clear();
			map2[i][j]->clear();
			for (int k=0; k<disp.size(); k++) {
				map1[i][j]->attach(disp[k]);
				map2[i][j]->attach(disp[k]);
			}
		}
	}
	for (int i=0; i<4; i++) {
		for (int j=0; j<11; j++) {
			nxtmap1[i][j]->clear();
			nxtmap2[i][j]->clear();
			for (int k=0; k<disp.size(); k++) {
				nxtmap1[i][j]->attach(disp[k]);
				nxtmap2[i][j]->attach(disp[k]);
			}
		}
	}
	this->lev1 = l1;
	this->lev2 = l2;
        (this->lp1).reset(makeLevel(lev1, 1));
        (this->lp2).reset(makeLevel(lev2, 2));
	(this->nxtpc1).reset(lp1->nextPiece(nxtmap1));
        (this->nxtpc2).reset(lp2->nextPiece(nxtmap2));
        this->curscore1 = 0;
        this->curscore2 = 0;
        this->uns1 = 0;
        this->uns2 = 0;
        this->bl1 = false;
        this->bl2 = false;
        this->hv1 = false;
        this->hv2 = false;
}
