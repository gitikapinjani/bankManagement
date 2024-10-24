#ifndef EMPLOYEE_FUNCTIONS
#define EMPLOYEE_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <time.h>

#include "entities.h"
#include "message.h"

bool authenticateEmployee(int clientSocket, struct Employee *authenticatedEmp);
bool addCustomer(int clientSocket);
bool modifyCustomer(int clientSocket);
void getAllCustomer(int clientSocket);
int getLastCustomerId(FILE *file);
bool viewTransactionHistory(int clientSocket);
void log_message(const char *level, const char *message);
bool processLoan(int clientSocket, const char *emp_id);
bool assignEmployeeToLoan(int clientSocket);
bool viewFeedbacks(int client_socket);
bool changePassword(int clientSocket, const char *username);
bool viewAssignedLoanAppliction(int clientSocket, const char *empId);

bool employeeOperationHandler(int clientSocket)
{
    struct Employee authenticatedEmp;
    if (authenticateEmployee(clientSocket, &authenticatedEmp))
    {
        send(clientSocket, LOGIN_SUCCESS, strlen(LOGIN_SUCCESS), 0);
        ssize_t writeBytes, readBytes;
        char readbuff[1000];

        if (strcmp(authenticatedEmp.role, "employee") == 0)
        {
            while (1)
            {
                send(clientSocket, EMPLOYEE_MENU, strlen(EMPLOYEE_MENU), 0);
                readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
                if (readBytes == -1)
                {
                    perror(ERROR_ADMIN_MENU);
                    return false;
                }

                int choice = atoi(readbuff);
                switch (choice)
                {
                case 1:
                    if (addCustomer(clientSocket))
                        send(clientSocket, CUSTOMER_ADDED, strlen(CUSTOMER_ADDED), 0);
                    break;
                case 2:
                    if (modifyCustomer(clientSocket))
                        send(clientSocket, CUSTOMER_MODIFIED, strlen(CUSTOMER_MODIFIED), 0);
                    break;
                case 3:
                    getAllCustomer(clientSocket);
                    send(clientSocket, CUSTOMERS_DISPLAYED, strlen(CUSTOMERS_DISPLAYED), 0);
                    break;
                case 4:
                    if (processLoan(clientSocket, authenticatedEmp.emp_id))
                        send(clientSocket, LOAN_PROCESSED, strlen(LOAN_PROCESSED), 0);
                    else
                        send(clientSocket, TRY_AGAIN, strlen(TRY_AGAIN), 0);
                    break;
                case 5:
                    if (viewAssignedLoanAppliction(clientSocket, authenticatedEmp.emp_id))
                        send(clientSocket, LOANS_DISPLAYED, strlen(LOANS_DISPLAYED), 0);
                    break;
                case 6:
                    if (viewTransactionHistory(clientSocket))
                        send(clientSocket, TRANSACTION_HISTORY, strlen(TRANSACTION_HISTORY), 0);
                    break;
                case 7:
                    if (changePassword(clientSocket, authenticatedEmp.emp_username))
                        send(clientSocket, PASSWORD_CHANGED, strlen(PASSWORD_CHANGED), 0);
                    break;
                case 9:
                    send(clientSocket, LOGOUT_SUCCESSFUL, strlen(LOGOUT_SUCCESSFUL), 0);
                    return true;
                case 10:
                    close(clientSocket);
                    return true;
                default:
                    send(clientSocket, INVALID_INPUT, strlen(INVALID_INPUT), 0);
                    continue;
                }
            }
        }
        else if (strcmp(authenticatedEmp.role, "manager") == 0)
        {
            while (1)
            {
                send(clientSocket, MANAGER_MENU, strlen(MANAGER_MENU), 0);
                readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
                if (readBytes == -1)
                {
                    perror(ERROR_ADMIN_MENU);
                    return false;
                }

                int choice = atoi(readbuff);
                switch (choice)
                {
                case 1:
                    if (addCustomer(clientSocket))
                        send(clientSocket, CUSTOMER_ADDED, strlen(CUSTOMER_ADDED), 0);
                    break;
                case 2:
                    if (modifyCustomer(clientSocket))
                        send(clientSocket, CUSTOMER_MODIFIED, strlen(CUSTOMER_MODIFIED), 0);
                    break;
                case 3:
                    getAllCustomer(clientSocket);
                    send(clientSocket, CUSTOMERS_DISPLAYED, strlen(CUSTOMERS_DISPLAYED), 0);
                    break;
                case 4:
                    if (processLoan(clientSocket, authenticatedEmp.emp_id))
                        send(clientSocket, LOAN_PROCESSED, strlen(LOAN_PROCESSED), 0);
                    else
                        send(clientSocket, TRY_AGAIN, strlen(TRY_AGAIN), 0);
                    break;
                case 5:
                    if (viewAssignedLoanAppliction(clientSocket, authenticatedEmp.emp_id))
                        send(clientSocket, LOANS_DISPLAYED, strlen(LOANS_DISPLAYED), 0);
                    break;
                case 8:
                    if (assignEmployeeToLoan(clientSocket))
                        send(clientSocket, LOAN_PROCESSED, strlen(LOAN_PROCESSED), 0);
                    break;
                case 9:
                    if (viewFeedbacks(clientSocket))
                        send(clientSocket, CUSTOMERS_DISPLAYED, strlen(CUSTOMERS_DISPLAYED), 0);
                    break;
                case 10:
                    if (changePassword(clientSocket, authenticatedEmp.emp_username))
                        send(clientSocket, PASSWORD_CHANGED, strlen(PASSWORD_CHANGED), 0);
                    break;
                case 12:
                    send(clientSocket, LOGOUT_SUCCESSFUL, strlen(LOGOUT_SUCCESSFUL), 0);
                    return true;
                case 13:
                    close(clientSocket);
                    return true;
                default:
                    send(clientSocket, INVALID_INPUT, strlen(INVALID_INPUT), 0);
                    continue;
                }
            }
        }
    }
    return false;
}

bool authenticateEmployee(int clientSocket, struct Employee *authenticatedEmployee)
{
    char username[50];
    char password[50];

    send(clientSocket, ENTER_USERNAME, strlen(ENTER_USERNAME), 0);
    int readResult = recv(clientSocket, username, sizeof(username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_USERNAME, strlen(ERROR_RECEIVING_USERNAME), 0);
        return false;
    }
    username[readResult] = '\0';
    if (username[readResult - 1] == '\n')
    {
        username[readResult - 1] = '\0';
    }

    send(clientSocket, ENTER_PASSWORD, strlen(ENTER_PASSWORD), 0);
    readResult = recv(clientSocket, password, sizeof(password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_PASSWORD, strlen(ERROR_RECEIVING_PASSWORD), 0);
        return false;
    }
    password[readResult] = '\0';
    if (password[readResult - 1] == '\n')
    {
        password[readResult - 1] = '\0';
    }

    FILE *dbFile = fopen(EMPLOYEESETAILS_DB, "r");
    if (dbFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        return false;
    }

    if (flock(fileno(dbFile), LOCK_SH) == -1)
    {
        perror(ERROR_LOCK_DB_FILE);
        fclose(dbFile);
        return false;
    }

    char line[512];
    bool authenticated = false;

    while (fgets(line, sizeof(line), dbFile) != NULL)
    {
        struct Employee tempEmp;
        char active[10];

        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmp.emp_name, tempEmp.emp_username, tempEmp.emp_password, tempEmp.role, tempEmp.emp_id, active);

        if (strcmp(username, tempEmp.emp_username) == 0 && strcmp(password, tempEmp.emp_password) == 0)
        {
            *authenticatedEmployee = tempEmp;
            authenticated = true;
            break;
        }
    }

    if (flock(fileno(dbFile), LOCK_UN) == -1)
    {
        perror(ERROR_UNLOCK_DB_FILE);
    }

    fclose(dbFile);

    if (authenticated)
    {
        send(clientSocket, AUTHENTICATION_SUCCESS, strlen(AUTHENTICATION_SUCCESS), 0);
        return true;
    }
    else
    {
        send(clientSocket, AUTHENTICATION_FAILED, strlen(AUTHENTICATION_FAILED), 0);
        return false;
    }
}

bool addCustomer(int clientSocket)
{
    struct Customer cust;

    // Receive customer name from the client
    send(clientSocket, ENTER_CUSTOMER_NAME, strlen(ENTER_CUSTOMER_NAME), 0);
    int readResult = recv(clientSocket, cust.cust_name, sizeof(cust.cust_name) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_NAME, strlen(ERROR_RECEIVING_CUSTOMER_NAME), 0);
        return false;
    }
    cust.cust_name[readResult] = '\0';
    if (cust.cust_name[readResult - 1] == '\n')
    {
        cust.cust_name[readResult - 1] = '\0';
    }

    // Receive customer username from the client
    send(clientSocket, ENTER_CUSTOMER_USERNAME, strlen(ENTER_CUSTOMER_USERNAME), 0);
    readResult = recv(clientSocket, cust.cust_username, sizeof(cust.cust_username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_USERNAME, strlen(ERROR_RECEIVING_CUSTOMER_USERNAME), 0);
        return false;
    }
    cust.cust_username[readResult] = '\0';
    if (cust.cust_username[readResult - 1] == '\n')
    {
        cust.cust_username[readResult - 1] = '\0';
    }

    // Receive customer password from the client
    send(clientSocket, ENTER_CUSTOMER_PASSWORD, strlen(ENTER_CUSTOMER_PASSWORD), 0);
    readResult = recv(clientSocket, cust.cust_password, sizeof(cust.cust_password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_PASSWORD, strlen(ERROR_RECEIVING_CUSTOMER_PASSWORD), 0);
        return false;
    }
    cust.cust_password[readResult] = '\0';
    if (cust.cust_password[readResult - 1] == '\n')
    {
        cust.cust_password[readResult - 1] = '\0';
    }

    // Generate a new customer ID
    int customerId = incrementIdCount("next_customer_id");
    if (customerId == -1)
        return false; // Error occurred while incrementing ID

    sprintf(cust.cust_id, "%d", customerId);

    // Receive customer balance from the client
    send(clientSocket, ENTER_CUSTOMER_BALANCE, strlen(ENTER_CUSTOMER_BALANCE), 0);
    readResult = recv(clientSocket, cust.cust_account_balance, sizeof(cust.cust_account_balance) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_BALANCE, strlen(ERROR_RECEIVING_CUSTOMER_BALANCE), 0);
        return false;
    }
    cust.cust_account_balance[readResult] = '\0';
    if (cust.cust_account_balance[readResult - 1] == '\n')
    {
        cust.cust_account_balance[readResult - 1] = '\0';
    }

    // Set default values
    strcpy(cust.active, "true");
    strcpy(cust.cust_fd_amount, "0");
    strcpy(cust.loan_amount, "0");
    strcpy(cust.loan_status, "NA");

    // Open the database file for appending
    const char *dbFileName = "customer_db.txt";
    int dbFile = open(dbFileName, O_RDWR | O_APPEND | O_CREAT, 0666);
    if (dbFile == -1)
    {
        perror(ERROR_OPEN_DB_FILE);
        return false;
    }

    // Lock the file for writing
    if (flock(dbFile, LOCK_EX) == -1)
    {
        perror(ERROR_LOCK_DB_FILE);
        close(dbFile);
        return false;
    }

    // Format the customer data as a single string
    char custRecord[512];
    snprintf(custRecord, sizeof(custRecord), "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
             cust.cust_id, cust.cust_name, cust.cust_username, cust.cust_password, cust.active, cust.loan_amount, cust.loan_status, cust.cust_fd_amount, cust.cust_account_balance);

    // Write the customer information to the file
    ssize_t writeResult = write(dbFile, custRecord, strlen(custRecord));
    if (writeResult == -1)
    {
        perror(ERROR_WRITE_DB_FILE);
        flock(dbFile, LOCK_UN);
        close(dbFile);
        return false;
    }

    // Unlock the file
    if (flock(dbFile, LOCK_UN) == -1)
    {
        perror(ERROR_UNLOCK_DB_FILE);
        close(dbFile);
        return false;
    }

    // Close the database file
    close(dbFile);

    return true;
}

void getAllCustomer(int clientSocket)
{
    int dbFile = open(CUSTOMER_DB, O_RDONLY);
    if (dbFile == -1)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return;
    }

    // Lock the file for shared reading
    if (flock(dbFile, LOCK_SH) == -1)
    {
        close(dbFile);
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        return;
    }

    // Read and send the customer data
    char buffer[256];
    ssize_t bytesRead;
    while ((bytesRead = read(dbFile, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        send(clientSocket, buffer, bytesRead, 0);
    }

    // Unlock and close the file
    flock(dbFile, LOCK_UN);
    close(dbFile);
}

bool modifyCustomer(int clientSocket)
{
    struct Customer cust;
    const char *dbFileName = "customer_db.txt";
    char custId[10];

    // Receive the customer ID from the client
    send(clientSocket, ENTER_CUSTOMER_ID, strlen(ENTER_CUSTOMER_ID), 0);
    int readResult = recv(clientSocket, custId, sizeof(custId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_ID, strlen(ERROR_RECEIVING_CUSTOMER_ID), 0);
        return false;
    }
    custId[readResult] = '\0';
    if (custId[readResult - 1] == '\n')
    {
        custId[readResult - 1] = '\0';
    }

    // Open the database file for reading and writing
    FILE *dbFile = fopen(dbFileName, "r+");
    if (dbFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        return false;
    }

    // Lock the file for writing
    if (flock(fileno(dbFile), LOCK_EX) == -1)
    {
        perror(ERROR_LOCK_DB_FILE);
        fclose(dbFile);
        return false;
    }

    // Temporary file to store updated records
    FILE *tempFile = fopen("temp_db.txt", "w+");
    if (tempFile == NULL)
    {
        perror(ERROR_OPEN_TEMP_FILE);
        flock(fileno(dbFile), LOCK_UN);
        fclose(dbFile);
        return false;
    }

    // Read the file line by line, search for the customer by ID
    char line[512];
    bool customerFound = false;

    while (fgets(line, sizeof(line), dbFile) != NULL)
    {
        struct Customer tempCust;

        // Parse the customer details from the line
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active, tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        // Compare the ID from the file with the one provided by the client
        if (strcmp(custId, tempCust.cust_id) == 0)
        {
            customerFound = true;

            // Now ask the client for the new details and modify them
            // Receive updated customer name
            send(clientSocket, ENTER_NEW_CUSTOMER_NAME, strlen(ENTER_NEW_CUSTOMER_NAME), 0);
            readResult = recv(clientSocket, cust.cust_name, sizeof(cust.cust_name) - 1, 0);
            if (readResult > 0 && readResult != 1)
            { // Ignore if just pressing enter
                cust.cust_name[readResult] = '\0';
                if (cust.cust_name[readResult - 1] == '\n')
                {
                    cust.cust_name[readResult - 1] = '\0';
                }
                if (strlen(cust.cust_name) > 0)
                {
                    strcpy(tempCust.cust_name, cust.cust_name);
                }
            }

            // Receive updated customer username
            send(clientSocket, ENTER_NEW_CUSTOMER_USERNAME, strlen(ENTER_NEW_CUSTOMER_USERNAME), 0);
            readResult = recv(clientSocket, cust.cust_username, sizeof(cust.cust_username) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.cust_username[readResult] = '\0';
                if (cust.cust_username[readResult - 1] == '\n')
                {
                    cust.cust_username[readResult - 1] = '\0';
                }
                if (strlen(cust.cust_username) > 0)
                {
                    strcpy(tempCust.cust_username, cust.cust_username);
                }
            }

            // Receive updated customer password
            send(clientSocket, ENTER_NEW_CUSTOMER_PASSWORD, strlen(ENTER_NEW_CUSTOMER_PASSWORD), 0);
            readResult = recv(clientSocket, cust.cust_password, sizeof(cust.cust_password) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.cust_password[readResult] = '\0';
                if (cust.cust_password[readResult - 1] == '\n')
                {
                    cust.cust_password[readResult - 1] = '\0';
                }
                if (strlen(cust.cust_password) > 0)
                {
                    strcpy(tempCust.cust_password, cust.cust_password);
                }
            }

            // Receive updated customer loan amount
            send(clientSocket, ENTER_NEW_LOAN_AMOUNT, strlen(ENTER_NEW_LOAN_AMOUNT), 0);
            readResult = recv(clientSocket, cust.loan_amount, sizeof(cust.loan_amount) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.loan_amount[readResult] = '\0';
                if (cust.loan_amount[readResult - 1] == '\n')
                {
                    cust.loan_amount[readResult - 1] = '\0';
                }
                if (strlen(cust.loan_amount) > 0)
                {
                    strcpy(tempCust.loan_amount, cust.loan_amount);
                }
            }

            // Receive updated customer loan status
            send(clientSocket, ENTER_NEW_LOAN_STATUS, strlen(ENTER_NEW_LOAN_STATUS), 0);
            readResult = recv(clientSocket, cust.loan_status, sizeof(cust.loan_status) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.loan_status[readResult] = '\0';
                if (cust.loan_status[readResult - 1] == '\n')
                {
                    cust.loan_status[readResult - 1] = '\0';
                }
                if (strlen(cust.loan_status) > 0)
                {
                    strcpy(tempCust.loan_status, cust.loan_status);
                }
            }

            // Receive updated customer FD amount
            send(clientSocket, ENTER_NEW_FD_AMOUNT, strlen(ENTER_NEW_FD_AMOUNT), 0);
            readResult = recv(clientSocket, cust.cust_fd_amount, sizeof(cust.cust_fd_amount) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.cust_fd_amount[readResult] = '\0';
                if (cust.cust_fd_amount[readResult - 1] == '\n')
                {
                    cust.cust_fd_amount[readResult - 1] = '\0';
                }
                if (strlen(cust.cust_fd_amount) > 0)
                {
                    strcpy(tempCust.cust_fd_amount, cust.cust_fd_amount);
                }
            }

            // Receive updated customer account balance
            send(clientSocket, ENTER_NEW_ACCOUNT_BALANCE, strlen(ENTER_NEW_ACCOUNT_BALANCE), 0);
            readResult = recv(clientSocket, cust.cust_account_balance, sizeof(cust.cust_account_balance) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                cust.cust_account_balance[readResult] = '\0';
                if (cust.cust_account_balance[readResult - 1] == '\n')
                {
                    cust.cust_account_balance[readResult - 1] = '\0';
                }
                if (strlen(cust.cust_account_balance) > 0)
                {
                    strcpy(tempCust.cust_account_balance, cust.cust_account_balance);
                }
            }

            // Write the modified customer back to the temp file
            fprintf(tempFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
                    tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active, tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);
        }
        else
        {
            // Write the unchanged customer to the temp file
            fputs(line, tempFile);
        }
    }

    // Close both files
    fclose(dbFile);
    fclose(tempFile);

    // Unlock the file
    if (flock(fileno(dbFile), LOCK_UN) == -1)
    {
        perror(ERROR_UNLOCK_DB_FILE);
    }

    // If the customer was found, replace the original file with the modified file
    if (customerFound)
    {
        rename("temp_db.txt", dbFileName); // Replace the original file with the temp file
        send(clientSocket, CUSTOMER_MODIFIED_SUCCESS, strlen(CUSTOMER_MODIFIED_SUCCESS), 0);
        return true;
    }
    else
    {
        send(clientSocket, CUSTOMER_NOT_FOUND, strlen(CUSTOMER_NOT_FOUND), 0);
        remove("temp_db.txt"); // Remove the temporary file
        return false;
    }
}

bool viewTransactionHistory(int clientSocket)
{

    char cust_id[10];

    // Receive customer receiver's ID from the client
    send(clientSocket, ENTER_CUSTOMER_ID, strlen(ENTER_CUSTOMER_ID), 0);
    int readResult = recv(clientSocket, cust_id, sizeof(cust_id) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_ID, strlen(ERROR_RECEIVING_CUSTOMER_ID), 0);
        return false;
    }
    cust_id[readResult] = '\0';
    if (cust_id[readResult - 1] == '\n')
    {
        cust_id[readResult - 1] = '\0';
    }

    FILE *transFile;
    char line[256];
    bool transactionFound = false;

    // Open the transaction database file
    transFile = fopen(TRANSACTION_DB, "r");
    if (transFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    // Iterate through each line in the transaction log
    while (fgets(line, sizeof(line), transFile) != NULL)
    {
        char sender_id[10], receiver_id[10], amount[10], date[20];

        // Parse the transaction log entry
        sscanf(line, "Sender: %[^,], Receiver: %[^,], Amount: %[^,], Date: %[^\n]", sender_id, receiver_id, amount, date);

        // Check if the customer is involved in the transaction
        if (strcmp(sender_id, cust_id) == 0 || strcmp(receiver_id, cust_id) == 0)
        {
            // Send the transaction details to the client
            send(clientSocket, line, strlen(line), 0);
            transactionFound = true;
        }
    }

    // Close the transaction log file
    fclose(transFile);

    if (!transactionFound)
        send(clientSocket, NO_TRANSACTION_FOUND, strlen(NO_TRANSACTION_FOUND), 0);
    else
        send(clientSocket, END_OF_TRANSACTION_HISTORY, strlen(END_OF_TRANSACTION_HISTORY), 0);

    return transactionFound;
}

bool assignEmployeeToLoan(int clientSocket)
{
    char custId[10];
    char empId[10];
    char line[512];
    bool found = false;

    // Receive the customer ID
    send(clientSocket, ENTER_CUSTOMER_ID, strlen(ENTER_CUSTOMER_ID), 0);
    int readResult = recv(clientSocket, custId, sizeof(custId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_ID, strlen(ERROR_RECEIVING_CUSTOMER_ID), 0);
        return false;
    }
    custId[readResult] = '\0';
    if (custId[readResult - 1] == '\n')
    {
        custId[readResult - 1] = '\0';
    }

    // Receive the employee ID
    send(clientSocket, ENTER_EMP_ID, strlen(ENTER_EMP_ID), 0);
    readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVE_EMP_ID, strlen(ERROR_RECEIVE_EMP_ID), 0);
        return false;
    }
    empId[readResult] = '\0';
    if (empId[readResult - 1] == '\n')
    {
        empId[readResult - 1] = '\0';
    }

    // Open the loan database file for reading and writing
    FILE *loanFile = fopen(LOAN_DB, "r+");
    if (loanFile == NULL)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    // Lock the loan file for writing
    if (lockFile(loanFile) == -1)
    {
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        fclose(loanFile);
        return false;
    }

    // Open a temporary file for writing updated loan data
    FILE *tempFile = fopen("temp_loan_db.txt", "w+");
    if (tempFile == NULL)
    {
        send(clientSocket, ERROR_OPEN_TEMP_FILE, strlen(ERROR_OPEN_TEMP_FILE), 0);
        unlockFile(loanFile);
        fclose(loanFile);
        return false;
    }

    // Read through the loan database line by line and find the loan to update
    while (fgets(line, sizeof(line), loanFile) != NULL)
    {
        struct Loan tempLoan;
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %[^\n]",
               tempLoan.cust_id, tempLoan.loan_amount, tempLoan.loan_status, tempLoan.assigned_emp_id, tempLoan.applied_date);

        // If the loan belongs to the customer, assign the employee
        if (strcmp(tempLoan.cust_id, custId) == 0)
        {
            found = true;
            strcpy(tempLoan.assigned_emp_id, empId); // Assign employee to this loan
        }

        // Write the (possibly modified) loan data to the temp file
        fprintf(tempFile, "CustomerID: %s, LoanAmount: %s, Status: %s, AssignedEmployee: %s, AppliedDate: %s\n",
                tempLoan.cust_id, tempLoan.loan_amount, tempLoan.loan_status, tempLoan.assigned_emp_id, tempLoan.applied_date);
    }

    // Close both files
    fclose(tempFile);
    unlockFile(loanFile);
    fclose(loanFile);

    // Replace original loan file with the temp file
    remove(LOAN_DB);
    rename("temp_loan_db.txt", LOAN_DB);

    // Notify admin of success or failure
    if (found)
    {
        send(clientSocket, EMPLOYEE_ASSIGNED_SUCCESS, strlen(EMPLOYEE_ASSIGNED_SUCCESS), 0);
    }
    else
    {
        send(clientSocket, CUSTOMER_LOAN_NOT_FOUND, strlen(CUSTOMER_LOAN_NOT_FOUND), 0);
    }

    return true;
}

bool processLoan(int clientSocket, const char *emp_id)
{
    FILE *loanFile, *custFile, *tempFile, *tempCustFile;
    struct Loan loan;
    struct Customer customer;
    char line[512];
    char loan_id[10];
    char decision[10];
    bool loanFound = false;

    // Step 1: Ask employee for the loan they want to process
    send(clientSocket, ENTER_CUSTOMER_ID, strlen(ENTER_CUSTOMER_ID), 0);
    int readResult = recv(clientSocket, loan_id, sizeof(loan_id) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_CUSTOMER_ID, strlen(ERROR_RECEIVING_CUSTOMER_ID), 0);
        return false;
    }
    loan_id[readResult] = '\0';
    if (loan_id[readResult - 1] == '\n')
    {
        loan_id[readResult - 1] = '\0'; // Trim newline
    }

    // Open loan_db.txt to find the loan for the given customer
    loanFile = fopen(LOAN_DB, "r+");
    if (loanFile == NULL)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    // Lock the loan file for reading and writing
    if (flock(fileno(loanFile), LOCK_EX) == -1)
    {
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        fclose(loanFile);
        return false;
    }

    // Step 2: Find the loan corresponding to the given Customer ID
    while (fgets(line, sizeof(line), loanFile) != NULL)
    {
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %s\n",
               loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);

        if (strcmp(loan.cust_id, loan_id) == 0 && strcmp(loan.assigned_emp_id, emp_id) == 0 && strcmp(loan.loan_status, "Pending") == 0)
        {
            loanFound = true;
            break;
        }
    }

    if (!loanFound)
    {
        send(clientSocket, LOAN_NOT_FOUND, strlen(LOAN_NOT_FOUND), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    // Step 3: Ask employee to approve or reject the loan
    send(clientSocket, PROMPT_DECISION, strlen(PROMPT_DECISION), 0);
    readResult = recv(clientSocket, decision, sizeof(decision) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVING_DECISION, strlen(ERROR_RECEIVING_DECISION), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }
    decision[readResult] = '\0';
    if (decision[readResult - 1] == '\n')
    {
        decision[readResult - 1] = '\0'; // Trim newline
    }

    // Step 4: Process the decision
    if (strcmp(decision, "approve") == 0)
    {
        strcpy(loan.loan_status, "Approved");
    }
    else if (strcmp(decision, "reject") == 0)
    {
        strcpy(loan.loan_status, "Rejected");
    }
    else
    {
        send(clientSocket, INVALID_DECISION, strlen(INVALID_DECISION), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    // Step 5: Update loan_db.txt with the new loan status
    fseek(loanFile, 0, SEEK_SET);
    tempFile = fopen("temp_loan_db.txt", "w+");
    if (tempFile == NULL)
    {
        send(clientSocket, ERROR_CREATING_TEMP_LOAN_DB, strlen(ERROR_CREATING_TEMP_LOAN_DB), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    while (fgets(line, sizeof(line), loanFile) != NULL)
    {
        struct Loan tempLoan;
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %s\n",
               tempLoan.cust_id, tempLoan.loan_amount, tempLoan.loan_status, tempLoan.assigned_emp_id, tempLoan.applied_date);

        if (strcmp(tempLoan.cust_id, loan.cust_id) == 0)
        {
            // Update loan status
            fprintf(tempFile, "CustomerID: %s, LoanAmount: %s, Status: %s, AssignedEmployee: %s, AppliedDate: %s\n",
                    loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);
        }
        else
        {
            // Write the existing loan entry
            fprintf(tempFile, "%s", line);
        }
    }

    // Close loan files and replace the old loan database with the updated one
    fclose(loanFile);
    fclose(tempFile);
    rename("temp_loan_db.txt", LOAN_DB);
    flock(fileno(loanFile), LOCK_UN);

    // Step 6: Update customer loan details in customer_db.txt
    custFile = fopen(CUSTOMER_DB, "r+");
    if (custFile == NULL)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    // Lock the customer file for reading and writing
    if (flock(fileno(custFile), LOCK_EX) == -1)
    {
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        fclose(custFile);
        return false;
    }

    fseek(custFile, 0, SEEK_SET);
    tempCustFile = fopen("temp_customer_db.txt", "w+");
    if (tempCustFile == NULL)
    {
        send(clientSocket, ERROR_CREATING_TEMP_CUSTOMER_DB, strlen(ERROR_CREATING_TEMP_CUSTOMER_DB), 0);
        flock(fileno(custFile), LOCK_UN);
        fclose(custFile);
        return false;
    }

    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        struct Customer tempCust;
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
               tempCust.loan_amount, tempCust.loan_status, tempCust.cust_account_balance);

        if (strcmp(tempCust.cust_id, loan.cust_id) == 0)
        {
            // Update customer loan status and loan amount
            strcpy(tempCust.loan_amount, loan.loan_amount);
            strcpy(tempCust.loan_status, loan.loan_status);
        }

        // Write updated or existing customer details to temp file
        fprintf(tempCustFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, Account Balance: %s\n",
                tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
                tempCust.loan_amount, tempCust.loan_status, tempCust.cust_account_balance);
    }

    // Close customer files and replace the old customer database with the updated one
    fclose(custFile);
    fclose(tempCustFile);
    rename("temp_customer_db.txt", CUSTOMER_DB);

    // Step 7: Notify success
    send(clientSocket, LOAN_PROCESSED_SUCCESS, strlen(LOAN_PROCESSED_SUCCESS), 0);
    return true;
}

bool viewFeedbacks(int client_socket)
{
    FILE *feedbackFile;
    char line[256];

    // Open feedback database for reading
    feedbackFile = fopen(FEEDBACK_DB, "r");
    if (feedbackFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        send(client_socket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    // Lock the feedback file for reading
    if (flock(fileno(feedbackFile), LOCK_SH) == -1)
    {
        perror(ERROR_LOCK_DB_FILE);
        fclose(feedbackFile);
        return false;
    }

    // Send all feedbacks line by line
    while (fgets(line, sizeof(line), feedbackFile) != NULL)
    {
        send(client_socket, line, strlen(line), 0);
    }

    // Unlock and close the feedback file
    flock(fileno(feedbackFile), LOCK_UN);
    fclose(feedbackFile);

    // Send a closing message
    char *endMessage = "End of feedbacks.\n";
    send(client_socket, endMessage, strlen(endMessage), 0);
    return true;
}

bool changePassword(int clientSocket, const char *username)
{
    FILE *empFile = fopen(EMPLOYEESETAILS_DB, "r+");
    if (empFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        return false;
    }

    struct Employee employee;
    char line[512];
    bool userFound = false;

    // Lock the file for reading and writing
    int fd = fileno(empFile);
    if (flock(fd, LOCK_EX) == -1)
    {
        perror(ERROR_LOCK_DB_FILE);
        fclose(empFile);
        return false;
    }

    // Step 1: Find the employee by username
    while (fgets(line, sizeof(line), empFile) != NULL)
    {
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               employee.emp_name, employee.emp_username, employee.emp_password, employee.role, employee.emp_id, employee.active);

        if (strcmp(employee.emp_username, username) == 0)
        {
            userFound = true;
            break; // Break on finding the employee
        }
    }

    if (!userFound)
    {
        send(clientSocket, USERNAME_NOT_FOUND, strlen(USERNAME_NOT_FOUND), 0);
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Step 2: Verify the current password
    char currentPassword[20];
    send(clientSocket, ENTER_CURRENT_PASSWORD, strlen(ENTER_CURRENT_PASSWORD), 0);
    recv(clientSocket, currentPassword, sizeof(currentPassword) - 1, 0);
    currentPassword[strcspn(currentPassword, "\n")] = 0; // Remove newline character

    if (strcmp(employee.emp_password, currentPassword) != 0)
    {
        send(clientSocket, CURRENT_PASSWORD_INCORRECT, strlen(CURRENT_PASSWORD_INCORRECT), 0);
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Step 3: Allow user to set a new password
    char newPassword[20];
    send(clientSocket, ENTER_NEW_PASSWORD, strlen(ENTER_NEW_PASSWORD), 0);
    recv(clientSocket, newPassword, sizeof(newPassword) - 1, 0);
    newPassword[strcspn(newPassword, "\n")] = 0; // Remove newline character

    // Step 4: Update the password in the database
    fseek(empFile, 0, SEEK_SET);                         // Go back to the start of the file
    FILE *tempFile = fopen("temp_employee_db.txt", "w"); // Temporary file for updates
    if (tempFile == NULL)
    {
        perror(ERROR_OPEN_TEMP_FILE);
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Write back all employees, updating the password for the user who requested it
    while (fgets(line, sizeof(line), empFile) != NULL)
    {
        struct Employee tempEmployee;
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmployee.emp_name, tempEmployee.emp_username, tempEmployee.emp_password, tempEmployee.role, tempEmployee.emp_id, tempEmployee.active);

        // If we found the matching employee, update their password
        if (strcmp(tempEmployee.emp_username, username) == 0)
        {
            // Use the new password here
            fprintf(tempFile, "Name: %s, Username: %s, Password: %s, Role: %s, ID: %s, Active: %s\n",
                    tempEmployee.emp_name, tempEmployee.emp_username, newPassword, tempEmployee.role, tempEmployee.emp_id, tempEmployee.active);
        }
        else
        {
            // Otherwise, write the original line
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(empFile);
    fclose(tempFile);

    // Replace the old file with the updated one
    remove(EMPLOYEESETAILS_DB);                         // Delete the old employee database
    rename("temp_employee_db.txt", EMPLOYEESETAILS_DB); // Rename the temporary file to the original file name

    send(clientSocket, PASSWORD_CHANGED, strlen(PASSWORD_CHANGED), 0);
    flock(fd, LOCK_UN);
    return true;
}

bool viewAssignedLoanAppliction(int clientSocket, const char *empId)
{
    FILE *loanFile = fopen(LOAN_DB, "r");
    if (loanFile == NULL)
    {
        perror(ERROR_OPEN_DB_FILE);
        return false;
    }

    int fd = fileno(loanFile);
    if (flock(fd, LOCK_SH) == -1)
    { // Shared lock for reading
        perror(ERROR_LOCK_DB_FILE);
        fclose(loanFile);
        return false;
    }

    struct Loan loan;
    char line[256];
    char response[2048] = ""; // Buffer for sending the response

    // Read loans from file
    while (fgets(line, sizeof(line), loanFile) != NULL)
    {
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %s\n", 
               loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);

        log_message("INFO", loan.assigned_emp_id);
        log_message("INFO", empId);
        // Check if the loan is assigned to the given employee
        if (strcmp(loan.assigned_emp_id, empId) == 0)
        {
            char loanDetails[256];
            sprintf(loanDetails, "CustomerID: %s, LoanAmount: %s, Status: %s, Date: %s\n",
                    loan.cust_id, loan.loan_amount, loan.loan_status, loan.applied_date);
            strcat(response, loanDetails);
        }
    }

    flock(fd, LOCK_UN); // Unlock file after reading
    fclose(loanFile);

    // Send response to client
    if (strlen(response) == 0)
    {
        strcpy(response, "No loan applications assigned to you.\n");
    }
    send(clientSocket, response, strlen(response), 0);
    return true;
}

#endif