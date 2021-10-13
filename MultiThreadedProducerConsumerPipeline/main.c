
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define term  "STOP\n"
#define plus "++"

char inputs[1000];


/*---------------------------------------------------------------*/

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_1 = PTHREAD_COND_INITIALIZER;

char* buffer_1[1000];
int count_1 = 0;
int pd_idx_1 = 0;
int full_1 = 0;

void put1(char* put_line_1) {
	pthread_mutex_lock(&mutex_1); // lock mutex
	buffer_1[pd_idx_1] = put_line_1; // put line into buffer
	pd_idx_1++; // increment put down
	count_1++; // increment number of lines still in buff
	pthread_cond_signal(&cond_1); // singal buff not empty
	pthread_mutex_unlock(&mutex_1); // unlock mutex
}
char* get1() {
	pthread_mutex_lock(&mutex_1); // lock mutex
	while (count_1 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond_1, &mutex_1);
	}
	char* get_line_1 = buffer_1[full_1]; // get line
	full_1++; // increment pick up
	count_1--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_1); // unlock the mutex

	return get_line_1;
}

/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_2 = PTHREAD_COND_INITIALIZER;

char* buffer_2[1000];
int count_2 = 0;
int pd_idx_2 = 0;
int full_2 = 0;


void put2(char* put_line_2) {
	pthread_mutex_lock(&mutex_2); // lock mutex
	buffer_2[pd_idx_2] = put_line_2; // put line into buffer
	pd_idx_2++; // increment put down
	count_2++; // increment number of lines still in buff
	pthread_cond_signal(&cond_2); // singal buff not empty
	pthread_mutex_unlock(&mutex_2); // unlock mutex
}
char* get2() {
	pthread_mutex_lock(&mutex_2); // lock mutex
	while (count_2 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond_2, &mutex_2);
	}
	char* get_line_2 = buffer_2[full_2]; // get line
	full_2++; // increment pick up
	count_2--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_2); // unlock the mutex

	return get_line_2;
}

/*---------------------------------------------------------------*/


/*---------------------------------------------------------------*/

pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_3 = PTHREAD_COND_INITIALIZER;

char* buffer_3[1000];
int count_3 = 0;
int pd_idx_3 = 0;
int full_3 = 0;


void put3(char* put_line_3) {
	pthread_mutex_lock(&mutex_3); // lock mutex
	buffer_3[pd_idx_3] = put_line_3; // put line into buffer
	pd_idx_3++; // increment put down
	count_3++; // increment number of lines still in buff
	pthread_cond_signal(&cond_3); // singal buff not empty
	pthread_mutex_unlock(&mutex_3); // unlock mutex
}
char* get3() {
	pthread_mutex_lock(&mutex_3); // lock mutex
	while (count_3 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond_3, &mutex_3);
	}
	char* get_line_3 = buffer_3[full_3]; // get line
	full_3++; // increment pick up
	count_3--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_3); // unlock the mutex

	return get_line_3;
}


/*---------------------------------------------------------------*/


void* get_input() {
	while (1) {
		size_t len = 0;
		char* curr_line = NULL;
		getline(&curr_line, &len, stdin); // gets a line of user input
		put1(curr_line); // put the curr line into out thread buffer
		if (strcmp(curr_line, term) == 0) {//if we see STOP then dont read any more lines
			break;
		}
	}
	return NULL;
}


void* line_seperator() {
	while (1) {
		char* temp = get1(); // get the contents of the thread buffer and strore them in temp
		int len = strlen(temp);
		if (strcmp(temp, term) == 0) {//if we see STOP then dont read any more lines
			printf("line: |%s|\n", temp);
			put2(temp);
			printf("line2: |%s|\n", temp);
			break;
		}
		for (int i = 0; i < len; i++) // loop through temp
		{
			if (temp[i] == '\n') { // if there is a newline character 
				temp[i] = ' '; // replace witha  space
			}
		}
		put2(temp); // put out new Array into the next threads buffer
	}
	return NULL;
}
void* plus_sign() {

	while (1) {
		char* temp = get2(); // get the buffer
		char before[1000]; // stores string before the plus
		char after[1000]; // stores string after plus
		int len = strlen(temp);
		if (strcmp(temp, term) == 0) {//if we see STOP then dont read any more lines
			put3(temp);
			break;
		}
		for (int i = 0; i < len; i++)
		{
			if (temp[i] == '+' && temp[i + 1] == '+') { //comepare for ++
				for (int j = 0; j < i; j++) //loop upto the index where we found the first +
				{
					before[j] = temp[j];// array has everything before +

				}
				for (int j = (i + 2); j < len; j++) // loop from where we found the ++ up to the length of the buffer
				{
					after[j - (i + 2)] = temp[j];

				}
				memset(temp, '\0', len); // null out temp
				strcat(temp, before); // the following adds our before + ^ + after = temp buffer with ^ 
				strcat(temp, "^");
				strcat(temp, after);
			}
		}
		put3(temp); // put out new Array into the next threads buffer
	}
	return NULL;


}
// similar approach to ++ thread 
void* output() {

	while (1) {
		char* temp = get3(); //get the data from buffer
		strcat(inputs, temp); // store the temp into the global
		char before[1000];// both temp arrays to hold data
		char after[1000];// this holds charcters after 80
		int len = strlen(inputs); //define the length of the global 
		if (strcmp(temp, term) == 0) {//if we see STOP then dont read any more lines
			break;
		}
		while (len > 80) { 
			for (int i = 0; i < 80; i++) //this will copy up to 80 chars into the temp array
			{
				before[i] = inputs[i];
			}
			for (int j = 80; j < len; j++)// this copys over everything after the 80 
			{
				after[j - 80] = inputs[j];
			}
			memset(inputs, '\0', len);
			strcat(inputs, after);
			printf("|%s| \n", before); //array1 holds the 80char line
			len = len - 80;
			memset(before, '\0', len); //reset both arrays
			memset(after, '\0', len);
		}
	}
	return NULL;

}

int main() {
	pthread_t input_thread;
	pthread_t line_seperator_thread;
	pthread_t sign_thread;
	pthread_t output_thread;

	pthread_create(&input_thread, NULL, get_input, NULL);
	pthread_create(&line_seperator_thread, NULL, line_seperator, NULL);
	pthread_create(&sign_thread, NULL, plus_sign, NULL);
	pthread_create(&output_thread, NULL, output, NULL);

	pthread_join(input_thread, NULL);
	pthread_join(line_seperator_thread, NULL);
	pthread_join(sign_thread, NULL);
	pthread_join(output_thread, NULL);


}
