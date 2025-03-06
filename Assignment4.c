#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX 100

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point point;
    int g, h, f;
} Node;

typedef struct {
    Node nodes[MAX];
    int size;
} PriorityQueue;

int grid[MAX][MAX];
int rows, cols;
Point start, goal;
bool visited[MAX][MAX];
char pathGrid[MAX][MAX];
Point parent[MAX][MAX];

void initializeGrid() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = 0;
            visited[i][j] = false;
            pathGrid[i][j] = ' ';
            parent[i][j] = (Point){-1, -1};
        }
    }
}

void enterDimensions() {
    printf("Enter grid dimensions (rows cols): ");
    if (scanf("%d %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0 || rows > MAX || cols > MAX) {
        printf("Invalid dimensions! Please enter positive integers less than or equal to %d.\n", MAX);
        exit(1);
    }
    initializeGrid();
}

void enterObstacles() {
    int numObstacles;
    printf("Enter number of obstacles: ");
    if (scanf("%d", &numObstacles) != 1 || numObstacles < 0) {
        printf("Invalid number of obstacles! Please enter a non-negative integer.\n");
        exit(1);
    }
    printf("Enter obstacle positions (row col):\n");
    for (int i = 0; i < numObstacles; i++) {
        int r, c;
        if (scanf("%d %d", &r, &c) != 2 || r < 0 || r >= rows || c < 0 || c >= cols) {
            printf("Invalid obstacle position! Please enter valid row and column indices.\n");
            exit(1);
        }
        grid[r][c] = 1;
        pathGrid[r][c] = '$';
    }
}

void enterStartGoal() {
    printf("Enter start state (row col): ");
    if (scanf("%d %d", &start.x, &start.y) != 2 || start.x < 0 || start.x >= rows || start.y < 0 || start.y >= cols) {
        printf("Invalid start state! Please enter valid row and column indices.\n");
        exit(1);
    }
    printf("Enter goal state (row col): ");
    if (scanf("%d %d", &goal.x, &goal.y) != 2 || goal.x < 0 || goal.x >= rows || goal.y < 0 || goal.y >= cols) {
        printf("Invalid goal state! Please enter valid row and column indices.\n");
        exit(1);
    }
}

int heuristic(Point a, Point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

bool isValid(Point p) {
    return (p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols && grid[p.x][p.y] == 0);
}

void printGrid(char grid[MAX][MAX]) {
    printf("  ");
    for (int j = 0; j < cols; j++) {
        printf(" %2d ", j);
    }
    printf("\n");
    for (int i = 0; i < rows; i++) {
        printf("   ");
        for (int j = 0; j < cols; j++) {
            printf("----");
        }
        printf("-\n");
        printf("%2d |", i);
        for (int j = 0; j < cols; j++) {
            printf(" %c |", grid[i][j]);
        }
        printf("\n");
    }
    printf("   ");
    for (int j = 0; j < cols; j++) {
        printf("----");
    }
    printf("-\n");
}

void printHeuristics() {
    printf("  ");
    for (int j = 0; j < cols; j++) {
        printf(" %2d ", j);
    }
    printf("\n");
    for (int i = 0; i < rows; i++) {
        printf("   ");
        for (int j = 0; j < cols; j++) {
            printf("----");
        }
        printf("-\n");
        printf("%2d |", i);
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 1) {
                printf(" $ |");
            } else {
                printf("%2d |", heuristic((Point){i, j}, goal));
            }
        }
        printf("\n");
    }
    printf("   ");
    for (int j = 0; j < cols; j++) {
        printf("----");
    }
    printf("-\n");
}

void reconstructPath(Point current) {
    while (parent[current.x][current.y].x != -1 && parent[current.x][current.y].y != -1) {
        pathGrid[current.x][current.y] = '*';
        current = parent[current.x][current.y];
    }
    pathGrid[start.x][start.y] = 'S';
    pathGrid[goal.x][goal.y] = 'G';
}

void bfs() {
    Point openList[MAX], closedList[MAX];
    int openSize = 0, closedSize = 0;
    Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    openList[openSize++] = start;
    visited[start.x][start.y] = true;
    
    while (openSize > 0) {
        Point current = openList[0];
        for (int i = 0; i < openSize - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openSize--;
        closedList[closedSize++] = current;
        
        if (current.x == goal.x && current.y == goal.y) {
            printf("Goal reached!\n");
            reconstructPath(current);
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            Point neighbor = {current.x + directions[i].x, current.y + directions[i].y};
            if (isValid(neighbor) && !visited[neighbor.x][neighbor.y]) {
                openList[openSize++] = neighbor;
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = current;
            }
        }
    }
    
    printf("Final Open List: ");
    for (int i = 0; i < openSize; i++) {
        printf("(%d, %d) ", openList[i].x, openList[i].y);
    }
    printf("\nFinal Closed List: ");
    for (int i = 0; i < closedSize; i++) {
        printf("(%d, %d) ", closedList[i].x, closedList[i].y);
    }
    printf("\n");
    
    printf("Path Grid:\n");
    printGrid(pathGrid);
    printf("Heuristics:\n");
    printHeuristics();
}

void push(PriorityQueue *pq, Node node) {
    pq->nodes[pq->size++] = node;
}

Node pop(PriorityQueue *pq) {
    int minIndex = 0;
    for (int i = 1; i < pq->size; i++) {
        if (pq->nodes[i].f < pq->nodes[minIndex].f) {
            minIndex = i;
        }
    }
    Node minNode = pq->nodes[minIndex];
    for (int i = minIndex; i < pq->size - 1; i++) {
        pq->nodes[i] = pq->nodes[i + 1];
    }
    pq->size--;
    return minNode;
}

void aStar() {
    PriorityQueue openList = { .size = 0 };
    Point closedList[MAX];
    int closedSize = 0;
    Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    Node startNode = { .point = start, .g = 0, .h = heuristic(start, goal), .f = heuristic(start, goal) };
    push(&openList, startNode);
    visited[start.x][start.y] = true;
    
    while (openList.size > 0) {
        Node current = pop(&openList);
        closedList[closedSize++] = current.point;
        
        if (current.point.x == goal.x && current.point.y == goal.y) {
            printf("Goal reached!\n");
            reconstructPath(current.point);
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            Point neighbor = {current.point.x + directions[i].x, current.point.y + directions[i].y};
            if (isValid(neighbor) && !visited[neighbor.x][neighbor.y]) {
                Node neighborNode = { .point = neighbor, .g = current.g + 1, .h = heuristic(neighbor, goal), .f = current.g + 1 + heuristic(neighbor, goal) };
                push(&openList, neighborNode);
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = current.point;
            }
        }
    }
    
    printf("Final Open List: ");
    for (int i = 0; i < openList.size; i++) {
        printf("(%d, %d) ", openList.nodes[i].point.x, openList.nodes[i].point.y);
    }
    printf("\nFinal Closed List: ");
    for (int i = 0; i < closedSize; i++) {
        printf("(%d, %d) ", closedList[i].x, closedList[i].y);
    }
    printf("\n");
    
    printf("Path Grid:\n");
    printGrid(pathGrid);
    printf("Heuristics:\n");
    printHeuristics();
}

int main() {
    int choice;
    do {
        printf("Menu:\n");
        printf("1. Enter input dimensions\n");
        printf("2. Enter obstacles\n");
        printf("3. Enter start state and goal state\n");
        printf("4. Use BFS\n");
        printf("5. Use A*\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice! Please enter a valid integer.\n");
            exit(1);
        }
        
        switch (choice) {
            case 1:
                enterDimensions();
                break;
            case 2:
                enterObstacles();
                break;
            case 3:
                enterStartGoal();
                break;
            case 4:
                bfs();
                break;
            case 5:
                aStar();
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
    
    return 0;
}