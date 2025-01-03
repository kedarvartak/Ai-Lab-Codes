#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME 50
#define MAX_STUDENTS 100

// Structure definition for student
struct Student {
    int rollNo;
    char name[MAX_NAME];
    float marks1;
    float marks2;
    float marks3;
};

// Function to swap two students
void swap(struct Student *a, struct Student *b) {
    struct Student temp = *a;
    *a = *b;
    *b = temp;
}

// QuickSort function
int partition(struct Student arr[], int low, int high) {
    int pivot = arr[high].rollNo;
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].rollNo < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(struct Student arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// MergeSort functions
void merge(struct Student arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    
    struct Student *L = (struct Student *)malloc(n1 * sizeof(struct Student));
    struct Student *R = (struct Student *)malloc(n2 * sizeof(struct Student));
    
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
        
    i = 0;
    j = 0;
    k = l;
    
    while (i < n1 && j < n2) {
        if (L[i].rollNo <= R[j].rollNo) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    free(L);
    free(R);
}

void mergeSort(struct Student arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Function to print student data
void printStudents(struct Student arr[], int n) {
    printf("\nStudent Records:\n");
    printf("Roll No\tName\t\t\tMarks1\tMarks2\tMarks3\n");
    printf("--------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%-20s\t%.2f\t%.2f\t%.2f\n", arr[i].rollNo, arr[i].name, arr[i].marks1, arr[i].marks2, arr[i].marks3);
    }
}

int main() {
    struct Student students[MAX_STUDENTS];
    int n;
    
    printf("Enter number of students: ");
    scanf("%d", &n);
    
    // Input student data
    for (int i = 0; i < n; i++) {
        printf("\nEnter details for student %d:\n", i + 1);
        printf("Roll No: ");
        scanf("%d", &students[i].rollNo);
        printf("Name (First Last): ");
        getchar(); // Clear buffer
        fgets(students[i].name, MAX_NAME, stdin);
        students[i].name[strcspn(students[i].name, "\n")] = 0; // Remove newline
        printf("Marks1: ");
        scanf("%f", &students[i].marks1);
        printf("Marks2: "); 
        scanf("%f", &students[i].marks2);
        printf("Marks3: ");
        scanf("%f", &students[i].marks3);
    }
    
    // Create copies for both sorting methods
    struct Student studentsQuick[MAX_STUDENTS];
    struct Student studentsMerge[MAX_STUDENTS];
    memcpy(studentsQuick, students, n * sizeof(struct Student));
    memcpy(studentsMerge, students, n * sizeof(struct Student));
    
    printf("\nOriginal Data:");
    printStudents(students, n);
    
    // QuickSort
    quickSort(studentsQuick, 0, n - 1);
    printf("\nAfter QuickSort:");
    printStudents(studentsQuick, n);
    
    // MergeSort
    mergeSort(studentsMerge, 0, n - 1);
    printf("\nAfter MergeSort:");
    printStudents(studentsMerge, n);
    
    return 0;
}
