#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h>
#include <string.h>

//global variables

//database
sqlite3 *db;
//buffer to store user input
char operation[5000];
//string used to store active topic
char topic[150];
//used for boolean functions
int external_boolean;
//valid inputs
char help[10] = "help";
char close[10] = "close";
char start[10] = "start";
char end[10] = "end";
char makeNew[10] = "makenew";
char list[10] = "list";
char entry[10] = "entry";
char daily[10] = "daily";
char open[10] = "open";
char lookup[10] = "lookup";
char average[10] = "average";

//number of inputs
int numInputs = 11;
//input array
char *inputs[11] = {help, close, start, end, makeNew, list, entry, daily, open, lookup, average};

//used for copying arguments out of callbacks
typedef struct {
    char **argv;     
    int argc;
} saveArguments;

//global struct
saveArguments saveArgs = {NULL, 0};

//testing for React front-end
int process_input(const char *s){
	int i = *s;
	return i;	
}

//frees all data from the struct
void reset_saveArgs(){
	//frees the memory
        for(int i = 0; i < saveArgs.argc; i++){
                free(saveArgs.argv[i]);
        }
        free(saveArgs.argv);

        //resets the argc and argv
        saveArgs.argc = 0;
        saveArgs.argv = NULL;
}

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
		"'open' : opens a study set for structured study or card entry \n"
		"'lookup [topic] [lowerBound] [upperBound]' : prints the entries with IDs between lowerBound and upperBound \n"
		"'average [numDays]' : prints the average number of hours spent a day studying during the given time period \n\n");
}

//close input
void appClose(){
	printf("\nclosing the program\n\n");
	exit(0);
}

//getArgs takes a pointer to a string as input and parses it into a number of valid arguments
//It separates items by spaces and removes new-line characters
int getArgs_v2(char *str){
	int numArgs = 0;
	int index = 0;
	int startSaveInd = 0;

	//finds the first non-space character
	while(*(str+index) == ' '){
		index++;
	}
	//returns if there are no arguments
	if(*(str+index) == '\n'){
		return 0;
	}
	startSaveInd = index; //this is the new "index 0" of the string

	//arguments are deliniated by spaces. The entry is terminated by a new-line and then a null character
	//we have guarenteed that there is at least 1 valid character (non-space, non-newline, non-null)
	int is_quoted = 0;
	if(*(str+index) == '"'){
		is_quoted = 1;
	}
	index++;

	//counts the number of arguments
	char val = *(str+index);
	while(val != '\0'){
		//toggles the quoted boolean
		if(val == '"'){
			is_quoted = !is_quoted;
		}
		//may count an argument only if it is not quoted
		if(!is_quoted){
			if((val == ' ' || val == '\n') && (*(str+index-1) != ' ')){
				numArgs++;
			}
		}
		//increments
		index++;
		val = *(str+index);
	}

	//checks that the quotations are valid
	if(is_quoted){
		printf("Error: unclosed quotations\n");
		return 1;
	}

	//saves numArgs
	saveArgs.argc = numArgs;
	//allocates space
	saveArgs.argv = malloc(sizeof(char *) * numArgs);

	//shortens index to be the length of the last relevant '\0' character
	//note that there is at least 1 non-space, non-newline, non-null character
    	while(*(str+index) == '\0' || *(str+index) == '\n' || *(str+index) == ' '){
		index--;
	}
	index++;

	//saves the arguments to saveInd
	int saveInd = 0;
	//set is_quoted if the first value is in quotes
	if(*(str+startSaveInd) == '"'){
		is_quoted = 1;
	}
	val = *(str+startSaveInd);
	for(int i = startSaveInd+1; i < index + 1; i++){
		if(*(str+i) == '"'){
			is_quoted = !is_quoted;
		}
		//may count an argument only if it is not quoted
		if(!is_quoted){
			if(( *(str+i) == ' ' || *(str+i) == '\n') && (*(str+i-1) != ' ')){
				saveArgs.argv[saveInd] = strndup((str+startSaveInd), (i - startSaveInd)*sizeof(char));
				saveInd++;
				startSaveInd = i + 1;
			}
			if(*(str+i) == ' ' && *(str+i-1) == ' '){
				startSaveInd = i + 1;
			}
		}
	}
	return 0;
}

int getArgs(char *str){
	int numArgs = 0;
	int index = 0;
	//finds the first non-space character
	printf("\nfinding first non-space character\n");
	while(*str == ' '){
		str++;
	}
	//returns if there are no arguments
	if(*str == '\n'){
		return 0;
	}
	printf("\nincrementing index\n");
	//there is at least 1 non-space, non-new-line, non-null character so we can increment index by 1
	index++;
	//boolean for quotations. 1 if the loop is processing a quotation, 0 otherwise.
	
	int is_quoted = 0;
	if(*str == '"'){
		is_quoted = 1;
	}
	printf("\nThe is_quoted boolean is: %d\n", is_quoted);
	//loops until the end of string to count the number of non-consecutive spaces
	while(*(str+index) != '\0'){
		printf("starting loop\n");
		//checks for closed quotations
		if(*(str+index) == '"'){
			is_quoted = !is_quoted;
		}
		printf("is_quoted: %d\n", is_quoted);
		//does not alter spaces or newlines in quoted string
		if(!is_quoted){
			//checks that the character is a space (guarenteed either the first or non-consecutive)
			if((*(str+index) == ' ' || *(str+index) == '\n') && (*(str+index-1) != ' ' &&  *(str+index-1) != '\0') ){
				numArgs++;
				printf("numArgs: %d; ", numArgs);
				*(str+index) = '\0';	//this changes the input string
				printf("Added null character.\n");
			}
		}
		index++;
		printf("incremented index\n");
	}
	if(is_quoted){
		printf("Error: unclosed quotations");
		return 1;
	}
	printf("Finished processing");

	//shortens index to be the length of the last relevant '\0' character
	//note that there is at least 1 non-space, non-newline, non-null character
    while(*(str+index) == '\0' || *(str+index) == '\n' || *(str+index) == ' '){
		index--;
	}
	//note that at the end, index is equal to the index of the last non-space, non-newline, non-null character
	//saves number of arguments
	saveArgs.argc = numArgs;
	
	//uses Malloc to allocate space in memory for argc and argv of saveArgs
	saveArgs.argv = malloc(sizeof(char *) * numArgs);
	//saves the first argv
	int saveInd = 0;
	saveArgs.argv[saveInd] = strdup(str);
	saveInd++;
	//loops through the rest (does not include the last null-character)
	//the loop will not run if the input is only one character (which is what we want)
	for(int i = 0; i < index; i++){
		//saves the next argument (without white space)
		if((*(str+i) == '\0' && *(str+i+1) != ' ') || (*(str+i) == ' ' && *(str+i+1) != ' ')){
			saveArgs.argv[saveInd] = strdup((str + i + 1));
			saveInd++;
		}
	}	
	//returns 0
	return 0;
}

//takes a string and casts it to an int;
int toInt(char *str){
	int ret = 0;
	for(int i = 0; *(str+i) != '\0'; i++){
		if(!isdigit(*(str+i))){
			printf("Input contained non-number characters. Aborting. ");
			return -1;
		}
		else{
			ret = ret * 10;
			ret = ret + *(str+i) - '0';
		}
	}

	return ret;
}

int validate_identifier(const char *s) {
	if (!s || !*s) return 0;
	for (const char *p = s; *p; p++) {
		if (!isalnum(*p) && *p != '_') return 0;
	}
	return 1;
}

int makenew(){
	//gets new topic name from user to name table
	
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
		"nextStudy REAL,"
		"interval INTEGER DEFAULT 0,"
		"n INTEGER DEFAULT 0,"
		"ef REAL DEFAULT 2.5"
		");",
		name
	);
	//note that the nextStudy REAL line is a stored JULIAN day.
	//note as well that interval, n, and ef are defined for the SM-2 Algorithm

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
	"INSERT INTO sessions (startenergy, start) "
	"VALUES (?, julianday('now'));";

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
			"endtime = julianday('now'), "
			"endenergy = ?, "
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
	sqlite3_bind_int(stmt, 2, focusdepth);
	
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
	if ( argc == 1 ){
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
	char sql[5120];	
		
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

int openTopic_v2(int argc, char* str){
	if(argc != 2){
		printf("\nIncorrect number of arguments. Please try again.\n\n");
		return 1;
	}

	char *err_msg = NULL;
	char sql[512];	

	//prepares the statement
	snprintf(sql, sizeof(sql),
                "SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT IN ('sessions', 'sqlite_sequence') AND name IN ('%s');",
                str
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
		strcpy(topic, str);
		printf("%s has been successfully openned\n\n", str);
	}
	else{
		printf("%s is not a valid entry\n\n", str);
		return 1;
	}
	return 0;
}

int makeEntry(){
	//checks that there is a valid topic
	if(topic[0] == '\0'){
		printf("\nUse 'open' to open a topic or 'makenew' to make a new topic.\n\n");
		return 0;
	}
	//if the topic exists, then the rest of the code will execute
	char sql[5000];
	char *err_msg = NULL;
	char question[1024];
	char answer[1024];
	char solution[2048];
	
	printf("\nWhat question do you want to ask?\n\nFailnaught: ");
	fgets(question, 1024, stdin);

	printf("\nWhat is the answer to your question?\n\nFailnaught: ");
	fgets(answer, 1024, stdin);
	
	printf("\nIf one exists, what is the solution to your question?\n\nFailnaught: ");
	fgets(solution, 2048, stdin);

	snprintf(sql, sizeof(sql), 
		"INSERT INTO %s (question, answer, solution, nextStudy) VALUES (?, ?, ?, julianday('now'))",
		topic
	);

	//begins the sqlite
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if( rc != SQLITE_OK) {
        	printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        	return rc;
    }

	//binds the text
	sqlite3_bind_text(stmt, 1, question, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, answer, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, solution, -1, SQLITE_TRANSIENT);

	/*
	snprintf(sql, sizeof(sql), 
		"INSERT INTO %s (question, answer, solution, nextStudy) VALUES ('%s', '%s', '%s', julianday('now'))",
		topic, question, answer, solution
	);

	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
                sqlite3_free(err_msg);
                sqlite3_close(db);
                return 1;
        }
	*/

	rc = sqlite3_step(stmt);

	if(rc != SQLITE_DONE){
		printf("There has been an error");
	}
	return rc;
}
//takes in 4 argv. Argv[0] = "makeEntry" (not passed), Argv[1] = "Question", Argv[2] = "Answer", Argv[3] = "Solution", Argv[4] = "topic"
//also takes in argc
int makeEntry_v2(int argc, char* question, char* answer, char* solution, char* tpc){
	//checks that there is a valid topic and valid argc, argv
	if(argc > 5){
		printf("\nToo many arguments\n\n");
		return 1;
	}
	else if(argc < 3){
		printf("\nToo few arguments\n\n");
		return 1;
	}
	if(argc == 5){
		int succ = openTopic_v2(2, tpc);
		if(succ == 1){
			//failure
			return 1;
		}
		//otherwise there has been success and a topic is open
	}
	else if(topic[0] == '\0'){
		printf("\nUse 'open' to open a topic or 'makenew' to make a new topic.\n\n");
		return 1;
	}
	//if the topic exists, then the rest of the code will execute
	char sql[5000];
	char *err_msg = NULL;

	snprintf(sql, sizeof(sql), 
		"INSERT INTO %s (question, answer, solution, nextStudy) VALUES (?, ?, ?, julianday('now'))",
		topic
	);

	//begins the sqlite
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if( rc != SQLITE_OK) {
        	printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        	return rc;
    }

	//binds the text
	sqlite3_bind_text(stmt, 1, question, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, answer, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, solution, -1, SQLITE_TRANSIENT);

	rc = sqlite3_step(stmt);

	if(rc != SQLITE_DONE){
		printf("There has been an error");
	}
	return rc;
}
//implements the SM-2 Algorithm
//takes the user grade q, repetition number n, easiness factor ef, interval 
int studyhelper(int q, int *n, double *ef, int *interval){
	if (q >= 3){
		if( *n == 0 ) {
			*interval = 1;		
		}
		else if ( *n == 1){
			*interval = 6;
		}
		else{
			*interval = (*interval) * (*ef);
		}
		*n = *n + 1;
	}
	else{
		*n = 0;
		*interval = 1;
	}

	*ef = *ef + (0.1 - (5.0 - q) * (0.08 + (5 - q) * 0.02));
	if( *ef < 1.3 ){
		*ef = 1.3;
	}

	return 0;
}

int study(){
	//checks that a topic is open
	if(topic[0] == '\0'){
		printf("Please open a topic with 'open' to study a set\n\n");
		return 0;
	}
	//gets the top priority item from the current topic (keeps ID)
	//priority is determined by the length of time past "next study", with the items with the most time being shown first
	char sql[256];

	//gets the newest value.
	snprintf(sql, sizeof(sql), 
		"SELECT * FROM %s WHERE date(nextStudy) <= date(julianday('now')) ORDER BY nextStudy ASC LIMIT 1;",
		topic
	);

	//prepares the statement
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK){
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
	}

	//gets the first row
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW){
		printf("There is no question available for study.\n");
		return 1;
	}

	//gets ID
	int id = sqlite3_column_int(stmt, 0);
	//gets question, answer, and solution
	const char *question = sqlite3_column_text(stmt, 1);
	const char *answer = sqlite3_column_text(stmt, 2);
	const char *solution = sqlite3_column_text(stmt, 3);
	
	printf("\nPrompt:\n%s\n", question);

	//gets the next prompt
	do {
		printf("Enter 'A' for answer and 'S' for the solution\n\n");
		fgets(operation, 150, stdin);
	}
	while(operation[0] != 'A' && operation[0] != 'a' && operation[0] != 'S' && operation[0] != 's' && operation[1] != '\n');
	
	//prints desired output
	if (operation[0] == 'A' || operation[0] == 'a'){
		printf("\nAnswer:\n\n%s\n", answer);

		printf("Would you like the solution? Enter 'S' or 's'\n\n");
		
		fgets(operation, 150, stdin);
		if(operation[0] == 's' || operation[0] == 'S' && operation[1] == '\n'){
			printf("Solution:\n\n%s\n", solution);
		}
	}
	else{
		printf("\nSolution:\n%s\n", solution);
		
		printf("Would you like the answer? Enter 'A' or 'a'\n\n");
		
		fgets(operation, 150, stdin);
		if(operation[0] == 'a' || operation[0] == 'A' && operation[1] == '\n'){
			printf("Answer:\n\n%s\n", answer);
		}
	}

	//collects q (easiness factor)
	int q = -1;
	while(q < 0 || q > 5){
		printf("\nEnter a number corresponding to the difficulty of the problem.\n 0 = complete failure to recall information.\n 1 = incorrect recall, but the content felt familiar.\n 2 = incorrect recall, but the content seems easy to remember.\n 3 = new content or correct recall, but the activity required significant effort to recall.\n 4 = correct recall, after some hesitation.\n 5 = correct with perfect recall. \n\nFailnaught: ");
		
		//collects input
		fgets(operation, 150, stdin);

		//preforms input validation
		if(sscanf(operation, "%d", &q) != 1 || (q < 0 || q > 5)){
			printf("Invalid input. Try again.\n\n");
		}
	}

	//collects n, ef, interval from table
	int interval = sqlite3_column_int(stmt, 5);
	int n = sqlite3_column_int(stmt, 6);
	double ef = sqlite3_column_double(stmt, 7);

	//finalizes the old statement
	sqlite3_finalize(stmt);

	//runs the helper method
	studyhelper(q, &n, &ef, &interval);

	//updates all values (nextStudy, n, ef, interval)
	snprintf(sql, sizeof(sql), 
		"UPDATE %s SET nextStudy = julianday('now') + %d, interval = %d, n = %d, ef = %f WHERE id = %d;",
		topic, interval, interval, n, ef, id
	);

	//prepares the statement
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK){
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
	}

	//executest the statement
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	sqlite3_finalize(stmt);

	return 0;
}

//called individually for each argv we want to save
int callback_daily(void *NotUsed, int argc, char **argv, char **colName) {
	//argc is always 1 when called with callback_daily
	saveArgs.argc = saveArgs.argc + argc;
	//increases the size of the array to accomodate new values
	saveArgs.argv = realloc(saveArgs.argv, sizeof(char*) * saveArgs.argc);
	//populates the space 
	saveArgs.argv[saveArgs.argc - 1] = strdup(argv[0]);
	
	return 0;
}

int dailyStudy(){
	char *err_msg = NULL;
	const char *sql = "SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT IN ('sessions', 'sqlite_sequence');"; 
	
	int rc = sqlite3_exec(db, sql, callback_daily, NULL, &err_msg);

    if (rc != SQLITE_OK) {
	fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
		return 1;
    }

	//accesses the argc and argv in the global struct saveArguments
	for (int i = 0; i < saveArgs.argc; i++) {
		
		//sets the topic first
		snprintf(topic, sizeof(topic), "%s", saveArgs.argv[i]);
		
		//allows the user to skip the topic
		printf("To begin studying %s, press enter. To skip it this time, enter any other input.\n\n", topic);
		fgets(operation, 150, stdin);
		if(operation[0] == '\n'){

			//calls study multiple times until it returns 1
			//note that study should remove 1 element from the list of daily items per run and can not return 1 prematurely.
			while (study() != 1){
				//allows the user to end the topic study prematurely
				printf("To continue with this topic, enter.\n\n");

				fgets(operation, 150, stdin);
				if(operation[0] != '\n'){
					break;
				}
			}
			printf("You have ended your study of %s, or there is no more content to study from %s at this time.\n", topic, topic);
		}
	}

	//frees the memory
	for(int i = 0; i < saveArgs.argc; i++){
		free(saveArgs.argv[i]);
	}
	free(saveArgs.argv);

	//resets the argc and argv
	saveArgs.argc = 0;
	saveArgs.argv = NULL;

	//resets topic
	topic[0] = '\0';

	return 0;
}

int entry_lookup(int argc, char *tpc, int lowerLimit, int upperLimit){
	char *err_msg = NULL;
	char sql[350];
	if(argc < 4 && topic[0] == '\0'){
		printf("Please open a topic to use the lookup feature\n\n");
		return 1;
	}
	if(argc > 4){
		printf("Too many arguments");
		return 1;
	}
	if(argc == 4 && openTopic_v2(2, tpc) == 1){
		return 1;
	}
	
	snprintf(sql, sizeof(sql),
        "SELECT id, question, answer, solution FROM %s WHERE id >= %d AND id <= %d;",
        topic, lowerLimit, upperLimit
    );

	//prepares the statement
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK){
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
	}


	//executes the statement until there are no statements left to execute
	rc = sqlite3_step(stmt);
	while(rc == SQLITE_ROW){
		printf("\n");
		int id = sqlite3_column_int(stmt, 0);
		const char *question = sqlite3_column_text(stmt, 1);
		const char *answer = sqlite3_column_text(stmt, 2);
		const char *solution = sqlite3_column_text(stmt, 3);
		printf("%d | %s | %s | %s\n", id, question, answer, solution);
		rc = sqlite3_step(stmt);
	}
	
	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	return 0;
}

//averages time spent studying over an entered period of time
int average_time(int argc, int numDays){
	char sql[150];

	if(argc != 2){
		printf("Too many arguments.\n");
		return 1;
	}
	if(numDays == 0){
		printf("Div by 0 error.\n");
		return 1;
	}

	snprintf(sql, sizeof(sql),
        "SELECT start, endtime, status FROM sessions WHERE date(start) > date((julianday('now') - %d)) ORDER BY start DESC;",
        numDays
    );

	//prepares the statement
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK){
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
	}

	//stores the number of hours studied in the day.
	double hrs = 0.0;
	//executes the statement until there are no statements left to execute
	rc = sqlite3_step(stmt);
	while(rc == SQLITE_ROW){
		//gets the status (only wants to count the status=1 sessions with a valid stard/end)
		int stat = sqlite3_column_int(stmt, 2);
		if(stat){
			//gets the end-time (greater than start time), then adds it.
			double tm = sqlite3_column_double(stmt, 1);
			hrs = hrs + tm;
			//gets the start-time (less than start time), then subtracts it.
			tm = sqlite3_column_double(stmt, 0);
			hrs = hrs - tm;
		}
		rc = sqlite3_step(stmt);
	}
	
	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	hrs = hrs * 24.0;
	double average = hrs / ((double)numDays); 

	printf("\nYou have spent an average of %f hours a day studying over the last %d days\n\n", average, numDays);
	return 0;
}

//this function gets the ID of the most recent entry
int getMostRecentID(){
	char *sql = "SELECT id FROM sessions ORDER BY id DESC LIMIT 1;";
	sqlite3_stmt *stmt;

	//prepare
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if( rc != SQLITE_OK) {
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    	return rc;
    }

	//execute
	rc = sqlite3_step(stmt);

	if( rc != SQLITE_ROW ){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
    	return rc;
	}
	//get column data
	int id = sqlite3_column_int(stmt, 0);

	//finish
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	return id;
}

//checks that the passed string is a datetime that can be processed by SQLITE3.
//dates are formatted like:
//2026-08-05 16:01:24
int is_valid_datetime(char *str){
	char checkstring[50];
	int checkInd = 0;
	int strPlace = 1;
	int month;
	for(int i = 0; *(str+i) != '\0'; i++){
		if(*(str+i) == '-' || *(str+i) == ' ' || *(str+i) == ':'){
			checkstring[checkInd] = '\0';
				int val = toInt(checkstring);
				if(val == -1){
					printf("Error: Invalid date input. Aborting.\n");
					return 1;
				}
				switch(strPlace){
					case 1:
						if(*(str+i) != '-'){
							printf("Error: date entries must be separated by a '-'.\n");
							return 1;
						}
						if(val < 2026 || val >= 9999){
							printf("Error: Invalid year. Aborting.\n");	
							return 1;
						}
						break;
					case 2:
						if(*(str+i) != '-'){
							printf("Error: date entries must be separated by a '-'.\n");
							return 1;
						}
						if(val < 1 || val > 12){
							printf("Error: Invalid month. Aborting.\n");
							return 1;
						}
						month = val;
						break;
					case 3:
						if(*(str+i) != ' '){
							printf("Error: date and time entries must be separated by a ' '.\n");
							return 1;
						}
						if(month == 2){
							if(val < 1 || val > 28){
								printf("Error: Invalid day. Aborting.\n");
								return 1;
							}
						}
						else if(month == 4 || month == 6 || month == 9 || month == 11){
							if(val < 1 || val > 30){
								printf("Error: Invalid day. Aborting.\n");
								return 1;
							}
						}
						else{
							if(val < 1 || val > 31){
								printf("Error: Invalid day. Aborting.\n");
								return 1;
							}
						}
						break;
					case 4:
						if(*(str+i) != ':'){
							printf("Error: time entries must be separated by a ':'.\n");
							return 1;
						}
						if(val >= 24){
							printf("Error: Invalid hour. Aborting.\n");
							return 1;
						}
						break;
					case 5:
						if(*(str+i) != ':'){
							printf("Error: time entries must be separated by a ':'.\n");
							return 1;
						}
						if(val >= 60){
							printf("Error: Invalid minute. Aborting.\n");
							return 1;
						}
						break;
					default:
						break;
				}
				strPlace++;
				checkInd = 0;
		}
		else if(*(str+i) != '"'){ 
			//note that toInt() validates that the characters are correct
			checkstring[checkInd] = *(str+i);
			checkInd++;
		}
	}
	if(strPlace != 6){
		printf("Error: incorrect number of arguments.\n");
		return 1;
	}
	checkstring[checkInd] = '\0';
	int val = toInt(checkstring);
	if(val == -1){
		printf("Error: Invalid date input. Aborting.\n");
		return 1;
	}
	if(val >= 60){
		printf("Error: Invalid second. Aborting.\n");
		return 1;
	}
	return 0;
}

//this function creates a new entry with start times only
int createSession(char *startTime){
	char sql[150];
	char *err_msg = NULL;
	int is_valid_startTime = is_valid_datetime(startTime);
	if(is_valid_startTime == 1){
		printf("Error: An invalid date was entered. Please correct your date input.\n");
		return 1;
	}
	snprintf(sql, sizeof(sql), "INSERT INTO sessions (start) VALUES (julianday(%s));", startTime);
	
	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	return 0;
}

int closeSession(char *endTime, int id){
	char sql[150];
	char *err_msg = NULL;
	int is_valid_endTime = is_valid_datetime(endTime);
	if(is_valid_endTime == 1){
		printf("Error: An invalid date was entered. Please correct your date input.\n");
		return 1;
	}
	snprintf(sql, sizeof(sql), "UPDATE sessions SET end = (julianday(%s)) WHERE id = %d;", endTime, id);
	
	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_DONE){
		printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	return 0;
}
//this method takes at least a start-time and end-time
//with 4 arguments it takes the startenergy and endenergy
//with 5 arguments it takes the focus depth
int enterSession(int argc, char startTime, char endTime, int startenergy, int endenergy, int focusdepth){
	char *sql = "INSERT INTO sessions (start, endtime, startenergy, endenergy, focusdepth, status) "
		"VALUES (?, ?, ?, ?, ?, 1);";
	if(argc < 3){
		printf("Too few arguments");
		return 1;
	}
	else if(argc > 6){
		printf("Too many arguments");
	}

	//prepares the statement
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
}

//The parse method compares the input to a list of possible inputs,
//calling the corresponding function.
//If the input is invalid, it prints 'invalid input'
void parse(char *str){
    //loads arguments into saveArgs
	int ret = getArgs_v2(str);
	//statement checks that the arguments have been processed correctly
	if(ret != 0 || saveArgs.argc == 0){
		printf("\nPlease enter a valid string\n\n");
	}
	else{
		int i = 0;
    	while(i < numInputs && (equalsStr(saveArgs.argv[0], inputs[i]) != 1)){
			i++;
		}
		if(saveArgs.argc == 1){
			//resets saved Args
			reset_saveArgs();
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
					makeEntry();
					break;
				case 7:
					dailyStudy();
					break;
				case 8: 
					openTopic();
					break;
            	default: 
					printf("\nThere is no matching input. Please retry or enter 'help' for more options.\n\n");
					break;
        	}
		}
		else{
			//there are more than 1 argv
			switch(i){
				case 6:
					if(saveArgs.argc == 3){
						makeEntry_v2(saveArgs.argc, saveArgs.argv[1], saveArgs.argv[2], NULL, NULL);
					}
					else if(saveArgs.argc == 4){
						makeEntry_v2(saveArgs.argc, saveArgs.argv[1], saveArgs.argv[2], saveArgs.argv[3], NULL);
					}
					else if(saveArgs.argc == 5){
						makeEntry_v2(saveArgs.argc, saveArgs.argv[1], saveArgs.argv[2], saveArgs.argv[3], saveArgs.argv[4]);
					}
					else{
						printf("\nIncorrect number of arguments. entry is passed with question, answer; question, answer, solution; or question, answer, solution, topic\n\n");
					}
					break;
				case 8:
					openTopic_v2(saveArgs.argc, saveArgs.argv[1]);
					break;
				case 9:
					int upperBound;
					int lowerBound;
					switch(saveArgs.argc){
						case 2: 
							//passed lookup [num], returns all values less than that num
							upperBound = toInt(saveArgs.argv[1]);
							if(upperBound == -1){
								printf("Invalid integer input. Please try again.\n");
							}
							else{
								entry_lookup(saveArgs.argc, NULL, 0, upperBound);
							}
							break;
						case 3:
							//passed lookup [lowernum] [uppernum], returns all values between those nums, inclusive
							lowerBound = toInt(saveArgs.argv[1]);
							upperBound = toInt(saveArgs.argv[2]);
							if(lowerBound == -1 || upperBound == -1 || lowerBound > upperBound){
								printf("Invalid integer inputs. Please try again.\n");
							}
							else{
								entry_lookup(saveArgs.argc, NULL, lowerBound, upperBound);
							}
							break;
						case 4:
							lowerBound = toInt(saveArgs.argv[2]);
							upperBound = toInt(saveArgs.argv[3]);
							if(lowerBound == -1 || upperBound == -1 || lowerBound > upperBound){
								printf("Invalid integer inputs. Please try again.\n");
							}
							else{
								entry_lookup(saveArgs.argc, saveArgs.argv[1], lowerBound, upperBound);
							}
					}
					break;
				case 10:
					int numDays = toInt(saveArgs.argv[1]);
					if(numDays == -1){
						printf("Invalid integer input. Please try again.\n");
					}
					else{
						average_time(saveArgs.argc, numDays);
					}
					break;
				default:
					printf("\nThere is no matching input.\n\n");
					break;
			}
			//resets the saveArgs parameters
			reset_saveArgs();
		}
	}
}

//main method
int main(){
	//resets topic to '\0'
	topic[0] = '\0';
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
	    "start REAL,"
		"endtime REAL,"
		"startenergy INTEGER,"
		"endenergy INTEGER,"
		"focusdepth INTEGER,"
		"status INTEGER DEFAULT 0"
		");";
	
	//executes the SQL
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
    	sqlite3_free(err_msg);
    	sqlite3_close(db);
    	return 1;
	}

	printf("Enter 'help' for a list of available commands.\n\n");
	
	//note that this is an infinite loop, which is intentional.
	int cont = 1;
	while(cont){
		printf("Failnaught: ");
		char *p = fgets(operation, 150, stdin);

		parse(p);	
	}
	exit(0);
}

