#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX 100

// data structures and global variables
// this defines a simple coordinate structure with 2 integers where -
// x -> row position in the grid and y -> column position in the grid
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point point;
    int g, h, f; // g -> cost from start to this node , h -> estimated cost from this node to goal (heuristic) , f -> g + h
} Node;

// min heap structure
typedef struct {
    Node nodes[MAX]; // array of nodes
    int size; // current number of nodes in heap
} MinHeap;

// Add these function prototypes here
void push(MinHeap *heap, Node node);
Node pop(MinHeap *heap);
void heapifyUp(MinHeap *heap, int index);

// global variables
int grid[MAX][MAX]; // stores obstacles and empty cells
int rows, cols;
Point start, goal;
bool visited[MAX][MAX]; // tracks visited cells
char pathGrid[MAX][MAX]; // visual representation of grid
Point parent[MAX][MAX]; // stores path information

// sets up grid 
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

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// for wrong inputs
bool validateInput(int value, int min, int max, const char* errorMsg) {
    if (value < min || value > max) {
        printf("%s (Valid range: %d-%d)\n", errorMsg, min, max);
        clearInputBuffer();
        return false;
    }
    return true;
}

// for entering manual dimens
void enterDimensions() {
    printf("Enter grid dimensions (rows cols): ");
    while (scanf("%d %d", &rows, &cols) != 2 || 
           !validateInput(rows, 1, MAX, "Invalid row dimension!") ||
           !validateInput(cols, 1, MAX, "Invalid column dimension!")) {
        clearInputBuffer();
        printf("Please enter two valid integers for dimensions (rows cols): ");
    }
    initializeGrid();
}

void enterObstacles() {
    int numObstacles;
    printf("Enter number of obstacles: ");
    while (scanf("%d", &numObstacles) != 1 || numObstacles < 0 || numObstacles >= rows * cols) {
        clearInputBuffer();
        printf("Invalid number of obstacles! Please enter a number between 0 and %d: ", rows * cols - 1);
    }

    printf("Enter obstacle positions (row col):\n");
    for (int i = 0; i < numObstacles; i++) {
        int r, c;
        printf("Obstacle %d: ", i + 1);
        while (scanf("%d %d", &r, &c) != 2 || 
               r < 0 || r >= rows || 
               c < 0 || c >= cols || 
               grid[r][c] == 1) {
            clearInputBuffer();
            if (grid[r][c] == 1) {
                printf("Position (%d,%d) already contains an obstacle!\n", r, c);
            } else {
                printf("Invalid position! Enter row (0-%d) and column (0-%d): ", rows-1, cols-1);
            }
        }
        grid[r][c] = 1;
        pathGrid[r][c] = '$';
    }
}

void enterStartGoal() {
    printf("Enter start state (row col): ");
    while (true) {
        if (scanf("%d %d", &start.x, &start.y) != 2 || 
            start.x < 0 || start.x >= rows || 
            start.y < 0 || start.y >= cols) {
            clearInputBuffer();
            printf("Invalid start state! Enter row (0-%d) and column (0-%d): ", rows-1, cols-1);
            continue;
        }
        if (grid[start.x][start.y] == 1) {
            printf("Start position cannot be on an obstacle! Try again: ");
            continue;
        }
        break;
    }

    printf("Enter goal state (row col): ");
    while (true) {
        if (scanf("%d %d", &goal.x, &goal.y) != 2 || 
            goal.x < 0 || goal.x >= rows || 
            goal.y < 0 || goal.y >= cols) {
            clearInputBuffer();
            printf("Invalid goal state! Enter row (0-%d) and column (0-%d): ", rows-1, cols-1);
            continue;
        }
        if (grid[goal.x][goal.y] == 1) {
            printf("Goal position cannot be on an obstacle! Try again: ");
            continue;
        }
        if (goal.x == start.x && goal.y == start.y) {
            printf("Goal position cannot be the same as start position! Try again: ");
            continue;
        }
        break;
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

void bestFirstSearch() {  
    MinHeap openList = { .size = 0 };  // Use the existing MinHeap structure
    bool inOpenList[MAX][MAX] = {false};  // Track nodes in open list
    Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    // Initialize start node
    Node startNode = { 
        .point = start, 
        .g = 0, 
        .h = heuristic(start, goal), 
        .f = heuristic(start, goal) 
    };
    push(&openList, startNode);
    inOpenList[start.x][start.y] = true;
    
    int step = 0;
    bool pathFound = false;
    
    while (openList.size > 0) {
        step++;
        printf("\nStep %d:\n", step);
        
        // Print current open list
        printf("Open List: ");
        for (int i = 0; i < openList.size; i++) {
            printf("(%d,%d)[h=%d] ", 
                openList.nodes[i].point.x, 
                openList.nodes[i].point.y,
                openList.nodes[i].h);
        }
        printf("\n");
        
        // Print current visited nodes
        printf("Visited Nodes: ");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (visited[i][j]) {
                    printf("(%d,%d) ", i, j);
                }
            }
        }
        printf("\n");
        
        Node current = pop(&openList);
        inOpenList[current.point.x][current.point.y] = false;
        visited[current.point.x][current.point.y] = true;
        
        if (current.point.x == goal.x && current.point.y == goal.y) {
            printf("\nGoal reached!\n");
            reconstructPath(current.point);
            pathFound = true;
            break;
        }
        
        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            Point neighbor = {current.point.x + directions[i].x, current.point.y + directions[i].y};
            if (isValid(neighbor) && !visited[neighbor.x][neighbor.y] && !inOpenList[neighbor.x][neighbor.y]) {
                Node neighborNode = { 
                    .point = neighbor,
                    .g = 0,  // Not used in Best First Search
                    .h = heuristic(neighbor, goal),
                    .f = heuristic(neighbor, goal)  // f = h in Best First Search
                };
                push(&openList, neighborNode);
                inOpenList[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = current.point;
            }
        }
        
        // Print current grid state
        printf("\nCurrent Grid State:\n");
        printGrid(pathGrid);
    }
    
    if (!pathFound) {
        printf("\nNo path found!\n");
    }
    
    printf("\nFinal Path Grid:\n");
    printGrid(pathGrid);
    printf("\nHeuristics:\n");
    printHeuristics();
}

void heapifyUp(MinHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->nodes[index].f < heap->nodes[parent].f) {
            Node temp = heap->nodes[index];
            heap->nodes[index] = heap->nodes[parent];
            heap->nodes[parent] = temp;
            index = parent;
        } else {
            break;
        }
    }
}

void push(MinHeap *heap, Node node) {
    heap->nodes[heap->size] = node;
    heapifyUp(heap, heap->size);
    heap->size++;
}

Node pop(MinHeap *heap) {
    Node minNode = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    int index = 0;
    while (true) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;
        if (leftChild < heap->size && heap->nodes[leftChild].f < heap->nodes[smallest].f) {
            smallest = leftChild;
        }
        if (rightChild < heap->size && heap->nodes[rightChild].f < heap->nodes[smallest].f) {
            smallest = rightChild;
        }
        if (smallest == index) {
            break;
        }
        Node temp = heap->nodes[index];
        heap->nodes[index] = heap->nodes[smallest];
        heap->nodes[smallest] = temp;
        index = smallest;
    }
    return minNode;
}

void aStar() {
    MinHeap openList = { .size = 0 };
    Point closedList[MAX];
    int closedSize = 0;
    Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    Node startNode = { .point = start, .g = 0, .h = heuristic(start, goal), .f = heuristic(start, goal) };
    push(&openList, startNode);
    visited[start.x][start.y] = true;
    
    int step = 0;
    bool pathFound = false;
    while (openList.size > 0) {
        step++;
        printf("\nStep %d:\n", step);
        
        // Print current open list
        printf("Open List: ");
        for (int i = 0; i < openList.size; i++) {
            printf("(%d,%d)[g=%d,h=%d,f=%d] ", 
                openList.nodes[i].point.x, 
                openList.nodes[i].point.y,
                openList.nodes[i].g,
                openList.nodes[i].h,
                openList.nodes[i].f);
        }
        printf("\n");
        
        // Print current closed list
        printf("Closed List: ");
        for (int i = 0; i < closedSize; i++) {
            printf("(%d,%d) ", closedList[i].x, closedList[i].y);
        }
        printf("\n");
        
        Node current = pop(&openList);
        closedList[closedSize++] = current.point;
        
        if (current.point.x == goal.x && current.point.y == goal.y) {
            printf("\nGoal reached!\n");
            reconstructPath(current.point);
            pathFound = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            Point neighbor = {current.point.x + directions[i].x, current.point.y + directions[i].y};
            if (isValid(neighbor) && !visited[neighbor.x][neighbor.y]) {
                Node neighborNode = { 
                    .point = neighbor,  
                    .g = current.g + 1, 
                    .h = heuristic(neighbor, goal), 
                    .f = current.g + 1 + heuristic(neighbor, goal) 
                };
                push(&openList, neighborNode);
                visited[neighbor.x][neighbor.y] = true;
                parent[neighbor.x][neighbor.y] = current.point;
            }
        }
        
        // Print current grid state
        printf("\nCurrent Grid State:\n");
        printGrid(pathGrid);
    }
    
    if (!pathFound) {
        printf("\nNo path found!\n");
        // Print final lists even when no path is found
        printf("\nFinal Open List: ");
        for (int i = 0; i < openList.size; i++) {
            printf("(%d,%d)[g=%d,h=%d,f=%d] ", 
                openList.nodes[i].point.x, 
                openList.nodes[i].point.y,
                openList.nodes[i].g,
                openList.nodes[i].h,
                openList.nodes[i].f);
        }
        printf("\nFinal Closed List: ");
        for (int i = 0; i < closedSize; i++) {
            printf("(%d,%d) ", closedList[i].x, closedList[i].y);
        }
        printf("\n");
    }
    
    printf("\nFinal Path Grid:\n");
    printGrid(pathGrid);
    printf("\nHeuristics:\n");
    printHeuristics();
}

bool loadFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("\n  Error: Could not open file %s\n", filename);
        return false;
    }

    // Read dimensions from first line
    if (fscanf(file, "%d %d", &rows, &cols) != 2) {
        printf("\n  Error: Invalid dimensions in file\n");
        fclose(file);
        return false;
    }

    if (rows <= 0 || rows > MAX || cols <= 0 || cols > MAX) {
        printf("\n  Error: Invalid grid dimensions in file\n");
        fclose(file);
        return false;
    }

    initializeGrid();

    // Skip the header line with column numbers
    char line[MAX * 4];
    fgets(line, sizeof(line), file);  // Skip newline after dimensions
    fgets(line, sizeof(line), file);  // Skip column numbers
    fgets(line, sizeof(line), file);  // Skip separator line

    // Read the grid
    int numObstacles = 0;
    for (int i = 0; i < rows; i++) {
        // Skip row number and first |
        if (fscanf(file, "%*d |") != 0) {
            printf("\n  Error: Invalid row format in file\n");
            fclose(file);
            return false;
        }

        // Read each cell in the row
        for (int j = 0; j < cols; j++) {
            char cell;
            // Read cell content (skipping spaces)
            if (fscanf(file, " %c |", &cell) != 1) {
                printf("\n  Error: Invalid cell format in file\n");
                fclose(file);
                return false;
            }

            switch (cell) {
                case 'S':
                    start.x = i;
                    start.y = j;
                    break;
                case 'G':
                    goal.x = i;
                    goal.y = j;
                    break;
                case '$':
                    grid[i][j] = 1;
                    pathGrid[i][j] = '$';
                    numObstacles++;
                    break;
                case '-':
                    // Empty cell
                    break;
                default:
                    printf("\n  Error: Invalid character '%c' in grid\n", cell);
                    fclose(file);
                    return false;
            }
        }
        // Skip newline
        fgets(line, sizeof(line), file);
    }

    // After successful loading, display the configuration
    printf("\nLoaded Configuration:\n");
    printf("Grid Size: %d x %d\n", rows, cols);
    printf("Number of Obstacles: %d\n", numObstacles);
    printf("\nInitial Grid State:\n");
    printGrid(pathGrid);
    printf("\nStart Position: (%d, %d)\n", start.x, start.y);
    printf("Goal Position: (%d, %d)\n", goal.x, goal.y);

    fclose(file);
    return true;
}

int main() {
    int choice;
    bool dimensionsSet = false;
    bool obstaclesSet = false;
    bool startGoalSet = false;

    printf("\nPath Finding Program\n");
    printf("-------------------\n");

    do {
        printf("\nMenu Options:\n");
        printf("1. Load Configuration from File\n");
        printf("2. Configure Grid Size\n");
        printf("3. Place Obstacles\n");
        printf("4. Set Start & Goal Points\n");
        printf("5. Run Best First Search Algorithm\n");
        printf("6. Run A* Algorithm\n");
        printf("7. Exit\n");
        
        printf("\nEnter your choice (1-7): ");
        
        while (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input! Please enter a number (1-7): ");
        }
        
        switch (choice) {
            case 1: {
                char filename[100];
                printf("\nEnter input file name: ");
                scanf("%s", filename);
                if (loadFromFile(filename)) {
                    dimensionsSet = true;
                    obstaclesSet = true;
                    startGoalSet = true;
                    printf("Configuration loaded successfully!\n");
                }
                break;
            }
            case 2:
                dimensionsSet = true;
                obstaclesSet = false;
                startGoalSet = false;
                enterDimensions();
                break;
            case 3:
                if (!dimensionsSet) {
                    printf("Error: Please configure grid size first (Option 2)\n");
                } else {
                    obstaclesSet = true;
                    enterObstacles();
                }
                break;
            case 4:
                if (!dimensionsSet) {
                    printf("Error: Please configure grid size first (Option 2)\n");
                } else {
                    startGoalSet = true;
                    enterStartGoal();
                }
                break;
            case 5:
                if (!dimensionsSet || !startGoalSet) {
                    printf("Error: Please complete the setup first!\n");
                } else {
                    printf("\nRunning Best First Search Algorithm:\n");
                    bestFirstSearch();
                }
                break;
            case 6:
                if (!dimensionsSet || !startGoalSet) {
                    printf("Error: Please complete the setup first!\n");
                } else {
                    printf("\nRunning A* Algorithm:\n");
                    aStar();
                }
                break;
            case 7:
                printf("\nExiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid option! Please enter a number between 1 and 7.\n");
        }
    } while (choice != 7);
    
    return 0;
}