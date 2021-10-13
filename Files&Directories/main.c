#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define PREFIX "movies_"
#define suffix ".csv"
struct movie
{
	char* title;
	char* year;
	char* language;
	char* rating;
	struct movie* next;

};

/*
* this code is from the week 1 repl.it
*/
struct movie* createmovie(char* currLine)
{
	struct movie* currmovie = malloc(sizeof(struct movie));

	// For use with strtok_r
	char* saveptr;

	// The first token is the title
	char* token = strtok_r(currLine, ",", &saveptr);


	currmovie->title = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currmovie->title, token);

	// The next token is the year
	token = strtok_r(NULL, ",", &saveptr);
	currmovie->year = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currmovie->year, token);

	// The next token is the language
	token = strtok_r(NULL, ",", &saveptr);
	currmovie->language = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currmovie->language, token);

	// The last token is the rating
	token = strtok_r(NULL, "\n", &saveptr);
	currmovie->rating = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currmovie->rating, token);

	// Set the next node to NULL in the newly created movie entry
	currmovie->next = NULL;

	return currmovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
* this code is from the week 1 repl.it
*/
struct movie* processFile(char* filePath)
{
	// Open the specified file for reading only
	FILE* movieFile = fopen(filePath, "r");


	char* currLine = NULL;

	size_t len = 0;
	size_t nread;
	// char* token;

	 // The head of the linked list
	struct movie* head = NULL;
	// The tail of the linked list
	struct movie* tail = NULL;
	getline(&currLine, &len, movieFile);
	// Read the file line by line
	while ((nread = getline(&currLine, &len, movieFile)) != -1)
	{
		// Get a new movie node corresponding to the current line
		struct movie* newNode = createmovie(currLine);

		// Is this the first node in the linked list?
		if (head == NULL)
		{
			// This is the first node in the linked link
			// Set the head and the tail to this node
			head = newNode;
			tail = newNode;
		}
		else
		{
			// This is not the first node.
			// Add this node to the list and advance the tail
			tail->next = newNode;
			tail = newNode;
		}
	}
	free(currLine);
	fclose(movieFile);
	return head;
}

/*
* Starts the program and prompts the user
* 
*/

void  initialprompt() {
	printf("1. Select a file to process\n");
	printf("2. Exit\n");
	printf("Enter in a choice 1 or 2: \n");
}
/*
* Promts the user if they choose option 1
*
*/
void optionsPrompt() {
	printf("Which file do you want to process?\n");
	printf("Enter 1 to pick the largest file\n");
	printf("Enter 2 to pick the smallest file\n");
	printf("Enter 3 to specify the name of the file\n");
}
/*
* finds the  largest file 
* returns the name of the largest entry
* Part of this code was from the Exploration:directories. However, it was modified 
*/
 char* findLargestFile() {
	 DIR* currDir = opendir(".");
	 struct dirent* aDir;
	 struct stat dirStat;
	 char entryName[256];
	 // stores the file fize in bytes 
	 off_t max_size = 0 ;
	 //loop goes through the diretory 
	 while ((aDir = readdir(currDir)) != NULL) {
		 // if the directory name matches the prefix(movies_) and the suffix(.csv) 
		 if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX))  == 0) {
			 if (strstr( aDir->d_name, suffix)){
				 // Get meta-data for the current entry
				 stat(aDir->d_name, &dirStat);

				 // if size of the file is > max size
				 if (dirStat.st_size >= max_size) {
					 //set new max to the files size
					 max_size = dirStat.st_size;
					 // set the memory
					 memset(entryName, '\0', sizeof(entryName));
					 //copy the name of the file into entryName 
					 strcpy(entryName, aDir->d_name);


				 }
			 }

		 }
	 }
	 //close the current directory
	 closedir(currDir);
	 //returns the name of the entry
	 return entryName;
 }
 /*
* finds the smallest  file
* returns the name of the smallest entry
*  Part of this code was from the Exploration:directories. However, it was modified 
*/
 char* findSmallestFile() {
	 DIR* currDir = opendir(".");
	 struct dirent* aDir;
	 struct stat dirStat;
	 char entryName[256];
	 // set the minimum size to the size of the current file 
	 off_t min_size = dirStat.st_size;
	 //loop goes through the diretory 

	 while ((aDir = readdir(currDir)) != NULL) {

		 if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
			 if (strstr(aDir->d_name, suffix)) {
				 // Get meta-data for the current entry
				 stat(aDir->d_name, &dirStat);				 
				 //if the files size is <= to the min size 
				 if (  dirStat.st_size <= min_size ) {
					 //sets a new minimum
					 min_size = dirStat.st_size;
					 //se the memory 
					 memset(entryName, '\0', sizeof(entryName));
					 //copy the name of the file into entryName 
					 strcpy(entryName, aDir->d_name);
				 }
			 }

		 }
	 }
	 //close the current directory
	 closedir(currDir);
	 
	 return entryName;

 }
 /*
* fuction for processing option 3
* returns 1 if the file is found and 0 if it doesn't exist 
*/
int specifyFile(char* fileName) {
	 
	 // keeps track of whether or not we found the file specified 
	 int found = 0;
	
	 DIR* currDir = opendir(".");
	 struct dirent* aDir;
	
	 //loop goes through the diretory 
	 while ((aDir = readdir(currDir)) != NULL) {
		 //check if our file name matches 
		 if (strncmp(fileName, aDir->d_name, strlen(fileName)) == 0) {
			 printf("Now processing the chosen file named %s \n", fileName);
			 found = 1;
		 }		 
	 }
	 // if found is equal to 0 then out file was not found
	 if (found == 0  ) {
		 printf("The file \"%s\" was not found.Try again\n", fileName);
		 
	 }
	 // close directory
	 closedir(currDir);
	 return found; 
 }
/*
* fuction for creating a directory
* returns the name of the directory created
*/
char* createDir() {
	//seed our random number generator
	time_t t;
	srand((unsigned)time(&t));
	// will store whether or not we created the directory 
	int check ;
	// num will be a number between 0-9999, both numbers inclusive 
	int num = rand() % 10000;
	// allocate some memory on the heap for the directory name 
	char* dirName = malloc(sizeof(char));
	// formats the name of the directory that will be created
	sprintf(dirName, "garibcri.movies.%d",num);
	// make a directory; permissions: owner has read, write, and execute. while group has read and execute
	check = mkdir(dirName, 0750);
	if (!check) {
		printf("Created directory with the name %s\n", dirName );
	}
	
	return dirName;
	

}
/*
* fuction for creating text files 
* param1: list of the processed csv file
* param2: Name of the directory where the text files will be placed into
* Part of this code was from the Exploration:permissions. However, it was modified create text files given a c.sv
*/
void createTxt(struct movie* list, char* dirName) {
	//will store whether a file was created 
	int fd;
	char filepath[256];
		//traverse the linked list 
		while (list != NULL) {
			//format the file path ex: garibcri.movies.3426/2014.txt 
			sprintf(filepath, "%s/%s.txt",dirName ,list->year);		
			//open the file with read and write permissions for the owner. The group can only read.
			//if the pathname does not exist it will create a file 
			fd = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0640);
			// if open returns -1 the file was not created
			if (fd == -1) {
				printf("open() failed on \"%s\"\n", filepath);
				perror("error");
				exit(1);
				
			}
			//create an array to store the title of our current movie 
			char title[256] ;
			//format the title 
			sprintf(title, "%s\n", list->title);
			//write the title into the file 
			write(fd, title, strlen(title) );
			close(fd);
			list = list->next;
	}
		
}

/*
* starts the program and processes user input 
*/
void start() {
	initialprompt();
	int userChoice = 0;
	scanf("%d", &userChoice);
	//will store the return value of the specifyFile function
	int result = 0;
	// reprompts user until user choice is 2(exit)
	while (userChoice != 2) {
		if (userChoice == 1) {
			optionsPrompt();
			scanf("%d", &userChoice);
			//user chose findLargestFile
			if (userChoice == 1)
			{
				printf("The largest file/directory starting with the prefix \"%s\"  is %s\n", PREFIX, findLargestFile());
				printf("Now processing the chosen file named %s\n", findLargestFile());
				//process the file
				struct movie* list = processFile(findLargestFile());
				//create the .txt and the directory
				createTxt(list, createDir());
				break;
			}
			//user chose findSmallestFile
			else if (userChoice == 2) {
				printf("The smallest file/directory starting with the prefix \"%s\"  is %s\n", PREFIX, findSmallestFile()); ;
				printf("Now processing the chosen file named %s\n", findSmallestFile());
				struct movie* list = processFile(findSmallestFile()); 
				//create the .txt and the directory
				createTxt(list, createDir() );
				break;
			}
			//user chose to specify a file name
			else if (userChoice == 3) {	
				printf("Enter the complete file name: \n");
				char* fileName = malloc(sizeof(char));;
				scanf("%s", fileName);
				 result = specifyFile(fileName);
				 //if the specified file exists create the dirrectory
				if (result == 1) {
					
					struct movie* list = processFile(fileName);
					createTxt(list, createDir());
					free(fileName);
					break;
				}

				//file doesnt exist, so reprompt the user 
				else {
					start();
					break;
				}
			}

		}
		//exit the program
		else if (userChoice == 2) {
			break;

		}
		else {
			start();
		}

	}
}
int main() {
	start();
	
			
	return EXIT_SUCCESS;
}