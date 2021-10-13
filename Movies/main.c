
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "movies.c"


struct movie
{
    char* title;
    char* year;
    char* language;
    char* rating;
    struct movie* next;



};


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
*/
struct movie* processFile(char* filePath)
{
    // Open the specified file for reading only
    FILE* movieFile = fopen(filePath,  "r");


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
* Print data for the given movie
*/
void printmovie(struct movie* amovie) {
    printf("%s, %s %s, %s\n", amovie->title,
        amovie->language,
        amovie->year,
        amovie->rating);
}
/*
* Print the linked list of movies
*/
void printmovieList(struct movie* list)
{
    while (list != NULL)
    {
        printmovie(list);
        list = list->next;
    }
}
/*
* Print all movies when given a speccific year
*/
void printSpecificYear(struct movie* list) {
    int year = 0;
    int movieCount = 0;
    
    printf("%s\n", "please specify a year "); scanf("%i", &year);
    if (year > 9999) {
        printf("%s\n", "Year must be a 4 didgit integer ");
        
    }
 
    // traverse linked list
    while (list != NULL)
        // if currmovie year matches specified year: print movie and move on to next node 
        if (atoi(list->year) == year) {
            printmovie(list);
            list = list->next;
            movieCount++; 
        }
        else {
        // cuurmovie doesnt match so move on
            list = list->next;


        }
    if (movieCount == 0) {
        // if our movie count is 0 there is no movies released in that year 
        printf("%s\n", "No movies released in that year.");

    }


}

/*
* bubble sort algorithm for linked list
* I used this function because I thought sorting a the linked list would be the best way to tackle option 2
* Since this sorts by year and there can be multiple repeats of years. I made a modification tto compare years if they are equal 
* it should then sort by ratings
* Source: Geeks for Geeks
* https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/
*/
void bubbleSort(struct movie* list)
{
    int swapped;
    struct movie* ptr1;
    struct movie* lptr = NULL;

    /* Checking for empty list */
    if (list == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = list;

        while (ptr1->next != lptr)
        {
            if (atoi(ptr1->year) > atoi(ptr1->next->year))
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            if (atoi(ptr1->year) == atoi(ptr1->next->year)) {
                if (atof(ptr1->rating) < atof(ptr1->next->rating)) {
                    swap(ptr1, ptr1->next);
                    swapped = 1;

                }

            }

            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}
/*
* swaps two nodes if they are out of order 
* I used this function because its necessary for bubble sort
* IT swapps nodes that are out of order and all their data
* again this is modified in order to swap every piece of the struct movie
* Source: Geeks for Geeks
* https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/
*/
void swap(struct movie* movie1, struct movie* movie2) {
    char* temp = movie1->title;
    movie1->title = movie2->title;
    movie2->title = temp;

    char* temp2 = movie1->year;
    movie1->year = movie2->year;
    movie2->year = temp2;

    char* temp3 = movie1->language;
    movie1->language = movie2->language;
    movie2->language = temp3;

    char* temp4 = movie1->rating;
    movie1->rating = movie2->rating;
    movie2->rating = temp4;

}

/*
* Finds highest year in a linked list
*/
int find_max_year(struct movie* list) {
    int max = 0;
    //traverse list 
    while (list != NULL) {
        //if the nodes year is > max then set a new max
        if (atoi(list->year) > max) {
            max = atoi(list -> year);
        }
        list = list->next;


    }
    //return the highest year
    return max; 
}

/*
* Displays highest rated movie for each year
*/
void showHighestrating(struct movie* list) {

    //call bubble sort
    bubbleSort(list);

    //since data is sorted 1st nodes year will be min year
    int min_year = atoi(list->year);
    int max_year = find_max_year(list);
    int curr_year = min_year;

    // create a temp list and set it equal to the perameter 
    struct movie* temp = list;

    // curr_year is the year we are iterating through  
    while( curr_year <= max_year){
        
        // traverse linked list
        while(list != NULL){
            
            //if the nodes year is equal  to the curr year then its the highest rated movie
            //print it
            if (atoi(list->year) == curr_year) {
                printf("%s %s %.1f \n", list->year, list->title, atof(list->rating));
                
                //break out of loop, so there is no repeats
                break;
            }
            list = list->next;

        }
        curr_year++;
        // set list = temp inorder to iterate again
        list = temp;
    }
}

    

                            


/*
* show movies displayed in acertain language 
*/
void showSpecificLang(struct movie* list) {
    
    //set language string = 200 chars
    char lang[200];
    scanf("%s", lang);
    // for use with strstr
    char* result; 
    
    // check if language is valid: no language should be < 4 characters 
    if (strlen(lang) < 4) {
        printf("%s\n", "Please enter in a valid language ");

    }
    // will keep track of how many movies are in secified language
    int langCount = 0;
    while (list != NULL) {
        // search for specified language within the nodes language: store in result
        result = strstr(list->language, lang);
        if (result) {
            //copy specified language into result        
            if (result == strcpy(result, lang)) {
                printf("%s, %s \n", list ->year,
                                    list->title);
               
                langCount++;
            }
        }   
        
        list = list->next;

    }
    // if langVount is 0 by the end of the list then no movies were releasen in the specified language
    if (langCount == 0) {
        printf( "No data about movies released in %s\n", lang);
        printf("Reminder: the language entered should be capitalized\n");
    }

}
/*
* shows length of a linked list and returns it
*/
int FindLength(struct movie* list) {
    int len = 0;

    //iterate through list and add 1 to length 
    while (list != NULL) {
        len++;
        list = list->next;
    }
    return len;

}
/*
* fuction to repromt user
*/
void repromt() {
   
    
    printf("%s\n", " Enter a choice from 1 to 4");
    printf("%s\n", "1. Show movies release in the specified year");
    printf("%s\n", "2. Show highest rated movie for each year");
    printf("%s\n", "3. Show movies and their year of release for a specific language");
    printf("%s\n", "4. Exit");

}

int main(int argc, char* argv[]) {
   
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movie_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    struct movie* list = processFile(argv[1]);
    printf("Processed file %s", argv[1] );
    printf(" and parsed data for %d movies\n", FindLength(list));
    int choice;

   
   
           repromt();
           scanf("%d", &choice);
           while ( choice != 4  )
               

               if (choice == 1) {
                   printSpecificYear(list);
                   repromt();
                   scanf("%d", &choice);
               }
               else if (choice == 2) {
                   showHighestrating(list);
                   repromt();
                   scanf("%d", &choice);
               }
               else if (choice == 3) {
                   printf("%s\n", "Type in a language that you wish to see a movie in");
                   showSpecificLang(list);
                   repromt();
                   scanf("%d", &choice);
               
               }
               else if (choice == 4) {
                   break;
               }
      
      
    
    return EXIT_SUCCESS;
}
