#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define SIZE 3
#define EMPTY ' '

char USER;
char AI;
char MAX_PLAYER = 'X';
char MIN_PLAYER = 'O';
void printBoard(char board[SIZE][SIZE]);
bool isGameOver(char board[SIZE][SIZE]);
int cal(char board[SIZE][SIZE]);
void userM(char board[SIZE][SIZE], int *moveCount);
int minMax(char board[SIZE][SIZE], int depth, bool isMaximizing);
void aiM(char board[SIZE][SIZE], int ply, int *moveCount);

char getUserSymbol() {
    char symbol;
    while (1) {
        printf("Choose your symbol (X/O): ");
        if (scanf(" %c", &symbol) != 1) {
            printf("Error: Invalid input! Please enter X or O.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        symbol = toupper(symbol);
        if (symbol == 'X' || symbol == 'O') {
            return symbol;
        }
        printf("Error: Invalid symbol! Please enter X or O.\n");
        while (getchar() != '\n'); 
    }
}

int getPlyDepth() {
    int ply;
    while (1) {
        printf("Choose Min-Max search depth (1 or 2): ");
        if (scanf("%d", &ply) != 1) {
            printf("Error: Invalid input! Please enter 1 or 2.\n");
            while (getchar() != '\n'); 
            continue;
        }
        
        if (ply == 1 || ply == 2) {
            return ply;
        }
        printf("Error: Invalid depth! Please enter 1 or 2.\n");
        while (getchar() != '\n'); 
    }
}

int main()
{
    char board[SIZE][SIZE] = {
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY}};

    
    USER = getUserSymbol();
    AI = (USER == 'X') ? 'O' : 'X';

    
    int ply = getPlyDepth();

    int currentPlayer = (USER == 'X') ? 1 : 2;

    int moveCount = 0;
    bool gameOver = false;

    while (!gameOver)
    {
        printf("\n");

        if (currentPlayer == 1)
        {
            userM(board, &moveCount);
        }
        else
        {
            aiM(board, ply, &moveCount);
        }

        printBoard(board);

        if (isGameOver(board))
        {
            int evaluation = cal(board);
            if (evaluation > 0)
            {
                printf("Congratulations! %c wins!\n", MIN_PLAYER);
            }
            else if (evaluation < 0)
            {
                printf("Congratulations! %c wins!\n", MAX_PLAYER);
            }
            else
            {
                printf("It's a draw!\n");
            }
            gameOver = true;
        }
        else if (moveCount == SIZE * SIZE)
        {
            printf("It's a draw!\n");
            gameOver = true;
        }
        currentPlayer = 3 - currentPlayer; 
    }

    return 0;
}   

void printBoard(char board[SIZE][SIZE]) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" %c | %c | %c \n", board[i][0], board[i][1], board[i][2]);
        if (i < SIZE - 1) {
            printf("---|---|---\n");
        }
    }
    printf("\n");
}

bool isGameOver(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        if ((board[i][0] != EMPTY && board[i][0] == board[i][1] && board[i][1] == board[i][2]) ||
            (board[0][i] != EMPTY && board[0][i] == board[1][i] && board[1][i] == board[2][i]))
        {
            return true;
        }
    }

    if ((board[0][0] != EMPTY && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
        (board[0][2] != EMPTY && board[0][2] == board[1][1] && board[1][1] == board[2][0]))
    {
        return true;
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
                return false; 
        }
    }

    return true; 
}

int cal(char board[SIZE][SIZE])
{
    int heuristic = 0;

    for (int i = 0; i < SIZE; i++)
    {
        int rowUserCount = 0, rowAICount = 0;
        int colUserCount = 0, colAICount = 0;

        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == MAX_PLAYER)
                rowUserCount++;
            if (board[i][j] == MIN_PLAYER)
                rowAICount++;
        }
        if (rowUserCount > 0 && rowUserCount < SIZE)
            heuristic++;
        if (rowAICount > 0 && rowAICount < SIZE)
            heuristic--;

        for (int j = 0; j < SIZE; j++)
        {
            if (board[j][i] == MAX_PLAYER)
                colUserCount++;
            if (board[j][i] == MIN_PLAYER)
                colAICount++;
        }
        if (colUserCount > 0 && colUserCount < SIZE)
            heuristic++;
        if (colAICount > 0 && colAICount < SIZE)
            heuristic--;
    }

    int diag1UserCount = 0, diag1AICount = 0;
    int diag2UserCount = 0, diag2AICount = 0;

    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][i] == MAX_PLAYER)
            diag1UserCount++;
        if (board[i][i] == MIN_PLAYER)
            diag1AICount++;
        if (board[i][SIZE - 1 - i] == MAX_PLAYER)
            diag2UserCount++;
        if (board[i][SIZE - 1 - i] == MIN_PLAYER)
            diag2AICount++;
    }
    if (diag1UserCount > 0 && diag1UserCount < SIZE)
        heuristic++;
    if (diag1AICount > 0 && diag1AICount < SIZE)
        heuristic--;
    if (diag2UserCount > 0 && diag2UserCount < SIZE)
        heuristic++;
    if (diag2AICount > 0 && diag2AICount < SIZE)
        heuristic--;

    return heuristic;
}

void userM(char board[SIZE][SIZE], int *moveCount)
{
    int row, col;
    printf("Your turn (%c):\n", USER);
    while (1)
    {
        printf("Enter row and column (0-2) separated by space: ");
        if (scanf("%d %d", &row, &col) != 2) {
            printf("Error: Invalid input! Please enter two numbers separated by space.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Check range
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
            printf("Error: Position out of range! Row and column must be between 0 and 2.\n");
            continue;
        }

        // Check if position is empty
        if (board[row][col] != EMPTY) {
            printf("Error: Position already occupied! Choose an empty position.\n");
            continue;
        }

        // Valid move
        board[row][col] = USER;
        (*moveCount)++;
        break;
    }
}

int minMax(char board[SIZE][SIZE], int depth, bool isMaximizing)
{
    int score = cal(board);

    if (score > 0) 
        return score - depth; // winning score for max 
    if (score < 0)
        return score + depth; // winning score for min
    if (isGameOver(board))
        return 0; // draw score

    if (isMaximizing)
    {
        int maxEval = -1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = AI; // try this move
                    int eval = minMax(board, depth + 1, false); // call minMax for the other player
                    board[i][j] = EMPTY; // undo the move
                    if (eval > maxEval)
                        maxEval = eval; // update the maxEval
                }
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = 1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = USER;
                    int eval = minMax(board, depth + 1, true);
                    board[i][j] = EMPTY; 
                    if (eval < minEval)
                        minEval = eval;
                }
            }
        }
        return minEval;
    }
}

void aiM(char board[SIZE][SIZE], int ply, int *moveCount)
{
    int bestMoveRow = -1;
    int bestMoveCol = -1;
    int bestScore = 1000; // we assign a high value when user is X
    int bestScore1 = -1000; // we assign a low value when user is O
    int currentMoveCount = 0;

    printf("AI evaluating moves:\n");

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                board[i][j] = AI;
                int score = -1000;
                int score1 = 1000;

                if (ply == 2) // looks two moves ahead
                {
                    for (int k = 0; k < SIZE; k++)
                    {
                        for (int l = 0; l < SIZE; l++)
                        {
                            if (board[k][l] == EMPTY)
                            {
                                board[k][l] = USER;
                                int moveScore = minMax(board, 0, false);

                                printf("AI's move (%d, %d) followed by USER's move (%d, %d) Heuristic: %d\n", i, j, k, l, moveScore);
                                printBoard(board);

                                board[k][l] = EMPTY; 

                                if (USER == 'X')
                                {
                                    if (board[k][l] == EMPTY) // if the position is empty we update the score
                                    {
                                        if (moveScore > score)
                                            score = moveScore;
                                    }
                                }

                                if (USER == 'O')
                                {
                                    if (board[k][l] == EMPTY)
                                    {
                                        if (moveScore < score1)
                                            score1 = moveScore;
                                    }
                                }
                            }
                        }
                    }
                }
                else // ply == 1
                {
                    if (USER == 'X')
                        score = minMax(board, 0, false);
                    if (USER == 'O')
                        score1 = minMax(board, 0, false);
                }

                if (USER == 'X')
                    printf("Move %d: (%d, %d) Heuristic: %d\n", ++currentMoveCount, i, j, score);
                if (USER == 'O')
                    printf("Move %d: (%d, %d) Heuristic: %d\n", ++currentMoveCount, i, j, score1);

                board[i][j] = AI;
                printBoard(board);
                board[i][j] = EMPTY; 

                if (USER == 'X')
                {
                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestMoveRow = i;
                        bestMoveCol = j;
                    }
                }
                if (USER == 'O')
                {
                    if (score1 > bestScore1)
                    {
                        bestScore1 = score1;
                        bestMoveRow = i;
                        bestMoveCol = j;
                    }
                }
            }
        }
    }
    if (USER == 'X')
        printf("selected move: (%d, %d) with Heuristic: %d\n", bestMoveRow, bestMoveCol, bestScore);

    if (USER == 'O')
        printf("selected move: (%d, %d) with Heuristic: %d\n", bestMoveRow, bestMoveCol, bestScore1);

    board[bestMoveRow][bestMoveCol] = AI;
    (*moveCount)++;
}