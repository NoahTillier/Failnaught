#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

//global variables

//buffer to store user input
char operation[150];

//valid inputs
char help[10] = "help\n";
char close[10] = "close\n";
char start[10] = "start\n";
char end[10] = "end\n";

//number of inputs
int numInputs = 4;

//input array
char *inputs[4] = {help, close, start, end};

//help input
void appHelp(){
	printf("\nThe following are available functions:\n\n"
		"'close' : closes the program\n"
		"'start' : starts a study session\n'"
		"end' : ends a started study session\n\n");
}

//close input
void appClose(){
	printf("\nclosing the program\n\n");
	exit(0);
}

//starts the clock by creating a new database entry.
void startClock(){
	const char *insert = 
	"INSERT INTO session (startenergy, endenergy) "
	"VALUES (?, ?);";

}

//ends the clock by creating a new database entry.
void endClock(){

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
		case 2:
			//start
			break;
		case 3: 
			//end
			break;
                default: 
			printf("\nThere is no matching input. Please retry or enter 'help' for more options.\n\n");
			break;
        }
}

//main method
int main(){
	//creates an empty pointer for the database
	sqlite3 *db;
	//creates an error message for sqlite functions
	char *err_msg = NULL;

	//open or create database
	int rc = sqlite3_open("sessions.db", &db);
	
	if (rc != SQLITE_OK){
		//then there has been some error
		return 1;
	}

	//creates a new table if none exists
	const char *sql =
		"CREATE TABLE IF NOT EXISTS sessions ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
	        "start DATETIME DEFAULT CURRENT_TIMESTAMP,"
		"end DATETIME,"
		"startenergy INTEGER,"
		"endenergy INTEGER,"
		"difficulty INTEGER,"
		"focusdepth INTEGER"
		");";
	
	//executes the SQL
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return 1;
	}

	printf("Enter 'help' for a list of available commands.\n\n");
	int cont = 10;
	while(cont){
		printf("Failnaught: ");
		char *p = fgets(operation, 150, stdin);

		parse(p);

		cont--;	
	}
	exit(0);
}
