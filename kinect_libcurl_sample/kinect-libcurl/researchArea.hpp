#include <string>
#pragma once

class researchArea
{
private:
	std::string name;
	int minRow;
	int maxRow;
	int minCol;
	int maxCol;
	int count;
	int status;

public:
	researchArea(void);
	~researchArea(void);

	researchArea(int minRow, int maxRow, int minCol, int maxCol);

	void setMinRow(int row) { minRow = row; };
	int getMinRow() { return minRow; };

	void setMaxRow(int row) { maxRow = row; };
	int getMaxRow() { return maxRow; };

	void setMinCol(int col) { minCol = col; };
	int getMinCol() { return minCol; };

	void setMaxCol(int col) { maxCol = col; };
	int getMaxCol() { return maxCol; };

	void setCount(int cou) { count = cou; };
	int getCount() { return count; };

	void setStatus(int stat) { status = stat; };
	int getStatus() { return status; };

	void setName(std::string na) { name = na; };
	std::string getName() { return name; };

};

