#include <stdio.h>
#include <stdlib.h>

//global variables

//buffer to store user input
char operation[150];

//valid inputs
char close[10] = "close\n\0";
char help[10] = "help\n\0";

//number of inputs
int numInputs = 2;

//input array
char *inputs[2] = {help, close};

//help input
void appHelp(){
	printf("You have reached the help function, please enter 'close'  to end the program\n");
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
		a++;
		b++;
	}
	return 0;
}

//The parse method compares the input to a list of possible inputs,
//calling the corresponding function.
//If the input is invalid, it prints 'invalid input'
void parse(char *str){
        int i = 0;
        while(i < numInputs && (equalsStr(str, inputs[i]) != 1)){
		i++;
        }
        switch(i){
                case 0: 
			appHelp();
			break;
                case 1: 
			appClose();
			break;
                default: 
			printf("No matching input\n");
			break;
        }
}

//main method
int main(){
	printf("Enter 'help' for a list of available commands.\n");
	int cont = 10;
	while(cont){
		printf("Failnaught: ");
		char *p = fgets(operation, 150, stdin);

		parse(p);

		cont--;	
	}
	exit(0);
}
