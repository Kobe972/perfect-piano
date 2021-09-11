#include"generator.h"
//Get grid as string in row major order
string Generator::getGrid()
{
    string s = "";
    for (int row_num = 0; row_num < 9; ++row_num)
    {
        for (int col_num = 0; col_num < 9; ++col_num)
        {
            s = s + to_string(grid[row_num][col_num]);
        }
    }
    return s;
}

//Generate random number
int genRandNum(int maxLimit)
{
    return rand() % maxLimit;
}

// Create seed grid
void Generator::createSeed()
{
    this->solveGrid();
    // Saving the solution grid
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            this->solnGrid[i][j] = this->grid[i][j];
        }
    }
}

Generator::Generator()
{

    // initialize difficulty level
    this->difficultyLevel = 0;

    // Randomly shuffling the array of removing grid positions
    for (int i = 0; i < 81; i++)
    {
        this->gridPos[i] = i;
    }

    random_shuffle(this->gridPos, (this->gridPos) + 81, genRandNum);

    // Randomly shuffling the guessing number array
    for (int i = 0; i < 9; i++)
    {
        this->guessNum[i] = i + 1;
    }

    random_shuffle(this->guessNum, (this->guessNum) + 9, genRandNum);

    // Initialising the grid
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            this->grid[i][j] = 0;
        }
    }
}


bool FindUnassignedLocation(int grid[9][9], int& row, int& col)
{
    for (row = 0; row < 9; row++)
    {
        for (col = 0; col < 9; col++)
        {
            if (grid[row][col] == UNASSIGNED)
                return true;
        }
    }
    return false;
}

bool UsedInRow(int grid[9][9], int row, int num)
{
    for (int col = 0; col < 9; col++)
    {
        if (grid[row][col] == num)
            return true;
    }
    return false;
}

bool UsedInCol(int grid[9][9], int col, int num)
{
    for (int row = 0; row < 9; row++)
    {
        if (grid[row][col] == num)
            return true;
    }
    return false;
}

bool UsedInBox(int grid[9][9], int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
        }
    }
    return false;
}

bool isSafe(int grid[9][9], int row, int col, int num)
{
    return !UsedInRow(grid, row, num) && !UsedInCol(grid, col, num) && !UsedInBox(grid, row - row % 3, col - col % 3, num);
}

// Modified Sudoku solver
bool Generator::solveGrid()
{
    int row, col;
    // If there is no unassigned location, we are done
    if (!FindUnassignedLocation(this->grid, row, col))
        return true; // success!
     // Consider digits 1 to 9
    for (int num = 0; num < 9; num++)
    {
        // if looks promising
        if (isSafe(this->grid, row, col, this->guessNum[num]))
        {
            // make tentative assignment
            this->grid[row][col] = this->guessNum[num];
            // return, if success
            if (solveGrid())
                return true;
            // failure, unmake & try again
            this->grid[row][col] = UNASSIGNED;
        }
    }
    return false; // this triggers backtracking
}

// Check if the grid is uniquely solvable
void Generator::countSoln(int& number)
{
    int row, col;

    if (!FindUnassignedLocation(this->grid, row, col))
    {
        number++;
        return;
    }

    for (int i = 0; i < 9 && number < 2; i++)
    {
        if (isSafe(this->grid, row, col, this->guessNum[i]))
        {
            this->grid[row][col] = this->guessNum[i];
            countSoln(number);
        }
        this->grid[row][col] = UNASSIGNED;
    }
}

// Generate puzzle
void Generator::genPuzzle()
{
    for (int i = 0; i < 81; i++)
    {
        int x = (this->gridPos[i]) / 9;
        int y = (this->gridPos[i]) % 9;
        int temp = this->grid[x][y];
        this->grid[x][y] = UNASSIGNED;

        // If now more than 1 solution , replace the removed cell back.
        int check = 0;
        countSoln(check);
        if (check != 1)
        {
            this->grid[x][y] = temp;
        }
    }
}
int Generator::branchDifficultyScore()
{
    int emptyPositions = -1;
    int tempGrid[9][9];
    int sum = 0;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            tempGrid[i][j] = this->grid[i][j];
        }
    }

    while (emptyPositions != 0)
    {
        vector<vector<int> > empty;

        for (int i = 0; i < 81; i++)
        {
            if (tempGrid[(int)(i / 9)][(int)(i % 9)] == 0)
            {
                vector<int> temp;
                temp.push_back(i);

                for (int num = 1; num <= 9; num++)
                {
                    if (isSafe(tempGrid, i / 9, i % 9, num))
                    {
                        temp.push_back(num);
                    }
                }

                empty.push_back(temp);
            }

        }

        if (empty.size() == 0)
        {
            cout << "Hello: " << sum << endl;
            return sum;
        }

        int minIndex = 0;

        int check = empty.size();
        for (int i = 0; i < check; i++)
        {
            if (empty[i].size() < empty[minIndex].size())
                minIndex = i;
        }

        int branchFactor = empty[minIndex].size();
        int rowIndex = empty[minIndex][0] / 9;
        int colIndex = empty[minIndex][0] % 9;

        tempGrid[rowIndex][colIndex] = this->solnGrid[rowIndex][colIndex];
        sum = sum + ((branchFactor - 2) * (branchFactor - 2));

        emptyPositions = empty.size() - 1;
    }

    return sum;

}
void Generator::calculateDifficulty()
{
    int B = branchDifficultyScore();
    int emptyCells = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (this->grid[i][j] == 0)
                emptyCells++;
        }
    }
    this->difficultyLevel = B * 100 + emptyCells;
}

void Generator::CreateSudoku()
{
    srand((unsigned)time(NULL));
    createSeed();
    genPuzzle();
}