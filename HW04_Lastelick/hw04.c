// Trevor Lastelick
// COSC 4348 W01 - Systems Programming
// Assignment 4

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 255
#endif
#define _POSIX_C_SOURCE 200809L

int count = 0;
int dirCount = 0;
int fileCount = 0;
long totalSizeBytes = 0;

struct fileInfo {

int bytes;
char path[256];
char destPath[256];
unsigned long mode;
bool aboveMedianFlag;
};

struct dirInfo {

char destPath[256];

};

void copyLowerMedian(struct fileInfo *file, int fileCount, int median);
void makeAllDirectories(struct dirInfo *dir, int dirCount);
void traverseDirectory(long bytesArray[], char path[], struct fileInfo *file, struct dirInfo *dir, char destinationPath[]);
bool isEven(int number);
void selectionSort(long bytesArray[], int fileCount);
long findMedian(long bytesArray[], int fileCount);
int openFileForRW(char * file);
int truncateFile(char * file);
void copyFile(int sourceFileFd, int destinationFileFd);
pid_t r_wait(int *stat_loc);

#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
char buffer[512];

int main(int argc, char *argv[])
{

//int fileCount = 0;
long bytesArray[10];
long totalBytes;

long median = 0;

char path[256];
char rootPath[256];
char rootDestination[256];

//strcpy(path, argv[1]);

if (argc != 3)
	{
		fprintf(stderr, "Usage: %s source_directory destination_directory\n", argv[0]);
		return 1;
	}
 
    // Initialize directory and file holders.
    struct fileInfo file[50];
    struct dirInfo dir[50];

    // Paths
    strcpy(path, argv[1]);
    strcpy(rootPath, argv[1]);
    strcpy(rootDestination, argv[2]);
    
    // Get file structure.
    traverseDirectory(bytesArray, path, file, dir, rootDestination);
    selectionSort(bytesArray, fileCount);
    median = findMedian(bytesArray, fileCount);
    
    printf("The total size of the files in this directory: %ld\n", totalSizeBytes);
    printf("This is the median: %ld\n", median); 
    
    // Print all file and directory paths. 
    for (int i=0; i<fileCount; i++)
    {
      // Flag if file bytes is above the median.
      if (file[i].bytes >= median)
        file[i].aboveMedianFlag = true;
        
      else
        file[i].aboveMedianFlag = false;
    } 
    
    // Create all files and directories. 
    makeAllDirectories(dir, dirCount);
    copyHigherMedian(file, fileCount, median);
    copyLowerMedian(file, fileCount, median);
    
    
    //                                             //
    // CODE REQUIRED TO HAVE THREADS SPLIT UP WORK //
    //                                             //
        
    // Have threads split up the work.
   /* 
    // Create 2 children threads.
    for (int i = 0; i < 2; i++)
		{  if ((childpid = fork()) <= 0)      // Ensures parent is only creator of children. 
        
        if (i == 0)
          copyHighDirectory;
          
        if ( i == 1)
          copyLowDirectory;    
    } 
    
    int pid1_res = waitpid(pid1, NULL, 0);
    printf("Waited for %d\n", pid1_res);
    int pid2_res = waitpid(pid2, NULL, 0);
    printf("Waited for %d\n", pid2_res);
    
    
    // Parent must wait for all of its children before execution. 
   	while (r_wait(NULL) > 0); 
  */
  
 return 0;
}

void copyLowerMedian(struct fileInfo *file, int fileCount, int median)
{
int sourceFd, destFd = 0;
int error;

    // Step through every file.
    for (int i=0; i<fileCount; i++)
      {
        // If below the median then copy file. 
        if (file[i].aboveMedianFlag == false)
            {
            sourceFd = openFileForRW(file[i].path);
            destFd = truncateFile(file[i].destPath);
            copyFile(sourceFd, destFd);
            error = chmod(file[i].destPath, file[i].mode);  // Create proper file permissions.
              if (error != 0)
                perror("chmod"); 
            }
      }
}

void copyHigherMedian(struct fileInfo *file, int fileCount, int median)
{

int sourceFd, destFd = 0;
int error;

    // Step through every file.
    for (int i=0; i<fileCount; i++)
      {
        // If above the median then copy file. 
        if (file[i].aboveMedianFlag == true)
            {
            sourceFd = openFileForRW(file[i].path);
            destFd = truncateFile(file[i].destPath);
            copyFile(sourceFd, destFd);
            error = chmod(file[i].destPath, file[i].mode);  // Create proper file permissions.
              if (error != 0)
                perror("chmod"); 
            }
      }
}

void makeAllDirectories(struct dirInfo *dir, int dirCount)
{
  
  errno = 0;
  int ret = 0;
  for (int i = 0; i < dirCount; i++)
  {
    // Make directory. 
    ret = mkdir(dir[i].destPath, S_IRWXU);
    
    // If folder already exists, continue on. 
    if (ret = -1)
    {
      switch (errno) {
        case EEXIST:
          continue;
    
        default:
          perror("mkdir");
          exit(0);
      }
    }
  
  }
}


void traverseDirectory(long bytesArray[], char path[], struct fileInfo *file, struct dirInfo *dir, char destinationPath[])
{
struct dirent *direntp;
DIR *dirp;
char tempPath[256];

struct stat buf;


    int sourceFd;
    //int destFd; 

strcpy(tempPath, path);


if (( dirp = opendir ( path )) == NULL )
{
perror (" Failed to open directory " );
return 1;
}

while (( direntp = readdir ( dirp )) != NULL )
{
    // Create proper path to file/directory. 
    strcpy(tempPath, path);
    strcat(tempPath, "/"); 
    strcat(tempPath, direntp->d_name);                
    
    // Check lstat.     
		if (lstat(tempPath, &buf) < 0)
		{
			perror("stat");
      exit(EXIT_FAILURE);
		}
    
    // Get file stats.      
    else
    {   
      lstat(tempPath, &buf);

      // If File. 
      if (S_ISREG(buf.st_mode))
        {
        
          // Create destination path.
          strcpy(file[fileCount].destPath, destinationPath);
          strcat(file[fileCount].destPath, "/");
          strcat(file[fileCount].destPath, direntp->d_name);
          //printf("destPath inside if file: %s\n", file[fileCount].destPath);
          
          // Create source path.
          strcpy(file[fileCount].path, tempPath);
          
          // Get median of all file byte, and accumulation of total bytes. 
          bytesArray[fileCount] = buf.st_size;
          file[fileCount].mode = buf.st_mode;
          totalSizeBytes += buf.st_size;
          
          fileCount++;      
        }
        
      // If directory. 
      if (S_ISDIR(buf.st_mode))
			  {
          //Create destination path.
          strcpy(dir[dirCount].destPath, destinationPath);
          strcat(dir[dirCount].destPath, "/");
          strcat(dir[dirCount].destPath, direntp->d_name);
          dirCount++;   
        }

    } // end stat else 
    strcpy(tempPath, "");
    
} // end stepping  
  
while (( closedir ( dirp ) == -1) && ( errno == EINTR )) ;

return 0;
}

// Returns true if number is even, false otherwise. 
bool isEven(int number)
{
  bool flag;

  if (number % 2 == 0)
    flag = true;
  else
    flag = false;
    
  return flag;
}

// Sort an array of long numbers.
// Result: Array shortest to biggest numbers.
void selectionSort(long bytesArray[], int fileCount)
{
  int counter1,counter2,minimum,tempValue;


  for(counter1=0;counter1<fileCount-1;counter1++)
  {
    minimum=counter1;
    
    for(counter2=counter1+1;counter2<fileCount;counter2++)
    {
      if(bytesArray[minimum]>bytesArray[counter2])
      minimum=counter2;
    }
    
    if(minimum!=counter1)
    {
      tempValue=bytesArray[counter1];
      bytesArray[counter1]=bytesArray[minimum];
      bytesArray[minimum]=tempValue;
    }
  } 
}

long findMedian(long bytesArray[], int fileCount)
{
  long median; 
  
    if (isEven(fileCount))
      median = (bytesArray[(fileCount/2)-1] + bytesArray[(fileCount/2)]) /2; 
    
    else
      median = bytesArray[(fileCount/2)]; 
      
    return median;
}

// Open a file for reading and writing. 
// @param file, The name of the file to open.
int openFileForRW(char * file)
{

int fd;

fd = open(file, O_RDWR);
if (fd == -1)
    {
    perror("Error opening source file");
    exit(0);
    }

return fd;
}

// If file does not exist then create it
// or if it does exist then open it for reading and writing and truncate it. 
// @param file, The name of the file to open.
int truncateFile(char * file)
{

int fd;

fd = open(file, CREATE_FLAGS, CREATE_MODE);
if (fd == -1)
  {
  perror("Error opening destination file");
  exit(0);
  }

return fd;
}

// Identically copy one file to another.
// Precondition: Both files must exist and be opened. 
// @param sourceFile, The file to be copied from.
// @param destinationFile, The file to copy to.
void copyFile(int sourceFileFd, int destinationFileFd)
{
  // Copy the file n bytes at a time.
  while(read(sourceFileFd, &buffer, 1) != 0)
    write(destinationFileFd, &buffer, 1);
}

// Tells the parent to wait for all of it's children before the parent can execute. 
// @param int *stat_loc, can be NULL value to achieve the description above. 
// @return retval, either -1 or the value of a child process id. 

pid_t r_wait(int *stat_loc)
{
  int retval;
  while (((retval = wait(stat_loc)) == -1) && (errno == EINTR));  // errno == EINTR, close function was interrupted by a signal. 
  return retval;
}

// SOURCES

// Various lecture slides and examples.  

// https://www.youtube.com/watch?v=kCGaRdArSnA&t=1s

// https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-wait-wait-child-process-end#rtwai

// https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-waitpid-wait-specific-child-process-end

// Wait_Ex1_Slide29.

// https://stackoverflow.com/questions/5486774/keeping-fileowner-and-permissions-after-copying-file-in-c

// https://www.delftstack.com/howto/c/mkdir-in-c/#:~:text=Use%20the%20mkdir%20Function%20to%20Create%20a%20New,can%20be%20used%20to%20create%20a%20new%20directory.

// https://www.youtube.com/watch?time_continue=300&v=Yp4koUOoV3U&feature=emb_logo

// http://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html

// https://linux.die.net/man/2/lstat

// https://www.systutorials.com/how-to-get-the-size-of-a-file-in-c/#:~:text=How%20to%20get%20a%20file%E2%80%99s%20size%20in%20C%3F,proceed%20with%20allocating%20memory%20and%20reading%20the%20file

// https://www.youtube.com/watch?v=FT2A2HQbTkU

// https://www.educative.io/blog/concatenate-string-c

// https://www.educba.com/sorting-in-c/

// https://stackoverflow.com/questions/8107826/proper-way-to-empty-a-c-string

// https://www.geeksforgeeks.org/umask-command-in-linux-with-examples/

// https://codingpointer.com/c-tutorial/median#:~:text=%20C%20Program%20To%20Calculate%20Median%20This%20c,the%20median%20value%20from%20the%20sorted%20array%20elements.

// https://www.gnu.org/software/libc/manual/html_node/Scanning-Directory-Content.html

// https://stackoverflow.com/questions/42404729/functionality-of-alphasort

// https://www.tutorialspoint.com/c_standard_library/c_function_free.htm#:~:text=C%20library%20function%20-%20free%20%28%29%201%20Description.,does%20not%20return%20any%20value.%205%20Example.%20