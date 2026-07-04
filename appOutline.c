#include <stdio.h>
#include <sqlite3.h>

//global variables

//buffer to store user input
char operation[150];

//valid inputs
char close[5] = "c";
char help[5] = "help";

//number of inputs
int numInputs = 2;

//input array
char *inputs[2] = {close, help};

//The parse method compares the input to a list of possible inputs, 
//calling the corresponding function. 
//If the input is invalid, it prints 'invalid input'
void parse(char *str){
	int i = 0;
	while(i < numInputs && equalsStr(str, inputs[i]) != true){
		i++;
	}
	switch(i){
		case 0: help();
		case 1: close();
		default: printf("No matching input");
	}
}

//help input
void help(){
	printf("You have reached the help function, please press 'c' to end the program");
}

//close input
void close(){
	printf("closing the program");
	exit(0);
}

//equalsStr is used to compare two strings, a and b
//It returns true if a and b are identical and false otherwise. 
//It is case sensitive.
bool equalsStr(char *a, char *b){
	while(*a == *b){
		if(*a == '\0'){
			return true;
		}
	}
	return false;
}

//main method
int main(){
	printf("Enter 'help' for a list of available commands");
	bool cont = true;
	while(cont){
		print("Failnaught: ");
		char *p = fgets(operation, 150, stdin);

		parse(operation);	
	}
}
