#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER 1048576 // define buffer size

/**
 * Tail copies basic principles of tail unix and linux command.
 * Note that many of the functions of the tail command in this program
 * have been omitted because I am lazy and instructions do not ask for them.
 * If you do actually read the comments and don't just check to see if I have
 * good for you.
 * Largely taken from a program my brother and I wrote over the Summer.
 */
int main(int argc, char* args[]) {
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

    if (argc > 2) { // checks if argument and number supplied
        if (strcmp(args[i], "-n") == 0) { // if argument is -n
            lines = atoi(args[i+1]); // set line count
            line = lines;
            nCom = true;
            if (lines < 1) { // checks to see if there is a line.
                setbuf(stdout, NULL);
                printf("ERROR: Line count must be a number greater than 0\n");
                return EXIT_FAILURE;
            } // if
        } else if (strcmp(args[i], "-c") == 0) { // if argument is -c
            bytes = atoi(args[i+1]); // sets byte count
            byte = bytes;
            nCom = true;
            if (bytes < 1) { // checks if byte count is above 0
                setbuf(stdout, NULL);
                printf("ERROR: Bytes count must be a number greater than 0\n");
                return EXIT_FAILURE;
            } // if
            l = false; // sets bool value to check for bytes not lines
        } // if
    } // if
    if (nCom) { // if n command is used skips index + 2
        i += 2;
    } else {
        lines = 10; // if no n command, lines automatically set to 10
        line = 10;
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
        int fd;
        if (!noArg) { // sets file if argument supplied
            file = args[j];
        } // if
        if ((noArg)||(*file == '-')) { // chooses standard input
            if ((multipleFiles)&&(*file == '-')) { // print if file is '-'
                write(STDOUT_FILENO, "==> ", 4);
                write(STDOUT_FILENO, "standard input", 14);
                write(STDOUT_FILENO, " <==\n", 5);
            } // if
            fd = creat("-", 0644); // creates file to save stdin to
            if (fd == -1) {
                perror("-");
                continue;
            } // if
            int rede;
            while ((rede = read(STDIN_FILENO, buf, BUFFER)) > 0) { // reads stdin
                int wr;
                wr = write(fd, buf, rede); // writes stdin to file
                if (wr == -1) {
                    perror("-");
                    continue;
                } // if
            } // while
            file = "-";
        } // if
        fd = open(file, O_RDONLY); // opens file
        if (fd == -1) {
            perror(file);
            continue;
        } // if
        int re = read(fd, buf, BUFFER); // reads file to buffer
        if (re == -1) {
            perror(file);
            continue;
        } // if
        off_t size = lseek(fd, 0, SEEK_END); // gets size of file
        if (size == -1) {
            perror(file);
            continue;
        } // if
        if (bytes > size) { // ensures bytes does not exceed size
            bytes = size;
        } // if
        if (l) { // if lines finds bytes
            int total = 0;
            int fileBytes = 0;
            for (int i = 0; i < size; i++) { // counts total lines
                if (buf[i] == '\n') {
                    total++;
                } // if
            } // for
            if (lines > total) { // ensures lines does not exceed total lines
                lines = total;
            } // if
            // counts bytes until last n lines reached
            for (int i = 0, j = 0; (i < size)&&(j < total - lines); i++) {
                if (buf[i] == '\n') {
                    j++;
                } // if
                fileBytes++;
            } // for
            bytes = size - fileBytes;
        } // if

        off_t ls = lseek(fd, (-1 * bytes), SEEK_END); // seeks to before last n lines
        if (ls == -1) {
            perror(file);
            continue;
        } // if
        int rede = read(fd, buf, BUFFER); // reads bytes
        if (rede == -1) {
            perror(file);
            continue;
        } // if
        if ((multipleFiles)&&(*file != '-')) { // prints if multiple files
            setbuf(stdout, NULL);
            printf("==> %s <==\n", file);
        } // if

        int wri = write(STDOUT_FILENO, buf, bytes); // writes to stdout
        if (wri == -1) {
            perror(file);
            continue;
        } // if
        if (j != argc - 1) { // prints new lines except last file
            write(STDOUT_FILENO, "\n", 1);
        } // if
        int cl = close(fd); // closes file
        if (cl == -1) {
            perror(file);
            continue;
        } // if
        bytes = byte;
        lines = line; // resets line and byte count
        remove("-"); // removes temporary file
    } // for
} // main
