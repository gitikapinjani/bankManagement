#ifndef ENTITIES_H  // Check if ENTITIES_H is not defined
#define ENTITIES_H

#define CUSTOMER_DB "customer_db.txt"
#define TRANSACTION_DB "transaction_db.txt"
#define BANKDETAILS_DB "bankdetails_db.txt"
#define EMPLOYEESETAILS_DB "employee_db.txt"
#define LOAN_DB "loan_db.txt"
#define FEEDBACK_DB "feedback_db.txt"
#define ID_COUNTS_DB "idcounts_db.txt"

struct Customer
{
char cust_id[10];
char cust_name[20];
char cust_username[20];
char cust_password[20];
char active[10];
char cust_fd_amount[10];
char loan_amount[10];
char loan_status[20];  //NA -for not applied
char cust_account_balance[10];
};

struct Employee
{
char emp_id[10];
char emp_name[20];
char emp_username[20];
char emp_password[20];
char active[10];
char role[20];
};

struct Transaction {
    char sender_cust_id[10];
    char receiver_cust_id[10];
    char amount[10];
    char date[20];
};

struct BankDetails{
    char totalBankFunds[10];
    char totalLoanTaken[10];
};

struct Loan {
    char cust_id[10];
    char loan_amount[10];
    char loan_status[20];   // Pending, Approved, Rejected
    char assigned_emp_id[10];
    char applied_date[20];
};

struct Feedback {
    char cust_id[10];
    char feedback[100];
};

int lockFile(FILE *file) {
    int fd = fileno(file);
    if (flock(fd, LOCK_EX) == -1) {
        perror("Error locking file");
        return -1;
    }
    return 0;
}

void unlockFile(FILE *file) {
    int fd = fileno(file);
    flock(fd, LOCK_UN);
}

int incrementIdCount(const char *idType) {
    FILE *idFile = fopen(ID_COUNTS_DB, "r+");
    if (!idFile) {
        perror("Error opening ID counts database");
        return -1;
    }

    int nextId = 0;
    char line[100];
    char temp[100][100]; // Temporary storage for lines
    int lineCount = 0;

    // Read lines and find the specified ID type
    while (fgets(line, sizeof(line), idFile)) {
        if (strstr(line, idType)) {
            sscanf(line, "%*[^:]: %d", &nextId); // Get the current ID
            nextId++; // Increment the ID
            sprintf(line, "%s: %d\n", idType, nextId); // Update the line with the new ID
        }
        strcpy(temp[lineCount++], line); // Store the line
    }

    // Move the file pointer to the beginning
    rewind(idFile);

    // Write back all lines, including the updated line
    for (int i = 0; i < lineCount; i++) {
        fprintf(idFile, "%s", temp[i]);
    }

    fclose(idFile);
    return nextId;
}

void setColor(const char* color) {
    printf("%s", color); // Change the color
}

// Reset color to default
void resetColor() {
    printf("\033[0m");
}

void log_message(const char *level, const char *message)
{
    FILE *logFile = fopen("program.log", "a"); // Open the log file in append mode
    if (logFile == NULL)
    {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    char *timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0'; // Remove the newline character at the end of time

    fprintf(logFile, "[%s] [%s]: %s\n", timeStr, level, message);
    fclose(logFile); // Close the log file after writing
}


#endif  // End of include guard