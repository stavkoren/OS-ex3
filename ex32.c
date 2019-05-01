//Stav Koren 207128539
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <wait.h>
#include <errno.h>


#define ZERO_OPTIONS 0
#define FAILED_VALUE -1
#define NUM_OF_BYTES_TO_READ 1
#define SIZE_OF_BUFFER 2
#define FIRST_CHAR_INDEX 0
#define SECOND_CHAR_INDEX 1
#define END_OF_STR '\0'
#define CONFIG_PATH_INDEX 1
#define END_OF_LINE '\n'
#define END_OF_LINE_STR "\n"
#define MAX_SIZE_OF_PATH 150
#define MAX_SIZE 150
#define POINT "."
#define TWO_POINTS ".."
#define CHAR_POINT '.'
#define CHAR_C 'c'
#define LAST_CHAR_INDEX -1
#define SECOND_LAST_CHAR_INDEX -2
#define INITIALIZE_NUM_OF_STUDENTS 1
#define NO_FILE_FOUND 0
#define COMP_ERROR 20
#define PID_SON 0
#define NAME_OF_COMP_FILE "/studentProg.out"
#define GCC "gcc"
#define FLAG_O "-o"
#define TIME_FOR_RUNNING 5
#define NAME_OF_OUTPUT_FILE "/result.txt"
#define TIMEOUT_SCORE 40
#define SECOND 1
#define LAST_INDEX_IN_ARR -1
#define ERROR "Error in system call\n"
#define SLASH "/"
#define COMP "comp.out"
#define IDENTICAL 1
#define IDENTICAL_GRADE 100
#define SIMILAR 3
#define SIMILAR_GRADE 80
#define DIFFERENT 2
#define DIFFERENT_GRADE 60
#define RESULT_FILE "result.csv"
#define COMMA ","
#define NO_C_FILE "NO_C_FILE"
#define COMPILATION_ERROR "COMPILATION_ERROR"
#define TIMEOUT "TIMEOUT"
#define BAD_OUTPUT "BAD_OUTPUT"
#define SIMILAR_OUTPUT "SIMILAR_OUTPUT"
#define GREAT_JOB "GREAT_JOB"

typedef struct paths{
    char  dirPath[MAX_SIZE_OF_PATH];
    char  inputPath[MAX_SIZE_OF_PATH];
    char  correctOutputPath[MAX_SIZE_OF_PATH];
}paths;
typedef struct student{
    char name[MAX_SIZE_OF_PATH];
    char directory[MAX_SIZE_OF_PATH];
    int grade;
}student;
/**
 * printErrorMessage
 */
void printErrorMessage(){
    write(fileno(stderr),ERROR,strlen(ERROR));
}

/**
 * readPathsFromConfigFile- read all paths from config file
 * @param path
 * @param paths
 * @return paths struct
 */
paths* readPathsFromConfigFile(char*path, paths* path1){
    int first = 1;
    int pathFd;
    int bytesToRead;
    char c[SIZE_OF_BUFFER];
    c[FIRST_CHAR_INDEX]=END_OF_STR;
    pathFd = open(path, O_RDONLY);
    if(pathFd<0){
        printErrorMessage();
        exit(EXIT_FAILURE);
        return;
    }
    c[FIRST_CHAR_INDEX]=END_OF_STR;
    while (c[FIRST_CHAR_INDEX]!=END_OF_LINE){
        bytesToRead=read(pathFd, c, NUM_OF_BYTES_TO_READ);
        if (bytesToRead < 0) {
            printErrorMessage();
 	    exit(EXIT_FAILURE);
        }
        c[SECOND_CHAR_INDEX]=END_OF_STR;
        if(c[FIRST_CHAR_INDEX]!=END_OF_LINE) {
          if(first) { strcpy(path1->dirPath, c); first = 0; }
            else {strcat(path1->dirPath, c); }
        }
    }
    
    c[FIRST_CHAR_INDEX]=END_OF_STR;
    while (c[FIRST_CHAR_INDEX]!=END_OF_LINE){
        bytesToRead=read(pathFd, c, NUM_OF_BYTES_TO_READ);
        if (bytesToRead < 0) {
            printErrorMessage();
 	    exit(EXIT_FAILURE);
        }
        c[SECOND_CHAR_INDEX]=END_OF_STR;
        if(c[FIRST_CHAR_INDEX]!=END_OF_LINE) {
            strcat(path1->inputPath, c);
        }
    }
    c[FIRST_CHAR_INDEX]=END_OF_STR;
    while (c[FIRST_CHAR_INDEX]!=END_OF_LINE){
        bytesToRead=read(pathFd, c, NUM_OF_BYTES_TO_READ);
        if (bytesToRead < 0) {
            printErrorMessage();
 	    exit(EXIT_FAILURE);
        }
        if(c[FIRST_CHAR_INDEX]!=END_OF_LINE) {
            strcat(path1->correctOutputPath, c);
        }
    }
    close(pathFd);
    return path1;
}

/**
 * listdir- search for c file of specific student
 * @param name
 * @param students
 * @param index
 * @return directory to c file if exist or null otherwise
 */
char*listdir(const char *name,student students[],int index)
{
    DIR *dir;
    struct dirent *pDirent;
    //error while tring open
    dir = opendir(name);
    if (dir==NULL){
        printErrorMessage();
        return NULL;
    }
    while ((pDirent = readdir(dir)) != NULL) {
        if (pDirent->d_type == DT_DIR) {
            char path[MAX_SIZE_OF_PATH];
            if (strcmp(pDirent->d_name,POINT) == 0 || strcmp(pDirent->d_name, TWO_POINTS) == 0)
                continue;
          //  snprintf(path, sizeof(path), "%s/%s", name, pDirent->d_name);
          strcpy(path,name);
          strcat(path,SLASH);
          strcat(path,pDirent->d_name);
            listdir(path,students,index);
        } else {
            //its a file
            char *fileName=pDirent->d_name;
            const int len=strlen(fileName);
            //check if the file is .c
            if(len>0&&fileName[len+SECOND_LAST_CHAR_INDEX]==CHAR_POINT&&fileName[len+LAST_CHAR_INDEX]==CHAR_C){
                char path[MAX_SIZE_OF_PATH];
               // snprintf(path, sizeof(path), "%s/%s", name, pDirent->d_name);
                strcpy(path,name);
                strcat(path,SLASH);
                strcat(path,pDirent->d_name);

                strcpy((char*)students[index].directory,path);
                closedir(dir);
                return students[index].directory;
            }
        }
    }
    closedir(dir);
    return NULL;
}
int studentsCount=0;
/**
 * testAllStudents - test c files of all students
 * @param name
 * @param students
 * @return student*
 */
student* testAllStudents(const char *name,student students[])
{
    DIR *dir;
    struct dirent *pDirent;
    //error while tring open
    if (!(dir = opendir(name))){
        return 0;
    }
    while ((pDirent = readdir(dir)) != NULL) {
        if(strcmp(pDirent->d_name,TWO_POINTS)!=0&&
        strcmp(pDirent->d_name,POINT)!=0) {
            //realloc new student
            students=realloc(students,(studentsCount+1)* sizeof(student));
            strcpy(students[studentsCount].name,pDirent->d_name);

            char path[MAX_SIZE_OF_PATH];
            strcpy(path,name);
            strcat(path,SLASH);
            strcat(path,pDirent->d_name);
            listdir(path,students,studentsCount);
            studentsCount++;
        }
    }
    closedir(dir);
    return students;
}

/**
 * compileStudentProg compile c file of specific student
 * @param path
 * @return true if program compiled or false otherwise
 */
bool compileStudentProg(char* path){
    //get current directory
    char cwd[MAX_SIZE_OF_PATH];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd,NAME_OF_COMP_FILE);
    char *argv[]={GCC,FLAG_O,cwd,path,NULL};
    pid_t  pid=fork();

    if (pid ==FAILED_VALUE ) {
        printErrorMessage();
        exit(EXIT_FAILURE);
    }
     if (pid == PID_SON) {
        //child
        if (execvp(GCC, argv) < 0) {
            printErrorMessage();
            exit(EXIT_FAILURE);
        }
    }else{
        int status;
            if((waitpid(pid,&status,ZERO_OPTIONS)==FAILED_VALUE)){
                printErrorMessage();
 	        exit(EXIT_FAILURE);
            }
            if(WEXITSTATUS(status)!=0) {
                return false;
            } else{
                return true;
            }
        }
}

/**
 * executeStudentProgram- execute a.out file
 */
void executeStudentProgram(paths*paths){
    //dir to output file
    char resultFile[MAX_SIZE_OF_PATH];
    getcwd(resultFile, sizeof(resultFile));
    strcat(resultFile,NAME_OF_OUTPUT_FILE);
    //get current directory
    char exeFile[MAX_SIZE_OF_PATH];
    getcwd(exeFile, sizeof(exeFile));
    strcat(exeFile,NAME_OF_COMP_FILE);

    char*argv[]={exeFile,NULL};

    int input= open(paths->inputPath,O_RDONLY);
    if(input==FAILED_VALUE){
        printErrorMessage();
    }
    int output=open(resultFile,O_WRONLY|O_TRUNC|O_CREAT,
            S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
    if(output==FAILED_VALUE){
        close(input);
        printErrorMessage();
    }
    if(dup2(input,STDIN_FILENO)==FAILED_VALUE){
        close(input);
        close(output);
        printErrorMessage();
    }
    if(close(input)==FAILED_VALUE){
        printErrorMessage();
        close(output);
    }
    if(dup2(output,STDOUT_FILENO)==FAILED_VALUE){
        close(output);
        printErrorMessage();
    }
    if(close(output)==FAILED_VALUE){
        printErrorMessage();
    }
    execvp(exeFile,argv);
    exit(EXIT_FAILURE);
}

/**
 * runStudentProgram
 * @param paths
 * @return true if program runs smoothly or false if there was timeout
 */
bool runStudentProgram(paths*paths){
    pid_t  pid=fork();
    if (pid ==FAILED_VALUE ) {
        printErrorMessage();
 	exit(EXIT_FAILURE);
    }
    if (pid == PID_SON) {
        //child
        executeStudentProgram(paths);
    }else{
        //wait for program to run until 5 seconds
        int status;
        int timer=0;
        while (timer<TIME_FOR_RUNNING&&(waitpid(pid,&status,WNOHANG)==0)){
            timer++;
            sleep(SECOND);
        }
        if(timer<TIME_FOR_RUNNING){
            return true;
        }
        return false;
    }
}

/**
 * compareFiles -the function compares files using comp.out
 * @param path
 * @return
 */
int compareFiles(paths* path){
    //get current output file
    char outputFile[MAX_SIZE_OF_PATH];
    getcwd(outputFile, sizeof(outputFile));
    strcat(outputFile,NAME_OF_OUTPUT_FILE);

    char compFile[MAX_SIZE_OF_PATH];
    getcwd(compFile, sizeof(compFile));
    strcat(compFile,SLASH);
    strcat(compFile,COMP);
    char *argv[]={compFile,path->correctOutputPath,outputFile,NULL};

    pid_t  pid=fork();

    if (pid ==FAILED_VALUE ) {
        printErrorMessage();
        exit(EXIT_FAILURE);
    }
    if (pid == PID_SON) {
        //child
        if (execvp(compFile, argv) < 0) {
            printErrorMessage();
             exit(EXIT_FAILURE);
        }
    }else{
        int status;
        if((waitpid(pid,&status,ZERO_OPTIONS)==FAILED_VALUE)){
            printErrorMessage();
 	    exit(EXIT_FAILURE);
        }
        return WEXITSTATUS(status);
    }
}
/**
 * giveGrades
 * @param students
 * @param paths
 * @return update student list
 */
student* giveGrades(student students[],paths*paths){
    int i=0;
    for(i;i<studentsCount;i++){
        if(strlen(students[i].directory)==0){
            students[i].grade=NO_FILE_FOUND;
        }else{
            if(!compileStudentProg(students[i].directory)){
                //compilation error
               students[i].grade=COMP_ERROR;
            }else{
                //run the program
                if(runStudentProgram(paths)==false){
                    //time out error
                    students[i].grade=TIMEOUT_SCORE;
                }else {
                    //compare correct output with current output
                    int result=compareFiles(paths);
                    if (result == IDENTICAL) {
                        students[i].grade = IDENTICAL_GRADE;
                    }
                    if (result == DIFFERENT) {
                        students[i].grade = DIFFERENT_GRADE;
                    }
                    if (result == SIMILAR) {
                        students[i].grade = SIMILAR_GRADE;
                    }
                }
            }
        }
    }
    return students;
}
/**
 * getMsg
 * @param grade
 * @return message according student's grade
 */
char *getMsg(int grade){
    switch (grade){
        case NO_FILE_FOUND:
            return NO_C_FILE;
        case COMP_ERROR:
            return COMPILATION_ERROR;
        case TIMEOUT_SCORE:
            return TIMEOUT;
        case IDENTICAL_GRADE:
            return GREAT_JOB;
        case SIMILAR_GRADE:
            return SIMILAR_OUTPUT;
        case DIFFERENT_GRADE:
            return BAD_OUTPUT;
        default:
            return NULL;
    }

}
/**
 * writeStudentsGrades write students grades into csv file
 * @param students
 */
void writeStudentsGrades(student*students){
    char resultFile[MAX_SIZE_OF_PATH];
    getcwd(resultFile, sizeof(resultFile));
    strcat(resultFile,SLASH);
    strcat(resultFile,RESULT_FILE);
    int resultFd=open(resultFile,O_WRONLY|O_APPEND|O_CREAT,
            S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
    if(resultFd==FAILED_VALUE){
        printErrorMessage();
         exit(EXIT_FAILURE);
    }
    int i=0;
    for(i;i<studentsCount;i++) {
        char line[MAX_SIZE];
        strcpy(line,students[i].name);
        strcat(line,COMMA);
        char gradeStr[MAX_SIZE];
        sprintf(gradeStr,"%d",students[i].grade);
        strcat(line,gradeStr);
        strcat(line,COMMA);
        strcat(line,getMsg(students[i].grade));
	if(i!=studentsCount+LAST_INDEX_IN_ARR){
        strcat(line,END_OF_LINE_STR);
	}
        if(write(resultFd,line,strlen(line))==FAILED_VALUE){
            printErrorMessage();
 	    exit(EXIT_FAILURE);
        }
    }
    close(resultFd);
}

/**
 * freeData
 */
void *freeData(student* students,paths*paths){
    free(paths);
    free(students);
}

/**
 * deleteFiles - clear a.out and result file
 */
void deleteFiles(){
    //get current directory
    char compPath[MAX_SIZE_OF_PATH];
    getcwd(compPath, sizeof(compPath));
    strcat(compPath,NAME_OF_COMP_FILE);
    //dir to output file
    char resultFile[MAX_SIZE_OF_PATH];
    getcwd(resultFile, sizeof(resultFile));
    strcat(resultFile,NAME_OF_OUTPUT_FILE);

    if(unlink(compPath)==FAILED_VALUE){
        printErrorMessage();
    }
    if(unlink(resultFile)==FAILED_VALUE){
        printErrorMessage();
    }

}
int main(int argc, char *argv[]) {
    char * configPath= argv[CONFIG_PATH_INDEX];
    //paths path = {0};
    //*paths=readPathsFromConfigFile(configPath,*paths);
    paths* path = calloc(1, sizeof(paths));
    readPathsFromConfigFile(configPath,path);
    student*students=calloc(INITIALIZE_NUM_OF_STUDENTS, sizeof(student));
    students= testAllStudents(path->dirPath,students);
    students=giveGrades(students,path);
    writeStudentsGrades(students);
    deleteFiles();
    freeData(students,path);
    return 0;
}
