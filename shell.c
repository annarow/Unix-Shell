#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <assert.h>
#include <fcntl.h>

/*
 * Project 1 - UNIX SHELL
 * By: Anna Rowena Waldron
 * Date: January 20, 2022
 * This program behaves like a unix shell. It takes in user input and runs them as if on a shell through a shell interface.
 */
#define MAX_LINE 80 /* The maximum length first_args */

int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* first_args line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    char commands[MAX_LINE + 1];
    char previous_command[MAX_LINE + 1];    //history of previous command entered
    strcpy(previous_command, "");

    while (should_run) { //runs the program until user types exit
        printf("osh>");
        scanf(" %[^\n]", commands);  //% means to look for special case, bracket with ^\n says dont stop scanning till see newline character
        fflush(stdout);


        for (int i = 0; i < MAX_LINE / 2 + 1; i++) //
        {
            args[i] = '\0';
        }
        //if a command like ls -l | less  is typed this is what it would look like ['l', 's', ' ', '-', 'l', '', '|', ' ', 'l', 'e', 's', 's', '\0']

        if (strcasecmp(commands, "exit") == 0) //if equal returns 0 if not returns 1 so must reverse to work in if
        {
            should_run = 0;
            continue;
        }
        if (strcasecmp(commands, "!!") == 0) {
            if (strcasecmp(previous_command, "") == 0) {
                printf("No commands in history\n");
                continue;
            }

            strcpy(commands, previous_command); //previous_command is copied onto first_args as a string copy
            printf("osh>%s\n",
                   commands);  //%s looks in the argument list in printf and substitutes the string for it. %d for int
        }

        strcpy(previous_command, commands); //first_args saved into history

        int token_length = 1;
        args[0] = strtok(commands, " ");

        while (args[token_length] = strtok(NULL, " ")) //counts the number of commands
        {
            token_length++;
        }

        int special_command = 0;    //0 means nothing special
        int special_command_index = 0;
        //following for lopp checks for special characters and the index in the list of the special character
        for (int i = 0; i < token_length; i++) {
            if (strcmp(args[i], ";") == 0) {
                special_command = 1;
                special_command_index = i;
                break;
            }
            if (strcmp(args[i], "&") == 0) {
                special_command = 2;
                special_command_index = i;
                break;
            }
            if (strcmp(args[i], "|") == 0) {
                special_command = 3;
                special_command_index = i;
                break;
            }
            if (strcmp(args[i], "<") == 0) {
                special_command = 4;
                special_command_index = i;
                break;
            }
            if (strcmp(args[i], ">") == 0) {
                special_command = 5;
                special_command_index = i;
                break;
            }
        }
        //following if and if else and else statements deals with forks and executing commands with special characters like a pipe
        //the following code is used if there was a ; symbol in the list of commands
        if (special_command == 1) //for ; symbol
        {
            char *first_args[MAX_LINE / 2 + 1];
            char *second_args[MAX_LINE / 2 + 1];

            for (int i = 0; i < MAX_LINE / 2 + 1; i++) //
            {
                first_args[i] = '\0';
                second_args[i] = '\0';
            }

            for (int i = 0; i < token_length; i++) {
                if (i < special_command_index) {
                    first_args[i] = args[i];
                } else if (i > special_command_index) {
                    second_args[i - (special_command_index + 1)] = args[i];
                }
            }
            //the above code gets ready the commands on either side of the special first_args character
            int fork_return_value = fork();
            assert(fork_return_value != -1); //-1 means came back with error, 0 good
            if (fork_return_value == 0) //child goes here to execute
            {
                int execute_command_value = execvp(first_args[0], first_args);   //executes left side of special char index
                assert(execute_command_value >= 0);
                return 0;
            } else {
                int status;
                wait(&status);
            }
            printf("\n");

            fork_return_value = fork();
            assert(fork_return_value != -1); //-1 means came back with error, 0 good
            if (fork_return_value == 0) //child goes here to execute
            {
                int execute_command_value = execvp(second_args[0], second_args);   //executes the commands to the right of special char index
                assert(execute_command_value >= 0);
                return 0;
            } else {
                int status;
                wait(&status);
            }
        }
        //the following code is used if there was a & symbol in the list of commands
        else if (special_command == 2) //for & symbol
        {
            char *first_args[MAX_LINE / 2 + 1];
            char *second_args[MAX_LINE / 2 + 1];

            for (int i = 0; i < MAX_LINE / 2 + 1; i++) //
            {
                first_args[i] = '\0';
                second_args[i] = '\0';
            }
            //following for loop fills two new lists of the left and right side of commands from the special character index
            for (int i = 0; i < token_length; i++) {
                if (i < special_command_index) {
                    first_args[i] = args[i];
                } else if (i > special_command_index) {
                    second_args[i - (special_command_index + 1)] = args[i];
                }
            }
            //the above code gets ready the commands on either side of the special first_args character
            int fork_return_value = fork();
            assert(fork_return_value != -1); //-1 means came back with error, 0 good
            if (fork_return_value == 0) //child goes here to execute
            {
                int execute_command_value = execvp(first_args[0], first_args);   //maybe change so saves to file and pass between child and parent processes?
                assert(execute_command_value >= 0);
                return 0;
            } else {
                int fork_return_value_two = fork(); //second fork for the parent so only one parent
                assert(fork_return_value_two != -1);
                if(fork_return_value_two == 0){     //Second baby child
                    int execute_command_value_two = execvp(second_args[0], second_args);
                    assert(execute_command_value_two >= 0);
                    return 0;
                } else {
                    int status2;
                    wait(&status2);
                }
                int status;
                wait(&status);
            }

        }
        //the following code is used if there was a | symbol in the list of commands
        else if (special_command == 3) //for | symbol
        {
            char *first_args[MAX_LINE / 2 + 1]; //left side commands from the special character index
            char *second_args[MAX_LINE / 2 + 1];    //right side commands from the special character index

            for (int i = 0; i < MAX_LINE / 2 + 1; i++) //clears junk data
            {
                first_args[i] = '\0';
                second_args[i] = '\0';
            }

            for (int i = 0; i < token_length; i++)
            {
                if (i < special_command_index) {
                    first_args[i] = args[i];
                } else if (i > special_command_index) {
                    second_args[i - (special_command_index + 1)] = args[i];
                }
            }
            //the above code gets ready the commands on either side of the special first_args character
            int the_pipe[2];
            enum {RD, WR};

            pipe(the_pipe);

            int fork_return_value = fork();
            assert(fork_return_value != -1);
            if (fork_return_value == 0) // first child of parent
            {
                dup2(the_pipe[WR], STDOUT_FILENO);
                close(the_pipe[RD]);        //for permissions for the pipe
                close(the_pipe[WR]);
                int execute_command_value = execvp(first_args[0], first_args); //executes left side commands
                assert(execute_command_value >= 0);
                return 0;
            } else {    //parent
                int fork_return_value_two = fork();
                assert(fork_return_value_two != -1);
                if(fork_return_value_two == 0){ //second child of parents
                    dup2(the_pipe[RD], STDIN_FILENO);
                    close(the_pipe[RD]);
                    close(the_pipe[WR]);
                    int execute_command_value_two = execvp(second_args[0], second_args); //executes right side commands
                    assert(execute_command_value_two >= 0);
                    return 0;
                } else {
                    close(the_pipe[RD]);
                    close(the_pipe[WR]);
                    wait(NULL);
                }
                wait(NULL);
            }

        }
        //the following code is used if there was a < symbol in the list of commands
        else if (special_command == 4) //for < symbol
        {
            char *shell_commands[MAX_LINE / 2 + 1];
            char *file_source[MAX_LINE / 2 + 1];

            for (int i = 0; i < MAX_LINE / 2 + 1; i++) //
            {
                shell_commands[i] = '\0';
                file_source[i] = '\0';
            }

            for (int i = 0; i < token_length; i++) {
                if (i < special_command_index) {
                    shell_commands[i] = args[i];
                } else if (i > special_command_index) {
                    file_source[i - (special_command_index + 1)] = args[i];
                    shell_commands[i - 1] = args[i];   //file name added to args
                }
            }

            int fork_return_value = fork();
            assert(fork_return_value != -1);

            if(fork_return_value == 0){
                int fd = open(file_source[0], O_RDONLY);
                assert(fd != -1);

                dup2(0, fd); //Redirects input to a file
                close(fd);

                int execute_command_value = execvp(shell_commands[0], shell_commands);
                assert(execute_command_value >= 0);
                return 0;
            } else {
                int status;
                wait(&status);
                printf("\n");       //used to formatting
            }
        }
        //the following code is used if there was a > symbol in the list of commands
        else if (special_command == 5) //for > symbol
        {
            char *first_args[MAX_LINE / 2 + 1];
            char *second_args[MAX_LINE / 2 + 1];

            for (int i = 0; i < MAX_LINE / 2 + 1; i++) //
            {
                first_args[i] = '\0';
                second_args[i] = '\0';
            }

            for (int i = 0; i < token_length; i++) {
                if (i < special_command_index) {
                    first_args[i] = args[i];
                } else if (i > special_command_index) {
                    second_args[i - (special_command_index + 1)] = args[i];
                }
            }

            int fork_return_value = fork();
            assert(fork_return_value != -1);

            if(fork_return_value == 0){ //child
                int fd = open(second_args[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                assert(fd != -1);
                                                //redirects the output of a command to a file
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);

                int execute_command_value = execvp(first_args[0], first_args);
                assert(execute_command_value >= 0);
                return 0;
            } else {    //parent
                int status;
                wait(&status);
            }
        }

        else { //No special symbol
            //the following code is for if there is no special characters in the commands
            int fork_return_value = fork();
            assert(fork_return_value != -1);    //-1 means came back with error, 0 good
            if (fork_return_value == 0) //child goes here to execute
            {
                int execute_command_value = execvp(args[0], args);  //executes all the commands
                assert(execute_command_value >= 0);
                return 0;
            } else {
                int status;
                wait(&status);
            }
        }



        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless first_args included &
        */
        //be aware of ; & | < > they will need if statements
    }
    return 0;
}
