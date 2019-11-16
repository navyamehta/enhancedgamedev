#include "cell.h"
#include <iostream>

Cell::Cell(int r, int c) {
	this->row = r;
	this->col = c;
	this->isoccup = false;
	this->disp = ' ';
}

Cell* Cell::getLeft() const { return this->left; }

Cell* Cell::getRight() const { return this->right; }

Cell* Cell::getUp() const { return this->up; }

Cell* Cell::getDown() const { return this->down; }

void Cell::setDir(Cell* l, Cell* r, Cell* u, Cell* d) {
	this->left = l;
	this->right = r;
	this->up = u;
	this->down = d;
}

char Cell::getdisp() const { return this->disp; }

void Cell::setdisp(char c) { this->disp = c; }

bool Cell::getbl() const { return this->isoccup; }

void Cell::setbl(bool b) { this->isoccup = b; }

