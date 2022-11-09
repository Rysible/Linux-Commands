#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER 1048576 // define buffer size

/**
 * Head copies basic principles of Head unix and linux command.
 * Note that many of the functions of the Head command in this program
 * have been omitted because I am lazy and instructions do not ask for them.
 * If you do actually read the comments and don't just check to see if I have
 * good for you.
 * Largely taken from a program my brother and I wrote over the Summer.
 */
int main(int argc, char* args[]) {
    // declare values
     // declare values
    int i, line, lines, byte, bytes;
    bool l, noArg, multipleFiles, nCom
    char buf[BUFFER];
    
    i = 1;
    lines = 0;
    line = 0;
    bytes = 0;
    byte = 0;
    l = true;
    noArg = false;
    multipleFiles = false;
    nCom = false;
  
    if (argc > 2) { // checks if an argument and number are supplied
        if (strcmp(args[i], "-n") == 0) { // if argument is -n
            lines = atoi(args[i+1]); // set line count
            nCom = true;
            if (lines < 1) { // checks to see if there is a line
                setbuf(stdout, NULL);
                printf("ERROR: Line count must be a number greater than 0\n");
                return EXIT_FAILURE;
            } // if
        } else if (strcmp(args[i], "-c") == 0) { // checks if user used -c
            bytes = atoi(args[i+1]); // sets byte count
            byte = bytes;
            nCom = true;
            if (bytes < 1) {
                setbuf(stdout, NULL);
                printf("ERROR: Bytes count must be a number greater than 0\n");
                return EXIT_FAILURE;
            } // if

            l = false; // sets bool value to look for bytes not lines
        } // if
    } // if
    if (nCom) { // if option is selected skips index + 2
        i += 2;
    } else {
        lines = 10; // if no option, lines automatically set to 10
    } // if
    if (i == argc) { // if no argument and no file
        argc++;
        noArg = true;
    } // if
    if (argc - i > 1) { // checks if multiple files are supplied
        multipleFiles = true;
    } // if

    char* file;
    for (int j = i; j < argc; j++) { // loops through files

        if (!noArg) { // sets file if file argument supplied
            file = args[j];
        } // if
        if ((noArg)||(*file == '-')) { // chooses standard input
            //  char b[BUFFER];
            if ((multipleFiles)&&(*file == '-')) { // print if file is '-'
                write(STDOUT_FILENO, "==> ", 4);
                write(STDOUT_FILENO, "standard input", 14);
                write(STDOUT_FILENO, " <==\n", 5);
            } // if
            if (l) { // if line option selected
                int n;
                int linesW = 0;
            out:
                while((linesW < lines)&&((n = read(STDIN_FILENO, buf, BUFFER)) > 0)) {
                    if (n == -1) {
                        perror(file);
                        continue;
                    } // if
                    if (linesW < lines) {
                        int wr = write(STDOUT_FILENO, buf, n); // writes lines
                        if (wr == -1) {
                            perror(file);
                            continue;
                        } // if

                        linesW++; // increments line count
                    } else {
                        goto out;
                    } // if
                } // while

            } else { // if bytes option selected
                int n;
                int toWrite = bytes;
                while ((toWrite > 0)&&((n = read(STDIN_FILENO, buf, BUFFER)) > 0)) { // reads lines
                    if (n == -1) {
                        perror(file);
                        continue;
                    } // if
                    if (n > toWrite) { // ensures not to try and write more bytes than byte count
                        n = toWrite;
                    } // if
                    int wr = write(STDOUT_FILENO, buf, n); // writes lines
                    if (wr == -1) {
                        perror(file);
                        continue;
                    } // if
                    toWrite -= n; // assigns how many more bytes should be written
                } // while
            } // if
            if (j != argc - 1) {
                write(STDOUT_FILENO, "\n", 1); // prints new line
            } // if
        } else {
            int fd = open(file, O_RDONLY); // opens file
            if (fd == -1) {
                perror(file);
                write(1, "\n", 1);
                continue;
            } // if
            int n = read(fd, buf, BUFFER); // reads file
            if (n == -1) {
                perror(file);
                continue;
            } // if
            off_t size = lseek(fd, 0, SEEK_END); // gets size of file in bytes
            if (size == -1) {
                perror(file);
                continue;
            } // if
            if (bytes > size) { // ensures bytes selected dont exceed size of file
                bytes = size;
            } // if
            if (l) { // if lines selected
                for (int i = 0, j = 0; (i < size)&&(j < lines); i++) { // counts amount of newlines
                    if (buf[i] == '\n') {
                        j++;
                    } // if
                    bytes++; // counts bytes
                } // for
            } // if
            if (multipleFiles) { // prints a header if user has multiple files
                setbuf(stdout, NULL);
                printf("==> %s <==\n", file);
            } // if
            int wr = write(STDOUT_FILENO, buf, bytes); // writes first n bytes
            if (wr == -1) {
                perror(file);
                continue;
            } // if
            if (j != argc - 1) {
                write(STDOUT_FILENO, "\n", 1); // prints newline
            } // if
            int cl = close(fd); // close file
            if (cl == -1) {
                perror(file);
                continue;
            } // if
            bytes = byte; // resets byte count for next file
        } // if
    } // for
} // main
