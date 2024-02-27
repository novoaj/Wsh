#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_ARGS 64
#define MAX_ARG_LENGTH 256 // changed from 64
#define DEFAULT_HISTORY_SIZE 5

char **history = NULL;
int history_count = 0;
int history_size = 0;
// function prototypes
char **wshSplitLine(char *line);
void removeNewline(char* str);
int wshExecute(char **args, char *line);
void wshLoop(void);
char* translateVariable(char* key);
int isVariable(char *s);
int launchProcessLoop(char **commands, int n);


// Node type will help us store our local vars
typedef struct Node
{
    char* data;
    char* key;
    // int count;
    struct Node *next;
} Node;
struct Node *head = NULL;
struct Node *current = NULL;
// prints linked list items
void printList(){
    struct Node *cur = head;
    while (cur != NULL) {
        printf("%s=%s\n", cur->key, cur->data);
        cur = cur->next;
    }
}
// removes from LL if node exists
void removeNode(char* key){
    if (head == NULL) {
        return;
    }
    if (strcmp(head->key, key) == 0) {
        struct Node *temp = head;
        head = head->next;
        free(temp->key);
        free(temp->data);
        free(temp);
        return;
    }
    struct Node *prev = head;
    struct Node *cur = head->next;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            prev->next = cur->next;
            free(cur->key);
            free(cur->data);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
// insert is upsert (update if exists)
void addNode(char* data, char* key) {
    // printf("addingNode...%s:%s\n", key, data);
    // go through list, if find duplicate (key = key) this means update data
    // if no key exists, add to end of LL
    struct Node *cur = (struct Node*) malloc(sizeof(struct Node));
    cur->key = strdup(key);
    cur->data = strdup(data);
    cur->next = NULL;

    if (head == NULL){
        head = cur;
        return;
    }

    struct Node *prev = NULL;
    struct Node *linkedList = head;
    while (linkedList != NULL){ // case where we need to update node key = key
        if (strcmp(linkedList->key, key) == 0){
            free(linkedList->data);
            linkedList->data = strdup(data);
            free(cur->key);
            free(cur->data);
            free(cur);
            return;
        }
        prev = linkedList;
        linkedList = linkedList->next;
    }
    prev->next = cur; // key doesn't exist in list
    return;
}
// returns the value associated with this key in our linked list or "" if none exists (adds new key with "" as val)
char *getNode(char* key){
    // try to retrieve with key, if cant retrieve add node with key, data = ""
    struct Node *linkedList = head;
    while (linkedList != NULL) {
        if (strcmp(linkedList->key, key) == 0){
            return linkedList->data;
        }
        linkedList = linkedList->next;
    }
    // addNode("", key);
    return "";
}
// frees all the nodes of our linked list.
void freeLinkedList(){
    struct Node *cur = head;
    while (cur != NULL){
        struct Node *temp = cur;
        cur = cur->next;
        free(temp->key);
        free(temp->data);
        free(temp);
    }
    head = NULL;
}
// gets the i'th command from history and returns a pointer to it
char* getCommand(int i){
    return history[i];
}
int isNum(char* s){
    int i = 0;
    while (i < strlen(s)){
        if (s[i] >= '0' && s[i] <= '9'){
            i++;
        }else{
            return 0;
        }
    }
    return 1;
}
// helper method that adds to/maintains our q of previouis commands (history)
void add_to_history(char* command){
    
    if (history == NULL) {
        history_size = DEFAULT_HISTORY_SIZE;
        history = malloc(history_size * sizeof(char*));
        if (history == NULL) {
            printf("mem allocation failed in add_to_history\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < history_size; i++){
            history[i] = NULL; // get rid of garbage vals
        }
    }
    if (history_size == 0) {
        return;
    }
    if (history_count == history_size){ // q is full, we are no longer interested in the item at history[history_size-1]
        free(history[history_size-1]);
        history_count--;
    }
    // shift all commands to the right to make space at idx 0 for new command
    for (int i =history_size - 1; i > 0; i--){
        history[i] = history[i-1];
    }
    history[0] = strdup(command); // append to list
    if (history[0] == NULL) {
        printf("Memory allocation failed for history command\n");
        exit(EXIT_FAILURE);
    }
    history_count++;    
    
}
// helper method to resize history
// TODO handle case where newSize = 0, history should be cleared
void resizeHistory(int newSize) {
    if (newSize < 0){
        // printf("error invalid history size. must be > 0!\n");
        return;
    }
    if (newSize == 0){
        for (int i = 0; i < history_count; i++){
            free(history[i]);
        }
        // free(history);
        history_count = 0;
        history_size = 0;
        return;
    }
    // new place for us to store our char pointers
    char **newHistory = malloc(newSize * sizeof(char*));
    if (newHistory == NULL){
        printf("mem allocation failed when trying to increase history size\n");
        exit(EXIT_FAILURE);
    } 
    for (int i = 0; i < newSize; i++) {
        newHistory[i] = NULL; // get rid of garbage vals
    }
    int numElements = (newSize < history_count) ? newSize : history_count;
    for (int i = 0; i < numElements; i++){
        newHistory[i] = history[i];
        // free(history[i]);
        // copy i'th pointer
    }
    if (history_size != 0){
        free(history);
    }
    history = newHistory; // reassign history to point to newHistory
    history_size = newSize;
    if (newSize < history_count) { //resize if necessary
        history_count = newSize;
    }
}
// helper method that prints commands in history
void printHistory() {
    if (history_count == 0) {
        // printf("nothing in history!\n");
        return;
    }
    for (int i = 0; i < history_count; i++){
        printf("%d) %s\n", i+1, history[i]);
    }
}
// reset variables associated with history and free all memory history array is using
void cleanHistory(){
    for (int i = 0; i < history_count; i++){
        free(history[i]);
        // history[i] = NULL;
    }
    history_count = 0;
    history_size = DEFAULT_HISTORY_SIZE;
}
int launchProcess(char** args){
    int pid; // , wpid;
    // inside launch processs maybe we should copy args and mutate all variables (strs that start with '$')
    
    char** argsWithoutVars = malloc(MAX_ARGS * sizeof(char *));
    if (argsWithoutVars == NULL) {
        exit(EXIT_FAILURE);
    }
    int i = 0;
    for (i = 0; args[i] != NULL; i++){
        if (isVariable(args[i])){
            char *variableName = strdup(args[i] + 1);
            if (variableName == NULL) {
                exit(EXIT_FAILURE);
            }
            
            char *translatedValue = translateVariable(variableName); // translate goes into ll to search - free this at some point?
            //printf("translated value: %s\n", translatedValue);
            free(variableName);
            if (translatedValue == NULL) {
                exit(EXIT_FAILURE);
            }
            argsWithoutVars[i] = strdup(translatedValue);
            free(translatedValue);
            //printf("translated value: %s\n", translatedValue);
        }else{
            argsWithoutVars[i] = strdup(args[i]);
            if (argsWithoutVars[i] == NULL) {
                exit(EXIT_FAILURE);
            }
        }
    }
    argsWithoutVars[i] = NULL;
    int j = 0;
    for (int k = 0; argsWithoutVars[k] != NULL; k++){
        if (strcmp(argsWithoutVars[k], "") != 0){
            argsWithoutVars[j] = argsWithoutVars[k];
            j++;
        }else{
            free(argsWithoutVars[k]);
        }
    }
    argsWithoutVars[j] = NULL;
    
    int status;

    pid = fork();
    if (pid == 0){
        // child
        if (execvp(argsWithoutVars[0], argsWithoutVars) == -1){
            printf("execvp: No such file or directory\n");
            exit(EXIT_FAILURE);
            // return 0; // return 0 on failure
            // printf("something went wrong running your command!\n");
        }
        exit(EXIT_FAILURE); // dont know if this is correct behavior
    }else if (pid < 0){
        printf("error spawning child");
    }else{
        waitpid(pid, &status, WUNTRACED);
    }
    // cleanup
    for (int i = 0; argsWithoutVars[i] != NULL; i++){
        free(argsWithoutVars[i]);
    }
    free(argsWithoutVars);
    if (status == 0){
        return 1;
    }
    return 0;
}
// executes history Command depending on user input/args
int historyCommand(char **args){
    //  cases: 
    // history (no args) = printHistory
    if (args[1] == NULL){
        printHistory();
        return 1;
    }else if(strcmp(args[1], "set") == 0) { // history set <n> = resizeHistory
        if (args[2] != NULL){
            // do something with it (check that it is a number)
            if (isNum(args[2])){
                int resize = atoi(args[2]);
                resizeHistory(resize);
                return 1;
            }
        }
    }else{ //history ___ (blank must be a number)
        // history <n> = get and execute the n'th command
        if (isNum(args[1])) {
            int nth = atoi(args[1]);
            if (nth <= history_count && nth > 0){
                // history is 0 indexed as array but 1th element should be 0th of arr
                char* cmd = getCommand(nth-1);

                char* originalCommand = strdup(cmd); //trailing newline here
                removeNewline(originalCommand);

                char** cmd_args = wshSplitLine(originalCommand); // this is mutating original string?
                
                int success = launchProcess(cmd_args);

                free(originalCommand); // free pointer

                if (success) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// returns the string associated with the key or an empty string if no variable is found
char* translateVariable(char* key){
    // first try to retrieve from local, then env
    char *envResult = getenv(key);
    // returns ptr or NULL
    if (envResult == NULL){
        char* result = NULL; // check local
        result = getNode(key);
        return strdup(result);
    }else{
        return strdup(envResult);
    }
}
// export command
int exportCommand(char **args) {
    if (args[1] == NULL) {
        return 0;
    }
    char *token = strtok(args[1], "=");
    if (token == NULL) {
        return 0;
    }
    char *key = strdup(token);
    token = strtok(NULL, "=");
    if (token == NULL){ // case where data is null
        // set and unset return 0 on sucess and -1 on err
        unsetenv(key);
        free(key);
        return 1;
    }
    char *data = strdup(token);
    setenv(key, data, 1);
    return 1;
    // add key:val pair to environ
}
// method that will parse the str "xyz=abc" and add it into out linked list
int assignLocalVariable(char *s) {
    // need to split s on equal sign, take chars before '=' as key, and after as value.
    char *token = strtok(s, "=");
    if (token == NULL) {
        return 0;
    }
    char *key = strdup(token);
    token = strtok(NULL, "=");
    if (token == NULL){ // case where data is null
        //printf("data is blank");
        removeNode(key);
        free(key);
        return 1;
    }
    // TODO change logic, data can be blank str. in this case, we remove from our LL using key
    char* data = strdup(token);
    addNode(data, key);  // add to LL
    return 1;
   
}
// checks whether a string is a variable (starts with '$')
int isVariable(char *s) {
    return (s[0] == '$');
}
// deals with args that starting with "local ... "
int localCommand(char** args) {
    if (args[1] == NULL){
        return 0;
    }else if (args[2] != NULL) {
        return 0;    
    }
    return assignLocalVariable(args[1]);
}
int wshExit(){
    // printf("exiting\n\n");
    exit(0);
}
int wshCd(char **args){
    if (args[2] != NULL) { // we only care about > 1 arg figure out that case
        printf("error in cd: too many args for cd command\n");
        return 1;
    }else{
        if (chdir(args[1]) != 0){
            printf("error changing directory\n");
        }
    }
    return 1;
}
// removes leading and trailing whitespace from char* str
// chatgpt helped me write this function for trimming whitespace at the start and end of a char*
char* strTrim(char *str){
    while (isspace((char)*str)) str++;
    if (*str == 0) return str; // end of str?
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((char)*end)) end--;
    end[1] = '\0';
    return str;
}
int wshExecute(char **args, char *line) {
    if (args[0] == NULL) {
        return 1;
    }
    if (strcmp(args[0], "cd") == 0){
        wshCd(args);
    }else if (strcmp(args[0], "exit") == 0){ // "it is an error to pass any args to exit"
        wshExit();
        // return 0;
    }else if (strcmp(args[0], "history") == 0) {
        historyCommand(args);
    }else if (strcmp(args[0], "local") == 0) {
        localCommand(args);
    }else if (strcmp(args[0],"vars") == 0) {
        // print out linked list of variables
        printList();
    }else if (strcmp(args[0], "export") == 0) {
        // do something here
        exportCommand(args);
    }else{
        char **commands = malloc(MAX_ARGS * sizeof(char*)); // check if malloc fails?
        char *command = strdup(line);
        // commands should be an array of commands
        // split command by | 
        char* token = NULL;
        int n = 0;
        while ((token = strsep(&command, "|")) != NULL){ // TODO get rid of spaces for our split commands
            commands[n++] = strTrim(token); // consider case where blank space beteween two pipes?
        }
        if (n > 1){
            launchProcessLoop(commands, n);
            // free(commands);
        }else{
            // normal execvp command
            int success = launchProcess(args); // something to think about is when a piped comand is in history
            if (success) {
                add_to_history(line);
            }
        }
        free(command);
        free(commands);
    } 
    return 1;
}
// This method is for handling commands with pipes '|' characters to chain together commands
// https://www.geeksforgeeks.org/pipe-system-call/
int launchProcessLoop(char **commands, int n) {
    // loop through commands, forking off processes
    // need to use dup2
    int pipes[n][2];
    int pipe_idx = 0;
    for (pipe_idx = 0; pipe_idx < n; pipe_idx++){
        pipe(pipes[pipe_idx]); // init read/write for our pipes in our 2d pipes array
    }
    int pids[n]; // will contain our pids
    for (int i = 0; i < n; i++){
        // execute cmd one by one
        int pid;
        if ((pid = fork()) < 0) {
            printf("error spawning child process\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0){
            // child process, should execute ith command
            if (i > 0){
                dup2(pipes[i-1][0], STDIN_FILENO); // reassigning child stdin to pipes[command][read] for second command onward
                // prev commands' "out" becomes this childs' "in"
            }
            if (i + 1 < n){
                dup2(pipes[i][1], STDOUT_FILENO); // reassigning stdout for all except last command
                // this childs' out becomes i+1th pipes in 
            }
            int j = 0;
            for (j = 0; j < n; j++){ // closing r/w pipes in the child
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            // we want commands to maybe be an array of args for this step
            char *cmd = strdup(commands[i]);
            char *args[MAX_ARGS];
            int arg_idx = 0;
            char *token = strtok(cmd, " ");
            
            while (token != NULL) {
                args[arg_idx++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_idx] = NULL;

            if (args == NULL){
                printf("error with mem allocation in launchProcessLoop\n");
                exit(EXIT_FAILURE);
            }
            if (execvp(args[0], args) == -1){ // we want first arg to be first word of commands[i]
                printf("\n\nerror, execvp failed\n");
                exit(EXIT_FAILURE);
            }
        }else{
            pids[i] = pid; // parent adds child process to arr of pids
        }
    }
    for (int i = 0; i < n; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < n; i++){
        waitpid(pids[i], NULL, WUNTRACED);
    }

    return 1;
}
// this function is responsible for parsing the string/line passed into it
char **wshSplitLine(char *line){
    int bufsize = MAX_ARGS;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *)); 
    char *token;
    char* delim = " \t\n\r"; // split string by these delims

    if (!tokens) {
        perror("wsh: allocation error\n");
        exit(EXIT_FAILURE); // exit if we are unable to allocate memory successfully
    }
    // use strsep to itereate through line, use delimiters " " (don't consider tabs, newline?)
    while ((token = strsep(&line, delim)) != NULL){
        
        if (strcmp(token, " ") == 0 || strcmp(token, "\n") == 0 || strcmp(token, "\r") == 0 || *token == '\0'){ // dont add new lines/spaces as tokens in our args array
            continue;
        }
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*)); // resizing buffer to fit more args
            if (!tokens) {
                printf("wsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    tokens[position] = NULL;
    return tokens;
}
// reads the line, takes no args as input, will read user's input from stdout
// returns a string/char* that is the user's input
char *wshReadLine(void){
    // retrieves user input from stdout
    char *line = NULL;
    size_t bufsize = 0;

    // man pages: ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) { // we have reached end of file we are reading or user typed ctrl-D
            exit(EXIT_SUCCESS); // exit(0)
        } else {
            printf("wshreadline"); // prints to stderr
            exit(EXIT_FAILURE); // exit(1)
        }
    }
    return line;
}
// removes trailing new line character and replaces with null terminator
void removeNewline(char* str){
    int len = strlen(str);
    if (len>0 && str[len-1] == '\n'){
        str[len-1] = '\0';
    }
}
// Loops indefinitely and reads/processes user inputs
void wshLoop(void){
    int condition = 1;
    char *line;
    char **args;
    // we want to implement piping, only needs to work for non built ins
    while (condition){
        printf("wsh> ");
        line = wshReadLine();
        char* originalCommand = strdup(line); //trailing newline here
        removeNewline(originalCommand);
        args = wshSplitLine(line); // split line mutates original "line" str which we want to store in history potentially
        condition = wshExecute(args, originalCommand);

        free(line);
        free(args);
        free(originalCommand);
    }
    
}
// batch mode, process commands line by line
//https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
void batchMode(char* file){
    // open file, read line by line
    FILE *fp;
    char *line;
    char **args;
    size_t len = 0;
    size_t read;
    fp = fopen(file, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1){
        // line should now hold the command (line is a char*)
        char* originalCommand = strdup(line); //trailing newline here
        removeNewline(originalCommand);
        args = wshSplitLine(line); // split line mutate original "line" str which we want to store in history potentially
        if (wshExecute(args, originalCommand) != 1) {
            break;
        }
    }
    // cleanup
    fclose(fp);
}
// tester method for my history add, resize, and clean methods
void testHistory() {
    add_to_history("ls");
    add_to_history("cd ..");
    add_to_history("gcc -o program program.c");
    add_to_history("echo Hello, World!");
    add_to_history("pwd");
    
    printf("history at length 5 (before resize): \n");
    printHistory();

    int newSize = 10;
    printf("\nresizing history to %d...\n", newSize);
    resizeHistory(newSize);

    printf("history after resizing: \n");
    printHistory();

    add_to_history("some command");
    add_to_history("another command!");

    printf("history after resize and adding more commands: \n");
    printHistory();

    resizeHistory(0);
    printf("history after resizing to 0:\n");
    printHistory();

    resizeHistory(3);
    add_to_history("ls");
    add_to_history("cd ..");
    printf("history after resizing from 0 to 3 and adding two commands\n");
    printHistory();

    printf("cleaning history...\n");
    cleanHistory();

    printf("history cleaned: ");
    printHistory();
}
// test LinkedList implementation (dupe handling, etc...)
void testLinkedList(){
    addNode("value1", "key1");
    addNode("value2", "key2");
    addNode("value3", "key3");

    printf("List after adding 3 nodes: ");
    printList();

    printf("\nvalue for key 'key3': %s\n", getNode("key3"));

    addNode("updated_value", "key1");
    printf("List after upsert: ");
    printList();

    freeLinkedList();
    printf("\nList after freeing: ");
    printList();
    printf("\n");
}

int main(int argc, char *argv[]){
    if (argc > 1){
        // batch mode, argv[1] should be the name of a file
        batchMode(argv[1]);
    }else{
        wshLoop();
    }
    cleanHistory();
    freeLinkedList();
    return -1;
}