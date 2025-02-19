#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TAG_LENGTH 100
#define MAX_STACK_SIZE 100

// Stack structure for storing tags
typedef struct {
    char *tags[MAX_STACK_SIZE];
    int top;
} Stack;

// Function to initialize the stack
void initStack(Stack *s) {
    s->top = -1;
}

// Function to check if the stack is empty
int isEmpty(Stack *s) {
    return s->top == -1;
}

// Function to push a tag onto the stack
void push(Stack *s, const char *tag) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->tags[++s->top] = strdup(tag);
    } else {
        printf("Stack Overflow! Too many nested tags.\n");
        exit(1);
    }
}

// Function to pop a tag from the stack
char *pop(Stack *s) {
    if (!isEmpty(s)) {
        return s->tags[s->top--];
    }
    return NULL;
}

// Function to check the top element of the stack
char *peek(Stack *s) {
    if (!isEmpty(s)) {
        return s->tags[s->top];
    }
    return NULL;
}

// Function to validate XML-like tags
int validateXML(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        return 0;
    }

    Stack stack;
    initStack(&stack);

    char ch, tag[MAX_TAG_LENGTH];
    int i, isClosingTag;
    
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '<') {
            i = 0;
            isClosingTag = 0;
            ch = fgetc(file);
            
            if (ch == '/') {  // Closing tag
                isClosingTag = 1;
                ch = fgetc(file);
            }
            
            while (ch != '>' && ch != EOF && i < MAX_TAG_LENGTH - 1) {
                tag[i++] = ch;
                ch = fgetc(file);
            }
            tag[i] = '\0'; // Null terminate tag name
            
            if (isClosingTag) {
                char *topTag = pop(&stack);
                if (!topTag || strcmp(topTag, tag) != 0) {
                    printf("Mismatched or missing closing tag: </%s>\n", tag);
                    fclose(file);
                    return 0;
                }
                free(topTag);
            } else {
                push(&stack, tag);
            }
        }
    }

    if (!isEmpty(&stack)) {
        printf("Unclosed tag: <%s>\n", pop(&stack));
        fclose(file);
        return 0;
    }

    fclose(file);
    printf("XML is well-formed!\n");
    return 1;
}

// Main function
int main() {
    char filename[100];
    printf("Enter the XML file name: ");
    scanf("%s", filename);
    
    if (validateXML(filename)) {
        printf("The XML structure is valid.\n");
    } else {
        printf("The XML structure is invalid.\n");
    }
    
    return 0;
}