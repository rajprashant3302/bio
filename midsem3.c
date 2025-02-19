#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LENGTH 1000
#define MAX_HISTORY 10

struct termios original;

// Enable raw input mode
void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &original);
    raw = original;
    raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Disable raw input mode
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

// Stack to maintain history of text states
char history[MAX_HISTORY][MAX_LENGTH];
int history_size = 0;
char text[MAX_LENGTH] = "";

// Save current state to history
void saveToHistory() {
    if (history_size < MAX_HISTORY) {
        strcpy(history[history_size], text);
        history_size++;
    } else {
        // Shift history left to discard the oldest entry
        for (int i = 1; i < MAX_HISTORY; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[MAX_HISTORY - 1], text);
    }
}

// Undo last change
void undo() {
    if (history_size > 0) {
        history_size--;
        strcpy(text, history[history_size]);
    }
}

// Refresh screen
void refreshScreen() {
    printf("\r%s ", text);
    fflush(stdout);
}

int main() {
    enableRawMode();
    printf("Simple Text Editor (Press Ctrl-C to exit, Ctrl-Z to undo)\n");
    refreshScreen();
    
    int len = 0;
    char ch;
    while (1) {
        ch = getchar();
        
        if (ch == 3) { // Ctrl-C to exit
            break;
        } else if (ch == 26) { // Ctrl-Z to undo
            undo();
        } else if (ch == 127) { // Backspace
            if (len > 0) {
                saveToHistory();
                text[--len] = '\0';
            }
        } else if (len < MAX_LENGTH - 1) {
            saveToHistory();
            text[len++] = ch;
            text[len] = '\0';
        }
        refreshScreen();
    }
    
    disableRawMode();
    printf("\nExiting Text Editor.\n");
    return 0;
}
