//Stav Koren 207128539
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define READ_FLAG "r"
#define SPACE ' '
#define END_OF_LINE '\n'
#define ASCI_LETTERS_GAP 32
#define FIRST_ARG_POS 1
#define SECOND_ARG_POS 2
#define IDENTICAL 1
#define SIMILAR 3
#define OTHERWISE 2
#define NUM_OF_BYTES_TO_READ 1
#define SIZE_OF_BUFFER 2
#define FIRST_CHAR_INDEX 0
#define ERROR "Error in system call\n"
void printErrorMessage(){
    write(fileno(stderr),ERROR,strlen(ERROR));
}
/**
 * checkIfFilesAreIdentical
 * @param filePath1
 * @param filePath2
 * @return true if files are identical or false otherwise
 */
bool checkIfFilesAreIdentical(char* filePath1, char* filePath2) {
    int file1;
    int file2;
    file1 = open(filePath1, O_RDONLY);
    file2 = open(filePath2, O_RDONLY);
    if (file1 < 0 || file2 < 0) {
        printErrorMessage();
    }
    char c1[SIZE_OF_BUFFER];
    char c2[SIZE_OF_BUFFER];
    int  bytesReaded1, bytesReaded2;

    c1[FIRST_CHAR_INDEX]=0;
    c2[FIRST_CHAR_INDEX]=0;
    // read char by chr until end of file
    do {
        if (c1[FIRST_CHAR_INDEX] != c2[FIRST_CHAR_INDEX]) {
            return false;
        }
        bytesReaded1=read(file1, c1, NUM_OF_BYTES_TO_READ);
        bytesReaded2=read(file2, c2, NUM_OF_BYTES_TO_READ);

        if (bytesReaded1 < 0) {
            printErrorMessage();
        }
        if (bytesReaded2 < 0) {
            printErrorMessage();
        }
    }
    while ((bytesReaded1==NUM_OF_BYTES_TO_READ&&bytesReaded2==NUM_OF_BYTES_TO_READ));
    //if one of files is bigger then the other (reading is not finished yet)
    if((bytesReaded1!=NUM_OF_BYTES_TO_READ&&bytesReaded2==NUM_OF_BYTES_TO_READ))
    {
        bytesReaded2=read(file2, c2, NUM_OF_BYTES_TO_READ);
        if (!(bytesReaded2 <NUM_OF_BYTES_TO_READ)){
            close(file1);
            close(file2);
            return false;
        }
    }
    if (bytesReaded1==NUM_OF_BYTES_TO_READ&&bytesReaded2!=NUM_OF_BYTES_TO_READ){
        bytesReaded1=read(file1, c1, NUM_OF_BYTES_TO_READ);
        if (!(bytesReaded1<NUM_OF_BYTES_TO_READ)){
            close(file1);
            close(file2);
            return false;
        }
    }
    close(file1);
    close(file2);
    return true;
}
/**
 * isBigLetter
 * @param c
 * @return true if its big letter or false otherwise
 */
bool isBigLetter(char c){
    if(c>='A' && c<='Z'){
        return true;
    }
    return false;
}

/**
 * isSmallLetter
 * @param c
 * @return true if its small letter or false otherwise
 */
bool isSmallLetter(char c){
    if(c>='a' && c<='z'){
        return true;
    }
    return false;
}

/**
 * checkIfSameLetter
 * @param c1
 * @param c2
 * @return true if both chars are same latter (like a&A) or false otherwise
 */
bool checkIfSameLetter(char c1, char c2){
    if(c1==c2){
        return true;
    }
    //if chars are not letters return false
    if(((isSmallLetter(c1)||isSmallLetter(c2))&&(isBigLetter(c1)||(isBigLetter(c2))))) {
        if (isSmallLetter(c1) && isBigLetter(c2)) {
            if(c1==c2+ASCI_LETTERS_GAP){
                return true;
            }
        }
        if (isSmallLetter(c2) && isBigLetter(c1)) {
            if(c2==c1+ASCI_LETTERS_GAP){
                return true;
            }
        }
    }
    return false;

}

/**
 * checkIfFilesAreSimilar
 * @param filePath1
 * @param filePath2
 * @return true if files are similar or false otherwise
 */
bool checkIfFilesAreSimilar(char* filePath1, char* filePath2)
{
    int file1;
    int file2;
    file1 = open(filePath1, O_RDONLY);
    file2 = open(filePath2, O_RDONLY);
    if (file1 < 0 || file2 < 0) {
        printErrorMessage();
    }
    char c1[SIZE_OF_BUFFER];
    char c2[SIZE_OF_BUFFER];
    int  bytesReaded1, bytesReaded2;
    c1[FIRST_CHAR_INDEX]=0;
    c2[FIRST_CHAR_INDEX]=0;
    // read char by chr until end of file
    do {
        if (!checkIfSameLetter(c1[FIRST_CHAR_INDEX],c2[FIRST_CHAR_INDEX])) {
            while ((c1[FIRST_CHAR_INDEX] == SPACE || c1[FIRST_CHAR_INDEX] == END_OF_LINE)
                   && bytesReaded1 == NUM_OF_BYTES_TO_READ) {
                //read data until next interesting char
                bytesReaded1=read(file1, c1, NUM_OF_BYTES_TO_READ);
                if (bytesReaded1 < 0) {
                    printErrorMessage();
                }
            }
            while ((c2[FIRST_CHAR_INDEX] == SPACE || c2[FIRST_CHAR_INDEX] == END_OF_LINE)
                   && bytesReaded2 == NUM_OF_BYTES_TO_READ) {
                //read data until next interesting char
                bytesReaded2=read(file2, c2, NUM_OF_BYTES_TO_READ);
                if (bytesReaded2 < 0) {
                    printErrorMessage();
                }
            }
            if ((bytesReaded1==SIZE_OF_BUFFER&&bytesReaded2==SIZE_OF_BUFFER)&&!checkIfSameLetter(c1[FIRST_CHAR_INDEX], c2[FIRST_CHAR_INDEX])) {
                return false;
            }
        }
        bytesReaded1=read(file1, c1, NUM_OF_BYTES_TO_READ);
        bytesReaded2=read(file2, c2, NUM_OF_BYTES_TO_READ);

        if (bytesReaded1 < 0) {
            printErrorMessage();
        }
        if (bytesReaded2 < 0) {
            printErrorMessage();
        }
    }
    while ((bytesReaded1==NUM_OF_BYTES_TO_READ&&bytesReaded2==NUM_OF_BYTES_TO_READ));
    //if one of files is bigger then the other (reading is not finished yet)
    if((bytesReaded1!=NUM_OF_BYTES_TO_READ&&bytesReaded2==NUM_OF_BYTES_TO_READ))
    {
        bytesReaded2=read(file2, c2, NUM_OF_BYTES_TO_READ);
        if (!(bytesReaded2 <NUM_OF_BYTES_TO_READ)){
            close(file1);
            close(file2);
            return false;
        }
    }
    if (bytesReaded1==NUM_OF_BYTES_TO_READ&&bytesReaded2!=NUM_OF_BYTES_TO_READ){
        bytesReaded1=read(file1, c1, NUM_OF_BYTES_TO_READ);
        if (!(bytesReaded1<NUM_OF_BYTES_TO_READ)){
            close(file1);
            close(file2);
            return false;
        }
    }
    return true;
}

int main( int argc, char *argv[] ) {
    if(checkIfFilesAreIdentical(argv[FIRST_ARG_POS],argv[SECOND_ARG_POS])){
        return IDENTICAL;
    }
    if(checkIfFilesAreSimilar(argv[FIRST_ARG_POS],argv[SECOND_ARG_POS])){
        return SIMILAR;
    }
    return OTHERWISE;
}