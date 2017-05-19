#include "NXT_FileIO.c"
#include "PC_FileIO.c"
//Kevin
// Global 2-D Array
int board[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int readBoard[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// return the open space in a line, as an index from 0 to 8
int isLineAt2(int player, int line)
{
		// Becuse ROBOTC is throwing weird compiler errors. RobotC does not allow for arithmetic inside array params.
		int linem1 = line - 1;
		int linem4 = line - 4;
	// player is either 1 (person) or 2 (AI) or 0 (Empty)
	// 1, 2, 3 denote rows
	// 4, 5, 6 denote columns
	// 7, 8 are \ and / respectively
	if (line < 4)
	{

		// No valid play if line is full
		if ((board[linem1][0] != 0) && (board[linem1][1] != 0) && (board[linem1][2] != 0)) return -1;
		// Check rows
		for (int i = 0; i < 2; i++)
		{
			if (board[linem1][i] == player && board[linem1][i + 1] == player)
			{
				return i == 0 ? (line - 1) * 3 + 2 : (line - 1) * 3;
			}
			if (i == 0 && (board[linem1][0] == player && board[linem1][2] == player))
			{
				return (line - 1) * 3 + 1;
			}
		}
	}
	else if (4 <= line && line < 7)
	{
		if (board[0][linem4] != 0 && board[1][linem4] != 0 && board[2][linem4] != 0) return -1;
		// Check columns
		for (int i = 0; i < 2; i++)
		{
			if (board[i][linem4] == player && board[i + 1][linem4] == player)
			{
				return i == 0 ? (linem4) + 6 : line - 4;
			}
			if (i == 0 && (board[0][linem4] == player && board[2][linem4] == player))
			{
				return (linem4) + 3;
			}
		}
	}
	else if (line == 7)
	{
		if (board[0][0] != 0 && board[1][1] != 0 && board[2][2] != 0) return -1;
		// Check first diagonal
		for (int i = 0; i < 2; i++)
		{
			if (board[i][i] == player && board[i + 1][i + 1] == player)
			{
				return i == 0 ? 8 : 0;
			}
			if (i == 0 && (board[0][0] == player && board[2][2] == player))
			{
				return 4;
			}
		}
	}
	else if (line == 8)
	{
		if (board[2][0] != 0 && board[1][1] != 0 && board[0][2]) return -1;
		// Check other diagonal
		for (int i = 0; i < 2; i++)
		{
			if (board[2 - i][i] == player && board[1 - i][i + 1] == player)
			{
				return i == 0 ? 2 : 6;
			}
			if (i == 0 && (board[2][0] == player && board[0][2] == player))
			{
				return 4;
			}
		}
	}

	return -1;
}

int findFork(int player)
{
	// Find the first 2 valid lines
	int validLines[2] = {0, 0};
	int token, prevToken, row, col, index = 0;
	for (int i = 1; i < 9; i++)
	{
		// Returns the space that isn't 0
		token = isLineAt2(0, i);
		//std::cout << "Line: " << i << " Token: " << token;
		row = token / 3; // Truncates anyways
		col = token % 3;
		//std::cout << "  Row: " << row << " Col: " << col << std::endl;
		if ((token >= 0 && board[row][col] == player) && token != prevToken)
		{
			validLines[index] = i;
			prevToken = token;
			index++;
			if (index == 2) break;
		}
	}

	//std::cout << validLines[0] << "   " << validLines[1] << std::endl;

	// Has to have 2 lines
	if (validLines[0] == 0 || validLines[1] == 0) return -1;

	// Find the intersection of the 2 lines
	if (validLines[0] < 4 && validLines[1] < 7)
	{
		// Perpendicular lines
		if (validLines[0] == 1) return validLines[1] - 4;
		if (validLines[0] == 2) return validLines[1] - 1;
		if (validLines[0] == 3) return validLines[1] + 2;
	}

	if ((validLines[0] == 1 || validLines[0] == 4) && validLines[1] == 7) return 0;
	if ((validLines[0] == 2 || validLines[0] == 5) && validLines[1] == 7) return 4;
	if ((validLines[0] == 3 || validLines[0] == 6) && validLines[1] == 7) return 8;

	if ((validLines[0] == 1 || validLines[0] == 6) && validLines[1] == 8) return 2;
	if ((validLines[0] == 2 || validLines[0] == 5) && validLines[1] == 8) return 4;
	if ((validLines[0] == 3 || validLines[0] == 4) && validLines[1] == 8) return 6;

	return -1;
}

int winMove()
{
	// Returns the first move, as an index from 0 to 8
	for (int i = 1; i < 9; i++)
	{
		int space = isLineAt2(2, i);
		if (space >= 0) return space;
	}
	// No win move
	return -1;
}

int blockMove()
{
	// Returns the first move, as an index from 0 to 8
	for (int i = 1; i < 9; i++)
	{
		int space = isLineAt2(1, i);
		if (space >= 0) return space;
	}
	// No block move
	return -1;
}

int computeNextMove()
{
	int winSpot = winMove();
	//std::cout << "win: " << winSpot << std::endl;
	if (winSpot >= 0) return winSpot;
	int blockSpot = blockMove();
	//std::cout  << "block: " << blockSpot << std::endl;
	if (blockSpot >= 0) return blockSpot;
	int forkSpot = findFork(2);
	//std::cout << "fork: " << forkSpot << std::endl;
	if (forkSpot >= 0) return forkSpot;
	int blockForkSpot = findFork(1);
	//std::cout << "blockfork: " << blockForkSpot << std::endl;
	if (blockForkSpot >= 0) return blockForkSpot;
	// Play center
	if (board[1][1] == 0) return 4;
	// Special case
	if ((board[0][0] == 1 && board[2][2] == 1) || (board[2][0] == 1 && board[2][0] == 1))
	{
		if (board[0][1] == 0) return 1;
		if (board[1][0] == 0) return 3;
		if (board[1][2] == 0) return 5;
		if (board[2][1] == 0) return 7;
	}
	// Play empty corners
	for (int i = 0; i <= 2; i += 2)
	{
		for (int j = 0; j <= 2; i += 2)
		{
			if (board[i][j] == 0) return i * 3 + j;
 		}
	}
	// Hard-code edge plays
	if (board[0][1] == 0) return 1;
	if (board[1][0] == 0) return 3;
	if (board[1][2] == 0) return 5;
	if (board[2][1] == 0) return 7;

	// Everything full
	return -1;
}

int checkWin()
{
	for (int i = 1; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 1 && (board[j][0] == board[j][1] && board[j][1] == board[j][2]))
			{
				// Check rows
				if (board[j][0] != 0) return board[j][0];
			}
			if (i == 2 && (board[0][j] == board[1][j] && board[1][j] == board[2][j]))
			{
				// Check columns
				if (board[j][0] != 0) return board[j][0];
			}
		}
	}
	if (board[1][1] != 0 && (board[0][0] == board[1][1] && board[1][1] == board[2][2]) || (board[0][2] != 0 && (board[0][2] == board[1][1] && board[1][1] == board[2][0])))
	{
		return board[1][1];
	}
	return -1;
}

bool checkCheat()
{
	// Call after reading the board. If there are 2 less blank squares, user cheated
	int numDiff = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (board[i][j] != readBoard[i][j]) numDiff++;
		}
	}
	if (numDiff > 1) return true;
	return false;
}

void printBoard(TFileHandle & fout)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			char s;
			if (board[i][j] == 0) s = ' ';
			else if (board[i][j] == 1) s = 'X';
			else if (board[i][j] == 2) s = 'O';
			writeCharPC(fout, s);
			if (j != 2) writeCharPC(fout, '|');
		}
		writeEndlPC(fout);
	}
}

/*task main()
{
	/*int nextMove, pRow, pCol;
	std::cout << checkCheat(board, rBoard);
	for (int i = 0; i < 5; i++)
	{
		std::cout << "Enter your move, as rows, then columns: ";
		std::cin >> pRow >> pCol;
		board[pRow][pCol] = 1;
		std::cout << "Your Turn:" << std::endl;
		printBoard(board);
		if (checkWin(board) == 1)
		{
			std::cout << "You Win!";
			break;
		}
		nextMove = computeNextMove(board);
		board[nextMove / 3][nextMove % 3] = 2;
		std::cout << "Computer Turn:" << std::endl;
		printBoard(board);
		if (checkWin(board) == 2)
		{
			std::cout << "You Suck!";
			break;
		}
	}
	if (checkWin(board) == -1) std::cout << "Tie! You don't suck that bad";*/
//}*/
