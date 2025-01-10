#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SIZE 3
#define EMPTY 2
#define X_PLAYER 3
#define O_PLAYER 5

void clearScreen() {
    system("cls");
}

void delay(int milliseconds) {
    Sleep(milliseconds);
}

void initializeBoard(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
}

void displayBoard(int board[SIZE][SIZE]) {
    clearScreen();
    printf("\n");
    printf("     1   2   3  \n");
    printf("   +---+---+---+\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" %d |", i + 1);
        for (int j = 0; j < SIZE; j++) {
            char symbol = (board[i][j] == X_PLAYER) ? 'X' : 
                         (board[i][j] == O_PLAYER) ? 'O' : ' ';
            printf(" %c |", symbol);
        }
        printf("\n   +---+---+---+\n");
    }
}

void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getPlayerChoice() {
    int choice;
    while (true) {
        printf("Choose your symbol (1=X, 2=O): ");
        if (scanf("%d", &choice) == 1 && (choice == 1 || choice == 2)) {
            flushInput();
            return choice;
        }
        printf("Invalid input! Please enter 1 or 2.\n");
        flushInput();
    }
}

bool isValidMove(int board[SIZE][SIZE], int row, int col) {
    return (row >= 0 && row < SIZE && 
            col >= 0 && col < SIZE && 
            board[row][col] == EMPTY);
}

bool isWinningMove(int board[SIZE][SIZE], int player) {
    int winProduct = player * player * player;
    
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] * board[i][1] * board[i][2] == winProduct ||
            board[0][i] * board[1][i] * board[2][i] == winProduct) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0][0] * board[1][1] * board[2][2] == winProduct ||
        board[0][2] * board[1][1] * board[2][0] == winProduct) {
        return true;
    }
    
    return false;
}

bool isFull(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY) return false;
        }
    }
    return true;
}

void playerMove(int board[SIZE][SIZE], int player) {
    int row, col;
    while (true) {
        printf("Enter row and column (1-3): ");
        if (scanf("%d %d", &row, &col) == 2) {
            row--; col--;
            if (isValidMove(board, row, col)) {
                board[row][col] = player;
                flushInput();
                break;
            }
            printf("Invalid move! Try again.\n");
        } else {
            printf("Invalid input! Enter two numbers.\n");
            flushInput();
        }
    }
}

void machineMove(int board[SIZE][SIZE], int machine, int player) {
    // Check for winning move
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = machine;
                if (isWinningMove(board, machine)) {
                    return;
                }
                board[i][j] = EMPTY;
            }
        }
    }

    // Block player's winning move
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = player;
                if (isWinningMove(board, player)) {
                    board[i][j] = machine;
                    return;
                }
                board[i][j] = EMPTY;
            }
        }
    }

    // Take center if available
    if (board[1][1] == EMPTY) {
        board[1][1] = machine;
        return;
    }

    // Take corners
    int corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (int i = 0; i < 4; i++) {
        if (board[corners[i][0]][corners[i][1]] == EMPTY) {
            board[corners[i][0]][corners[i][1]] = machine;
            return;
        }
    }

    // Take any available space
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = machine;
                return;
            }
        }
    }
}

bool playAgain() {
    char response;
    printf("\nPlay again? (y/n): ");
    flushInput();
    scanf("%c", &response);
    return (response == 'y' || response == 'Y');
}

int main() {
    int board[SIZE][SIZE];
    bool continueGame = true;

    while (continueGame) {
        initializeBoard(board);
        clearScreen();
        printf("=== Tic Tac Toe ===\n\n");
        
        int playerChoice = getPlayerChoice();
        int player = (playerChoice == 1) ? X_PLAYER : O_PLAYER;
        int machine = (player == X_PLAYER) ? O_PLAYER : X_PLAYER;
        int currentPlayer = X_PLAYER;

        while (true) {
            displayBoard(board);
            
            if (currentPlayer == player) {
                printf("\nYour turn:\n");
                playerMove(board, player);
            } else {
                printf("\nMachine thinking...");
                delay(1000);  // Add delay for better UX
                machineMove(board, machine, player);
            }

            if (isWinningMove(board, currentPlayer)) {
                displayBoard(board);
                printf("\n%s wins!\n", 
                       currentPlayer == player ? "You" : "Machine");
                break;
            }

            if (isFull(board)) {
                displayBoard(board);
                printf("\nIt's a draw!\n");
                break;
            }

            currentPlayer = (currentPlayer == X_PLAYER) ? O_PLAYER : X_PLAYER;
        }

        continueGame = playAgain();
    }

    printf("\nThanks for playing!\n");
    return 0;
}