#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
using namespace std;

#define width 7
#define height 6

class ConnectFour : public olc::PixelGameEngine
{
public:
	int cellW = 30;
	int borderW = 2;
	int turnCol = 0;

	struct Ball
	{
		int cX;
		int cY;
		int radius;
		olc::Pixel col;

		Ball() 
		{
			cX = 0;
			cY = 0;
			radius = 0;
			col = olc::BLANK;
		}

		Ball(int _cX, int _cY, int _radius, int _col)
		{
			cX = _cX;
			cY = _cY;
			radius = _radius;
			if (_col == 0)
				col = olc::RED;
			else if (_col == 1)
				col = olc::YELLOW;
		}

		void Show(olc::PixelGameEngine* pge)
		{
			pge->FillCircle(cX, cY, radius, col);
		}
	};

	struct Cell
	{
		int x;
		int y;
		Ball b;
		bool occupied;

		Cell() {}	
	};

	bool IsGameOver(const vector<vector<Cell>>& grid, int turn)
	{
		// look for this col
		olc::Pixel col = (turn == 0) ? olc::RED : olc::YELLOW; 

		// Check horizontals
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				int count = 0;
				for (int k = 0; k < 4; k++)
				{
					if (grid[j + k][i].b.col == col)
					{
						count++;
					}
				}
				if (count == 4)
				{
					return true;
				}
			}
		}

		// Check verticals
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int count = 0;
				for (int k = 0; k < 4; k++)
				{
					if (grid[i][j+k].b.col == col)
					{
						count++;
					}
				}
				if (count == 4)
				{
					return true;
				}
			}
		}

		// Check main diagonal
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (col == grid[i][j].b.col &&
					col == grid[i + 1][j + 1].b.col &&
					col == grid[i + 2][j + 2].b.col &&
					col == grid[i + 3][j + 3].b.col)
				{
					return true;
				}
			}
		}

		// Check counter diagonal
		for (int i = 0; i < 3; i++)
		{
			for (int j = 3; j < 7; j++)
			{
				if (col == grid[i][j].b.col &&
					col == grid[i + 1][j - 1].b.col &&
					col == grid[i + 2][j - 2].b.col &&
					col == grid[i + 3][j - 3].b.col)
				{
					return true;
				}
			}
		}

		return false;
	}

	void ClearGrid(vector<vector<Cell>>& grid)
	{
		turnCol = 0;
		decision = "";
		gameover = false;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Cell* c = &grid[x][y];
				c->x = x;
				c->y = y;
				c->b = Ball();
				c->occupied = false;
			}
		}
	}

	vector<vector<Cell>> grid;
	bool gameover = false;
	string decision;

public:
	bool OnUserCreate() override
	{
		grid.resize(width, vector<Cell>(height));

		// Create a grid of Cells
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Cell* c = &grid[x][y];
				c->x = x;
				c->y = y;
				c->b = Ball();
				c->occupied = false;
			}
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear Screen
		Clear(olc::VERY_DARK_GREY);

		// Get clicked column
		int selCol = GetMouseX() / cellW;
		
		int row = height - 1;
		if (GetMouse(0).bReleased && !gameover)	// Drop a circle in the selected column
		{
			// Make sure the column is valid
			if (selCol > -1 && selCol < width)
			{
				Cell* top = &grid[selCol][row];
				while (top->occupied && row > 0)
				{
					row--;
					top = &grid[selCol][row];
				}

				if (row == 0 && top->occupied)
				{
					top = nullptr;
				}
				else
				{
					top->occupied = true;

					// Ball settings
					int circleX = selCol * cellW + 0.5 * (cellW - borderW);
					int circleY = row * cellW + 0.5 * (cellW - borderW);
					int radius = cellW / 2 - borderW;
					turnCol++;

					top->b = Ball(circleX, circleY, radius, turnCol % 2);	

					if (IsGameOver(grid, turnCol % 2))
					{									
						if (turnCol % 2 == 0)
						{
							decision = "RED WON";
						}
						else
						{
							decision = "YELLOW WON";
						}
						gameover = true;
					}
					else if (turnCol == 42)
					{
						gameover = true;
						decision = "DRAW";
					}
				}
			}
		}
		
		if (GetKey(olc::Key::R).bReleased)
		{
			ClearGrid(grid);
		}

		if (gameover)
		{
			DrawString({ 10, 250 }, decision, olc::WHITE);
		}
		else
		{
			string colorMove = (turnCol % 2 != 0) ? "RED" : "YELLOW";
			DrawString({ 10, 250 }, colorMove + "'s turn", olc::WHITE);
		}

		// Draw board
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Cell* curr = &grid[x][y];
				DrawRect(curr->x * cellW, curr->y * cellW, cellW - borderW, cellW - borderW, olc::WHITE);
				curr->b.Show(this);
			}
		}

		return true;
	}
};

int main()
{
	ConnectFour game;
	if (game.Construct(512, 480, 2, 2))
	{
		game.Start();
	}

	return 0;
}