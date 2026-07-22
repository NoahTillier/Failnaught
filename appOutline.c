#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h>

//global variables

//database
sqlite3 *db;
//buffer to store user input
char operation[150];
//string used to store active topic
char topic[150];
//used for boolean functions
int external_boolean;
//valid inputs
char help[10] = "help\n";
char close[10] = "close\n";
char start[10] = "start\n";
char end[10] = "end\n";
char makeNew[10] = "makenew\n";
char list[10] = "list\n";
char entry[10] = "entry\n";
char daily[10] = "daily\n";
char open[10] = "open\n";

//number of inputs
int numInputs = 9;
//input array
char *inputs[9] = {help, close, start, end, makeNew, list, entry, daily, open};


//help input
void appHelp(){
	printf("\nThe following are available functions:\n\n"
		"'close' : closes the program\n"
		"'start' : starts a study session\n"
		"'end' : ends a started study session\n"
		"'makenew' : creates a new studyset\n"
		"'list' : lists all your current studysets\n"
		"'entry' : creates a new entry in a studyset\n"
		"'daily' : begins review of daily sets \n"
		"'open' : opens a study set for structured study or card entry \n\n");
}

//close input
void appClose(){
	printf("\nclosing the program\n\n");
	exit(0);
}

int validate_identifier(const char *s) {
	if (!s || !*s) return 0;
	for (const char *p = s; *p; p++) {
		if (!isalnum(*p) && *p != '_') return 0;
	}
	return 1;
}

int makenew(){
	//gets new subject name from user to name table
	
	int conf = 1;
	char name[150];
	while (conf) {
		//gets input from user.
		printf("\nEnter a name for your new studyset.\n\n");

		fgets(operation, 150, stdin);

		//removes the \n at the end of the input
		int i = 0;
		while(*(operation+i) != '\n'){
				i++;
		}
		*(operation+i) = '\0';
		
		//prints the input to the array 'name'
		snprintf(name, sizeof(operation), "%s", operation);
		
		//sanitize entry so that it can only include letters, numbers, and underscores.
		if (validate_identifier(name)) { 

			printf("\nYour entry was: '%s'. To finalize, enter 'Y' or 'y'. To quit, enter 'Q' or 'q'. Enter any input to try again.\n\n", name);

			//confirms the input
			fgets(operation, 150, stdin);

			if(*operation == 'y' || *operation == 'Y'){
				conf = 0;
			}
			else if(*operation == 'Q' || *operation == 'q'){
				return 0;
			}
		}
		else{
			printf("Your entry may only include numbers, letters, and underscores. Please try again.");
		}
	}
	
	//generates the sql code
	char sql[512];
	char *err_msg = NULL;

	//prints the sql command to sql
	snprintf(sql, sizeof(sql),
		"CREATE TABLE IF NOT EXISTS %s ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"question TEXT NOT NULL,"
		"answer TEXT,"
		"solution TEXT,"
		"nextStudy REAL"
		");",
		name
	);
	//note that the nextStudy REAL line is a stored JULIAN day.
	
	//executes sql
	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK) {
                sqlite3_free(err_msg);
                sqlite3_close(db);
                return 1;
        }

	return rc;
}

//starts the clock by creating a new database entry.
int startClock(){
	//start function
	const char *insert = 
	"INSERT INTO sessions (startenergy) "
	"VALUES (?);";

	int startEnergy = -1;
	
	while(startEnergy < 1 || startEnergy > 10){
	//gets values from user
		printf("\nWhat is your current energy level from 1 to 10?\n\nFailnaught: ");
	
		fgets(operation, 150, stdin);

		//preforms input validation
                if (sscanf(operation, "%d", &startEnergy) != 1 || (startEnergy < 1 || startEnergy > 10)){
                        printf("Invalid input. Try again.\n\n");
                }
	}

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, insert, -1, &stmt, NULL);

	if( rc != SQLITE_OK) {
        	printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        	return rc;
    	}

	//bind parameters
	sqlite3_bind_int(stmt, 1, startEnergy);

	//execute
	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE) {
		printf("Execution failed: %s\n", sqlite3_errmsg(db));
	}
	else{
		printf("\nYou have started a new study session.\n\n");
	}

	sqlite3_finalize(stmt);
	return rc;
}

//ends the clock by creating a new database entry.
int endClock(){
	//end function
	const char *endfun = 
		"UPDATE sessions SET "
			"end = CURRENT_TIMESTAMP, "
			"endenergy = ?, "
			"difficulty = ?, "
			"focusdepth = ?, "
			"status = 1 "	
		"WHERE "
			"id = (SELECT MAX(id) FROM sessions);";
	//status
	int status = -1;
	sqlite3_stmt *stmt;
	//get status function
	const char *statfun = 
		"SELECT status " 
		"FROM sessions "
		"WHERE id = (SELECT MAX(id) FROM sessions);";
	
	//error message string
	char *err = NULL;

	//execute statfun
	//The prepare statement compiles statfun into stmt.
	int rc  = sqlite3_prepare_v2(db, statfun, -1, &stmt, NULL);
	
	//checks to make sure that the statement was compiled without errors
	if(rc != SQLITE_OK){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}
	
	//the step statement evaluates the first row of results.
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW){
		status = sqlite3_column_int(stmt, 0);
	} else {
		printf("\nYou do not have any study sessions to close.\n\n");
		return rc;
	}
	
	//the finalize statement destroys stmt so it can be reused.
	sqlite3_finalize(stmt);

	if(status == 1){
		printf("\nThere are active no sessions to end.\n\n");
		return rc;
	}

	//otherwise, begins the endfun
	int endenergy = -1;
	int difficulty = -1;
	int focusdepth = -1;

	//collects the endenergy value
	while(endenergy < 1 || endenergy > 10){
		printf("\nRate your energy after this study session from 1 to 10.\n\nFailnaught: ");
		
		//collects input
		fgets(operation, 150, stdin);
		
		//preforms input validation
		if (sscanf(operation, "%d", &endenergy) != 1 || (endenergy < 1 || endenergy > 10)){
			printf("Invalid input. Try again.\n\n");
		}
	}

	//collects the difficulty value
	while(difficulty < 0 || difficulty > 5){
		printf("\nEnter a number corresponding to the difficulty of the activity you were doing. Estimate a weighted average if necessary.\n 0 = complete failure to recall information.\n 1 = incorrect recall, but the content felt familiar.\n 2 = incorrect recall, but the content seems easy to remember.\n 3 = new content or correct recall, but the activity required significant effort to recall.\n 4 = correct recall, after some hesitation.\n 5 = correct with perfect recall. \n\nFailnaught: ");
		
		//collects input
		fgets(operation, 150, stdin);

		//preforms input validation
		if(sscanf(operation, "%d", &difficulty) != 1 || (difficulty < 0 || difficulty > 5)){
			printf("Invalid input. Try again.\n\n");
		}
	}

	//collects the focusdepth value
	while(focusdepth < 1 || focusdepth > 5){
		printf("\nEnter a number corresponding to the activity you were doing. Estimate a weighted average if necessary.\n Logical proofs = 5\n Practice problems or a project = 4\n Notecard practice = 3\n Note taking or in-class participation = 2\n Light reading = 1\n\n Failnaught: ");
		
		//collects input
		fgets(operation, 150, stdin);

		//preforms input validation
		if(sscanf(operation, "%d", &focusdepth) != 1 || (focusdepth < 1 || focusdepth > 5)){
			printf("Invalid input. Try again.\n\n");
		}
	}


	//prepares endfun to be bound
	rc = sqlite3_prepare_v2(db, endfun, -1, &stmt, NULL);

	//checks taht the statement compiled without errors
	if(rc != SQLITE_OK){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	sqlite3_bind_int(stmt, 1, endenergy);
	sqlite3_bind_int(stmt, 2, difficulty);
	sqlite3_bind_int(stmt, 3, focusdepth);
	
	//runs the line
	sqlite3_step(stmt);

	//finalizes
	sqlite3_finalize(stmt);

	printf("\nYour study session has ended.\n\n");
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

//AI code--callback
int callback(void *NotUsed, int argc, char **argv, char **colName) {
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);   // print table name
	}
	return 0;
}

int callback_v2(void *NotUsed, int argc, char **argv, char **colName) {
	if ( argc = 1 ){
		external_boolean = 1;
	}
	else {
		external_boolean = 0;
	}
	return 0;
}

int listTopics(){
	char *err_msg = NULL;
	const char *sql = "SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT IN ('sessions', 'sqlite_sequence');"; 
	
	int rc = sqlite3_exec(db, sql, callback, NULL, &err_msg);

        if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
                return 1;
        }

	return 0;
}

int openTopic(){
	char *err_msg = NULL;
	char sql[512];	
		
	printf("\nWhich topic do you want to study?\n\n");
	
	//gets input
	fgets(operation, 150, stdin);
	//removes new line statement at the end of operation
	int i = 0;
        while(*(operation+i) != '\n'){
		i++;
        }
        *(operation+i) = '\0';
	//removes new line at the end of the operation	
	
	//prepares the statement
	snprintf(sql, sizeof(sql),
                "SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT IN ('sessions', 'sqlite_sequence') AND name IN ('%s');",
                operation
        );
	
	//resets the boolean so that the value is reliable
	external_boolean = 0;
	//runs the program
	int rc = sqlite3_exec(db, sql, callback_v2, NULL, &err_msg);
	
	if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
                return 1;
        }

	//prints if the thing is a valid entry
	if(external_boolean){
		//copies operation to topic
		int i = 0;
		while(*(operation + i) != '\0'){
			topic[i] = operation[i];
			i++;
		}
		topic[i] = '\0';

		printf("%s has been successfully openned\n\n", operation);
	}
	else{
		printf("%s is not a valid entry\n\n", operation);
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
			startClock();
			break;
		case 3: 
			endClock();
			break;
		case 4:
			makenew();
			break;
		case 5:
			listTopics();
			break;
		case 6:
			//entry
			break;
		case 7:
			//daily
			break;
		case 8: 
			openTopic();
			break;
                default: 
			printf("\nThere is no matching input. Please retry or enter 'help' for more options.\n\n");
			break;
        }
}

//main method
int main(){
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
		"focusdepth INTEGER,"
		"status INTEGER DEFAULT 0"
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

