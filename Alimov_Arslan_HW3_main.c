/**************************************************************
* Class:  CSC-415-02 Fall 2021
* Name:Arslan Alimov
* Student ID:916612104
* GitHub UserID:ArslanAlimov
* Project: Assignment 3 – Simple Shell
*
* File: Alimov_Arslan_HW3_main.c
*
* Description: Creating linux shell that takes users input and executes linux command , we utilize fork() , execwp and wait. As well as working with processes.
*
**************************************************************/
#include <sys/wait.h> 
// to use WifeExited, wait() for the childprocess
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define BLOCK_SIZE 180
//Defining Block Size
#define TOKEN_SEPARATOR " \t\r\n\a\0\e" 
/*
We need to datastruct for our cmd object => where we allocate space for our argument. [Half space of the block size]
*/
typedef struct cmdObj
{
    char *args[BLOCK_SIZE/2]; 
    /*max number of args should be 90 since the max input is 180
    * store in array with block size / 2 
    */
}cmdObj;
struct cmdObj cmd;
/*
*create buffer
*free buffer
*/
char *make_buff()
{
  char *buff=malloc(BLOCK_SIZE);
    return buff;
}
void freeBuffer(char *buffer)
{
    if(buffer)
        free(buffer);
        buffer=NULL;
}

/*
* we can write better code in future to separately get tokens. something like getToken(char*arg[]){} and call it from our cmdline => due to time wa
*/
// Start of the method
void CmdLine(int counter,char *arguments[])
{
    int iterator = 0;
    int childStatus;

    char*buffer = make_buff();
    //allocate buffer with BLOCK_SIZE of 180

    printf("%s ",(counter > 1 ? arguments[1] : ">")); 
    /*
    if(counter>1) 
    {
     printf("%s",arguments[1])
    }else
    { printf(">");}
    Your program must also accept a command line argument which is the prefix prompt. If no value is specified use “> ” as the prompt.
    */

   if(buffer == NULL)
   {
       printf("%s",strerror(errno));
       exit(1);
   }
    /*
    https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
    error handling instead of regular printf("Error")
    */
   
   cmd.args[iterator]= (!fgets(buffer,BLOCK_SIZE,stdin)) ? "exit" : strtok(buffer,TOKEN_SEPARATOR);
    //checks for exit before proceeding to the main function of the code 

    if(cmd.args[iterator]==NULL)
    {
        return;
    }
    //checking if the cmd argument is null 

   if(cmd.args[iterator] == NULL)
   {
       //if the call from terminal or file
    if(!isatty(0))
    {
        printf("\n");
        return;
    }
    /*
    Description about isatty()

    isatty() for test if the call from termial or from file.It prints out right characters depending if the call is from terminal or from file
    if not 0 => then we create a new line \n
    we check for empty line because file and terminal have different line ending in commands.txt 
    Little chart for isatty():
    file descriptors : 0,1,2,3,4
    global file tables ->
    0   -> Read only,offset:0                       |--\ inode table => /dev/pts21
    1,2 -> write-only,offset:0                      |--/ inode table => /dev/pts22
    3   -> read-write,offset:12                     inode table => /path/myfile2.txt
    4   -> read-write, offset:8                     inode table /path/myfile3.txt       
    we need it logically because our input has to be file as well
    */
   }

   if(!strcasecmp(cmd.args[iterator],"exit"))
   {
       freeBuffer(buffer);

       printf("%s",(isatty(0)) ? "" : "\n");
       exit(0);
        /*
        *When we want to exit the terminal it will check whether exit was typed into terminal or it was in the file. isatty will take into consideration whether 
        *the line ends on \n which is from file or "" from terminal
        */
   }

    //continues while there is no null 
    while(cmd.args[iterator]!= NULL)
    {
        cmd.args[++iterator]=strtok(NULL,TOKEN_SEPARATOR);
        //itterater through and add tokens to arguments divided by one of the delimeters that we defined in our main function
        // when there are no tokens left it returns null, using ++iterator because if we do iterator++ we will end up going over the tokens returning the incremented value
        // which will lead to the cmd args being nulled we call a break to get out of this while loop in order to not stay in foreverloop
        
        break;
    }
    
    iterator=0;
    if(!isatty(0))
    {
        while(cmd.args[iterator]!=NULL)
        {
            printf("%s ",cmd.args[iterator++]);
            printf("\n");
        }
    }
    //set iterator back to 0 (Reset) in order to continue 
    pid_t peID=fork();
    /* 
    *peID - for process identification
    *fork() is returning three values (-negative), (positive+),  is warning or failed call and sucecss 0  
    *https://pubs.opengroup.org/onlinepubs/7908799/xsh/fork.html#:~:text=RETURN%20VALUE,set%20to%20indicate%20the%20error.
    */
   if(peID<0)
   {
       puts("[Error] Fork.\n");
       exit(errno);
       /*
       * check child process return value of fork if its less than 0 meaning there is problem.
       * if its negative we return fork error if execvp is negative value 
       */
   }

   if(peID==0)
   {
       /*
       *creating child process we are checking if it was sucessful.
       *we also return a value of execvp which returns an int
       */
      if(execvp(cmd.args[0],cmd.args) < 0)
      {
          puts("Unknown command\n");
          exit(0);
      }
   }else
   {
       freeBuffer(buffer);

       wait(&childStatus);
       /*
       *our parent
       *we free our buffer with my function-which frees and sets buffer to NULL convenient(for every malloc we need to free it)
       *avoid dangling pointers and memory leakage (good practice for future)
       *we call wait ,since it blocks calling of other processes till the child exits the process or till the return is received from it.(for child termination)
       *Getting adress of childStatus to see the status
       * 
       */
      if(WIFEXITED(childStatus))
      {
          /*
          * Checks the status of child and if ended normally then this will be called the status is provided up top in childStatus by wait call
          * childStatus =0 if it exited correctly
          * format of our output 
          * Child #PEID(%d), exited with #(Check on Child) the childstatus should always be 0 since it terminated normally
          */
         printf("Child %d, exited with %d\n",peID,WEXITSTATUS(childStatus));
      }
   }

}
int main(int counter, char *arguments[])
{
    while(1)
    {
        CmdLine(counter,arguments);
    } 
    return 0;
    //while to call the function 
    // we could've done it just in main , but this way the code is better seen and easier to understand each part.
    // we don't really need return 0 since our main logic goes into CmdLine() function
}

/*
All the logic  + Variables are starting from here
Decision of using struct - I used struct to hold cmd args ,because it is a good practice and I am used to it because of OOP - Java,C#.
This is also more organizes this way.
I also decided to store Buffer Size up top instead of just writing an integer in malloc(180), because I can change this integer
in one place instead of multiple.
I then decided to store delimeters in separate define variable , in order for me to use it rather than placing those delimeters/separators
in each of the line in the code.
I created a function to make buffer, because If I need more buffers in future I can just call buffer = makebuff(); and it will create and allocate the buffer for me
I decided to create separate function for freebuffer() in order for to clear any buffer and Null it afterwards by calling one function rather than 
typing free() and then buffer=null; it is all done for us by calling this method.
I created new function which takes in two parameters in order to keep track of arguments and # of arguments to check if its null or actually has lines.
my logic of the cmd line is to check for "exit" first ,because if there is an exit the cmd line should not even work.
I used isatty() to check if the call from terminal or it takes all cmds from file and emulates input from file.
Allocating buffer with malloc rather than creating an array[180] that was the part of assignment.
*/

