#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define SIZE 5 // Tamanho da grade (cruzamento 5x5)
#define EMPTY 0
#define CAR 1
#define HORIZONTAL 0
#define VERTICAL 1

int grid[SIZE][SIZE]; // Grade do cruzamento
HANDLE mutex; // Mutex para controle de acesso à grade
int semaphore = HORIZONTAL; // Semáforo começa permitindo o fluxo horizontal

void initializeGrid() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = EMPTY;
        }
    }
}

void printGrid() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                
                printf(" X ");
            } else {
                
                
                printf(" . ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void moveCars(int direction) {
    WaitForSingleObject(mutex, INFINITE);

    // Limpa a posição anterior
    for (int i = SIZE - 1; i >= 0; i--) {
        for (int j = SIZE - 1; j >= 0; j--) {
            if (grid[i][j] == CAR) {
                grid[i][j] = EMPTY;
            }
        }
    }

    // Move os carros na direção especificada
    if (direction == HORIZONTAL) {
        for (int i = 0; i < SIZE; i++) {
            grid[2][i] = CAR;
        }
    } else if (direction == VERTICAL) {
        for (int i = 0; i < SIZE; i++) {
            grid[i][2] = CAR;
        }
    }

    ReleaseMutex(mutex);
}

DWORD WINAPI trafficLight(LPVOID arg) {
    while (1) {
        Sleep(3000); // Tempo que um sentido fica verde (em milissegundos)

        WaitForSingleObject(mutex, INFINITE);

        if (semaphore == HORIZONTAL) {
            semaphore = VERTICAL;
        } else {
            semaphore = HORIZONTAL;
        }

        ReleaseMutex(mutex);
    }
}

DWORD WINAPI manageTraffic(LPVOID arg) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);

        if (semaphore == HORIZONTAL) {
            moveCars(HORIZONTAL);
            printf("  Sentido horizontal\n");
            printf(" \n");
            printf(" \n");
        } else {
            moveCars(VERTICAL);
            printf("  Sentido vertical\n");
            printf(" \n");
            printf(" \n");
        }

        ReleaseMutex(mutex);

        printGrid();
        Sleep(1000); // Intervalo entre movimentos dos carros (em milissegundos)
    }
}

void centerConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    RECT rect;
    GetWindowRect(consoleWindow, &rect);

    int consoleWidth = rect.right - rect.left;
    int consoleHeight = rect.bottom - rect.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX = (screenWidth - consoleWidth) / 2;
    int posY = (screenHeight - consoleHeight) / 2;

    MoveWindow(consoleWindow, posX, posY, consoleWidth, consoleHeight, TRUE);
}

int main() {
    centerConsoleWindow();

    HANDLE threads[2];
    mutex = CreateMutex(NULL, FALSE, NULL); // Criação do mutex

    threads[0] = CreateThread(NULL, 0, trafficLight, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, manageTraffic, NULL, 0, NULL);

    initializeGrid();

    WaitForMultipleObjects(2, threads, TRUE, INFINITE); // Espera pelas threads

    CloseHandle(mutex); // Libera o mutex

    return 0;
}
