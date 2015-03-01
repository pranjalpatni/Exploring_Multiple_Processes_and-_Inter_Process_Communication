#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <time.h>

int main(int argc, char *argv[]) { //Takes 3 command line arguments: <.out file> <.txt file> <timer value>
    pid_t pid; //Process id
    int pipec2m[2], pipem2c[2]; //Creating arrays for pipes
    int address1 = 0, val1 = 0;
    int retpipe[2]; //To check both pipes are created or not
    int address = 0;
    int val = 0;
    int exit = 0;
    int userSP = 0, userPC = 0;
    int timerx = atoi(argv[2]);
    int timeIncrement = timerx;
    int instnum = 0;
    int PC = 0, SP = 1000, IR = 0, AC = 0, X = 0, Y = 0;
    int mode = 0; // mode: 0->user mode 1->system mode
    FILE* file; //File Handling
    char line[100];
    int memory[2000];
    int val2 = 0, index = 0;
    file = fopen(argv[1], "r");
    if (file == NULL) { // Error handling
        printf("Error in reading file");
    } else {
        while (fgets(line, 100, file) != NULL) {
            if (line[0] == '.') {
                if (sscanf(line, ".%d", &val2) == 1) {
                    index = val2;
                }
            } else {
                if (sscanf(line, "%d", &val2) == 1) {
                    memory[index] = val2;
                    index++;
                }
            }
        }
    }
    fclose(file);
    retpipe[0] = pipe(pipec2m); //Creating pipes
    retpipe[1] = pipe(pipem2c);
    if (retpipe[0] == -1 || retpipe[1] == -1) { //Error handling
        perror("Error in pipe");
    } else {
        pid = fork(); //forking
        if (pid < 0) {
            perror("Error in fork");
        }
        if (pid == 0) {
            //********CPU process (child process)*********
            char rw;
            close(pipec2m[0]); // Closing reading side of pipec2m
            close(pipem2c[1]); // Closing writing side of pipem2c 
            for (;;) {
                if (instnum >= timerx && mode == 0) { //For interrupt handling due to timeout
                    mode = 1;
                    timerx = timerx + timeIncrement;
                    rw = 'w';
                    userSP = SP;
                    //printf("userSP: %d\n",userSP);
                    userPC = PC;
                    SP = 2000;
                    PC = 1000;
                    SP--;
                    write(pipec2m[1], &rw, sizeof (char));
                    write(pipec2m[1], &SP, sizeof (int));
                    write(pipec2m[1], &userSP, sizeof (int));
                    SP--;
                    write(pipec2m[1], &rw, sizeof (char));
                    write(pipec2m[1], &SP, sizeof (int));
                    write(pipec2m[1], &userPC, sizeof (int));
                } else { //User mode operation
                    rw = 'r';
                    write(pipec2m[1], &rw, sizeof (char));
                    write(pipec2m[1], &PC, sizeof (int));
                    // printf("PC: %d\n", PC);
                    read(pipem2c[0], &IR, sizeof (int));
                    //  printf("Instruction register: %d\n", IR);
                    if (IR != 30) {
                        PC++;
                    }
                    if (mode == 0) //Increment the counter for counting number of executions in user mode only
                        instnum++;
                    // printf("Increment counter: %d\n",instnum);
                    switch (IR) { //Find the instruction
                        case 1: //Load value into AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &val, sizeof (int));
                            AC = val;
                            break;
                        case 2: //Load value at address into AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            // printf("ADDRESS:%d\n",address);
                            // printf("Mode: %d\n",mode);
                            if (address < 1000 || mode == 1) {
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                read(pipem2c[0], &val, sizeof (int));
                                AC = val;
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 3: //Load value from address found in the address into the AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            if (address < 1000 || mode == 1) {
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                read(pipem2c[0], &address, sizeof (int));
                                if (address < 1000 || mode == 1) {
                                    write(pipec2m[1], &rw, sizeof (char));
                                    write(pipec2m[1], &address, sizeof (int));
                                    read(pipem2c[0], &val, sizeof (int));
                                    AC = val;
                                    //printf("AC: %d",AC);
                                } else {
                                    printf("Memory violation! %d\n", address);
                                    break;
                                }
                            } else {
                                printf("Memory violation! %d\n", address);
                                break;
                            }
                            break;
                        case 4: //Load the value at (address+X) into the AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            address = address + X;
                            if (address < 1000 || mode == 1) {
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                read(pipem2c[0], &val, sizeof (int));
                                AC = val;
                                //printf("AC: %d",AC);
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 5: //Load the value at (address+Y) into the AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            address = address + Y;
                            if (address < 1000 || mode == 1) {
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                read(pipem2c[0], &val, sizeof (int));
                                AC = val;
                                //printf("AC: %d",AC);
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 6: //Load from (Sp+X) into the AC
                            rw = 'r';
                            address = SP + X;
                            if (address < 1000 || mode == 1) {
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                read(pipem2c[0], &val, sizeof (int));
                                AC = val;
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 7: //Store the value in the AC into the address
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            if (address < 1000 || mode == 1) {
                                rw = 'w';
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &address, sizeof (int));
                                write(pipec2m[1], &AC, sizeof (int));
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 8: //Gets a random int from 1 to 100 into the AC
                            srand((unsigned int) time(NULL));
                            AC = rand() % 100 + 1;
                            break;
                        case 9: //If port=1, writes AC as an int to the screen
                            //If port=2, writes AC as a char to the screen
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &val, sizeof (int));
                            if (val == 1) {
                                printf("%d", AC);
                            } else if (val == 2) {
                                printf("%c", AC);
                            }
                            break;
                        case 10: //Add the value in X to the AC
                            AC = AC + X;
                            break;
                        case 11: //Add the value in Y to the AC
                            AC = AC + Y;
                            break;
                        case 12: //Subtract the value in X from the AC
                            AC = AC - X;
                            break;
                        case 13: //Subtract the value in Y from the AC
                            AC = AC - Y;
                            break;
                        case 14: //Copy the value in the AC to X
                            X = AC;
                            break;
                        case 15: //Copy the value in X to the AC
                            AC = X;
                            break;
                        case 16: //Copy the value in the AC to Y
                            Y = AC;
                            break;
                        case 17: //Copy the value in Y to the AC
                            AC = Y;
                            break;
                        case 18: //Copy the value in AC to the SP
                            SP = AC;
                            break;
                        case 19: //Copy the value in SP to the AC 
                            AC = SP;
                            break;
                        case 20: //Jump to the address
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            if (address < 1000 || mode == 1) {
                                PC = address;
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 21: //Jump to the address only if the value in the AC is zero
                            if (AC == 0) {
                                rw = 'r';
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &PC, sizeof (int));
                            }
                            PC = PC + 1;
                            if (AC == 0) {
                                read(pipem2c[0], &address, sizeof (int));
                                if (address < 1000 || mode == 1) {
                                    PC = address;
                                } else {
                                    printf("Memory violation! %d\n", address);
                                    break;
                                }
                            }
                            break;
                        case 22: //Jump to the address only if the value in the AC is not zero
                            if (AC != 0) {
                                rw = 'r';
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &PC, sizeof (int));
                            }
                            PC = PC + 1;
                            if (AC != 0) {
                                read(pipem2c[0], &address, sizeof (int));
                                if (address < 1000 || mode == 1) {
                                    PC = address;
                                } else {
                                    printf("Memory violation! %d\n", address);
                                    break;
                                }
                            }
                            break;
                        case 23: //Push return address onto stack, jump to the address
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &PC, sizeof (int));
                            PC = PC + 1;
                            read(pipem2c[0], &address, sizeof (int));
                            if (address < 1000 || mode == 1) {
                                rw = 'w';
                                SP--;
                                //printf("SP: %d\n",SP);
                                write(pipec2m[1], &rw, sizeof (char));
                                write(pipec2m[1], &SP, sizeof (int));
                                write(pipec2m[1], &PC, sizeof (int));
                                PC = address;
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 24: //Pop return address from the stack, jump to the address
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            read(pipem2c[0], &address, sizeof (int));
                            if (address < 1000 || mode == 1) {
                                SP++;
                                PC = address;
                            } else {
                                printf("Memory violation! %d\n", address);
                            }
                            break;
                        case 25: //Increment the value in X
                            X++;
                            break;
                        case 26: //Decrement the value in X
                            X--;
                            break;
                        case 27: //Push AC onto stack
                            rw = 'w';
                            SP--;
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            // printf("SP pushed************ %d",SP);
                            write(pipec2m[1], &AC, sizeof (int));
                            // printf("AC pushed************ %d",AC);
                            break;
                        case 28: //Pop from stack into AC
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            //printf("SP pop************ %d",SP);
                            read(pipem2c[0], &AC, sizeof (int));
                            //printf("AC popped************ %d",AC);
                            SP++;
                            break;
                        case 29: //Set system mode, switch stack, push SP and PC, set new SP and PC
                            mode = 1;
                            userPC = PC;
                            userSP = SP;
                            SP = 2000;
                            PC = 1500;
                            rw = 'w';
                            SP--;
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            //printf("The system SP during 29 is: %d\n", SP);
                            write(pipec2m[1], &userSP, sizeof (int));
                            //printf("SP pushed: %d\n", userSP);
                            SP--;
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            //printf("The system SP during 29 is: %d\n", SP);
                            write(pipec2m[1], &userPC, sizeof (int));
                            //printf("PC pushed: %d\n", userPC);
                            break;
                        case 30: //Restore registers, set user mode
                            mode = 0;
                            rw = 'r';
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            //printf("The system SP is: %d\n", SP);
                            read(pipem2c[0], &userPC, sizeof (int));
                            //printf("The PC is: %d\n", userPC); /*test*/
                            SP++;
                            write(pipec2m[1], &rw, sizeof (char));
                            write(pipec2m[1], &SP, sizeof (int));
                            //printf("The system SP is: %d\n", SP);
                            read(pipem2c[0], &userSP, sizeof (int));
                            //printf("The SP is: %d\n", userSP);
                            SP++;
                            PC = userPC;
                            SP = userSP;
                            break;
                        case 50: //End execution
                            rw = 'e';
                            write(pipec2m[1], &rw, sizeof (char));
                            exit = 1;
                            break;
                    }
                }
                if (exit == 1) {
                    break;
                }
            }
        } else {
            //**************Memory process (Parent process)************
            char mrw;
            close(pipec2m[1]); //Closing writing side of pipec2m
            close(pipem2c[0]); //Closing reading side of pipem2c
            for (;;) {
                read(pipec2m[0], &mrw, sizeof (char));
                if (mrw == 'w') { //For writing to memory
                    read(pipec2m[0], &address1, sizeof (int));
                    read(pipec2m[0], &val1, sizeof (int));
                    memory[address1] = val1;
                    //printf("*******MEMMORY ADDRESS WRITTEN******%d \n",address1);
                    //printf("*******VALUE AT MEMMORY ADDRESS WRITTEN*****%d \n",val1);
                } else if (mrw == 'r') { //For reading from memory
                    read(pipec2m[0], &address1, sizeof (int));
                    //printf("Address value is: %d\n",address);
                    val1 = memory[address1];
                    write(pipem2c[1], &val1, sizeof (int));
                } else if (mrw == 'e') {
                    break;
                }
            }
        }
    }
}