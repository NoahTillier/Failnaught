#include <stdio.h>
#include <stdlib.h>

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

//help input
void appHelp(){
	printf("You have reached the help function, please press 'c' to end the program\n");
}

//close input
void appClose(){
	printf("closing the program\n");
	exit(0);
}

//equalsStr is used to compare two strings, a and b
//It returns true if a and b are identical and false otherwise. 
//It is case sensitive.
int equalsStr(char *a, char *b){
	while(*a == *b){
		if(*a == '\0'){
			return 1;
		}
	}
	return 0;
}

//The parse method compares the input to a list of possible inputs,
//calling the corresponding function.
//If the input is invalid, it prints 'invalid input'
void parse(char *str){
        int i = 0;
        while(i < numInputs && equalsStr(str, inputs[i]) != 1){
                i++;
        }
        switch(i){
                case 0: appHelp();
                case 1: appClose();
                default: printf("No matching input\n");
        }
}

//main method
int main(){
	printf("Enter 'help' for a list of available commands.\n");
	int cont = 1;
	while(cont){
		printf("Failnaught: ");
		char *p = fgets(operation, 150, stdin);

		parse(operation);
		
		cont = 0;	
	}
}
