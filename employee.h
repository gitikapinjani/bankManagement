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

#include "customer.h"
#include "entities.h"

#define TRANSACTION_DB "transaction_db.txt"

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

char employeeRole[10];

bool employeeOperationHandler(int clientSocket)
{
    struct Employee authenticatedEmp;
    if (authenticateEmployee(clientSocket, &authenticatedEmp))
    {
        send(clientSocket, "Login Successfully\n", strlen("Login Successfully\n"), 0);
        ssize_t writeBytes, readBytes;        // Number of bytes read from / written to the client
        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client

        if (strcmp(employeeRole, "employee") == 0)
        {
            while (1)
            {
                char leMenuEmployee[] = "Do you want to:\n - 1. Add new Customer\n - 2. Modify Customer Detail\n - 3. view all customers \n - 4. reject / approve loan \n - 5. view assigned loan application\n - 6. view transaction of customer(change customer password, view transaction, exit)\n - 7. Change Password\n - 8. logout\n - 9. Exit from this menu\n";
                send(clientSocket, leMenuEmployee, strlen(leMenuEmployee), 0);
                readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
                if (readBytes == -1)
                {
                    perror("Error while reading client's choice for ADMIN_MENU");
                    return false;
                }

                int choice = atoi(readbuff);
                switch (choice)
                {
                case 1:
                    if (addCustomer(clientSocket))
                    {
                        send(clientSocket, "Successfully added the employee\n", strlen("Successfully added the employee\n"), 0);
                    }
                    break;
                case 2:
                    if (modifyCustomer(clientSocket))
                    {
                        send(clientSocket, "Successfully modified employee!!\n", strlen("Successfully modified employee!!\n"), 0);
                    }
                    break;

                case 3:
                    getAllCustomer(clientSocket);
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                    break;
                case 4:
                    if (processLoan(clientSocket, authenticatedEmp.emp_id))
                    {
                        send(clientSocket, "Successfully deactivate the faculty!!\n", strlen("Successfully deactivate the faculty!!\n"), 0);
                    }
                    else
                    {
                        send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
                    }
                    break;

                // case 5:
                //     if (update_student(clientSocket))
                //     {
                //         send(clientSocket, "Successfully updated the student!!\n", strlen("Successfully updated the student!!\n"), 0);
                //     }
                //     else
                //     {
                //         send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
                //     }

                //     break;
                case 6:
                    if (viewTransactionHistory(clientSocket))
                    {
                        send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                    }
                    break;
                case 7:
                    if(changePassword(clientSocket, authenticatedEmp.emp_username))
                    send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                    break;
                default:
                    send(clientSocket, "Sorry...Had to logout", strlen("Sorry...Had to logout"), 0);
                    close(clientSocket);
                    return false;
                }
            }
        }
        else
        {
            while (1)
            {
                char leMenuManager[] = "Do you want to:\n - 1. Add new Customer\n - 2. Modify Customer Detail\n - 3. view all customers \n - 4. reject / approve loan \n - 5. view assigned loan application\n - 6. view transaction of customer(change customer password, view transaction, exit)\n - 7. Activate/deactivate customer account\n - 8. Assign Loan application process to employee \n - 9. review customer feedback\n - 10. Change Password\n - 11. logout\n - 12. Exit from this menu\n";
                send(clientSocket, leMenuManager, strlen(leMenuManager), 0);
                readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
                if (readBytes == -1)
                {
                    perror("Error while reading client's choice for ADMIN_MENU");
                    return false;
                }

                int choice = atoi(readbuff);
                switch (choice)
                {
                case 1:
                    if (addCustomer(clientSocket))
                    {
                        send(clientSocket, "Successfully added the employee\n", strlen("Successfully added the employee\n"), 0);
                    }
                    break;
                case 2:
                    if (modifyCustomer(clientSocket))
                    {
                        send(clientSocket, "Successfully modified employee!!\n", strlen("Successfully modified employee!!\n"), 0);
                    }
                    break;

                case 3:
                    getAllCustomer(clientSocket);
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                    break;
                case 4:
                    if (processLoan(clientSocket, authenticatedEmp.emp_id))
                    {
                        send(clientSocket, "Successfully deactivate the faculty!!\n", strlen("Successfully deactivate the faculty!!\n"), 0);
                    }
                    else
                    {
                        send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
                    }
                    break;

                // case 5:
                //     if (update_student(clientSocket))
                //     {
                //         send(clientSocket, "Successfully updated the student!!\n", strlen("Successfully updated the student!!\n"), 0);
                //     }
                //     else
                //     {
                //         send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
                //     }

                //     break;
                case 6:
                    if (viewTransactionHistory(clientSocket))
                    {
                        send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                    }
                    break;
                case 7:
                    close(clientSocket);
                    break;
                case 8:
                    if(assignEmployeeToLoan(clientSocket))
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                    break;
                case 9:
                    if(viewFeedbacks(clientSocket))
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                    break;
                case 10:
                    if(changePassword(clientSocket, authenticatedEmp.emp_username))
                    send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                    break;
                default:
                    send(clientSocket, "Sorry...Had to logout", strlen("Sorry...Had to logout"), 0);
                    close(clientSocket);
                    return false;
                }
            }
        }
    }
}

bool authenticateEmployee(int clientSocket, struct Employee *authenticatedEmployee)
{
    const char *dbFileName = "employee_db.txt";
    char username[50];
    char password[50];

    // Ask for the username
    char usernamePrompt[] = "Enter username: ";
    send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
    int readResult = recv(clientSocket, username, sizeof(username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving username", strlen("Error receiving username"), 0);
        return false;
    }
    username[readResult] = '\0';
    if (username[readResult - 1] == '\n')
    {
        username[readResult - 1] = '\0';
    }

    // Ask for the password
    char passwordPrompt[] = "Enter password: ";
    send(clientSocket, passwordPrompt, strlen(passwordPrompt), 0);
    readResult = recv(clientSocket, password, sizeof(password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving password", strlen("Error receiving password"), 0);
        return false;
    }
    password[readResult] = '\0';
    if (password[readResult - 1] == '\n')
    {
        password[readResult - 1] = '\0';
    }

    // Open the employee database file
    FILE *dbFile = fopen(dbFileName, "r");
    if (dbFile == NULL)
    {
        perror("Error opening the database file");
        return false;
    }

    // Lock the file for reading
    if (flock(fileno(dbFile), LOCK_SH) == -1)
    {
        perror("Error locking the file for reading");
        fclose(dbFile);
        return false;
    }

    // Read the file line by line and search for the matching username and password
    char line[512];
    bool authenticated = false;

    while (fgets(line, sizeof(line), dbFile) != NULL)
    {
        struct Employee tempEmp;
        char active[10];

        // Parse the line to extract employee data (including username and password)
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmp.emp_name, tempEmp.emp_username, tempEmp.emp_password, tempEmp.role, tempEmp.emp_id, active);

        // Compare the username and password with the data in the file
        if (strcmp(username, tempEmp.emp_username) == 0 && strcmp(password, tempEmp.emp_password) == 0)
        {
            *authenticatedEmployee = tempEmp;
            if (strcmp(tempEmp.role, "manager") == 0)
                strcpy(employeeRole, "manager");
            else if (strcmp(tempEmp.role, "employee") == 0)
                strcpy(employeeRole, "employee");
            ;
            authenticated = true;
            break;
        }
    }

    // Unlock the file
    if (flock(fileno(dbFile), LOCK_UN) == -1)
    {
        perror("Error unlocking the file");
    }

    // Close the database file
    fclose(dbFile);

    // Send the authentication result to the client
    if (authenticated)
    {
        send(clientSocket, "Authentication successful", strlen("Authentication successful"), 0);
        return true;
    }
    else
    {
        send(clientSocket, "Authentication failed", strlen("Authentication failed"), 0);
        return false;
    }
}

bool addCustomer(int clientSocket)
{
    struct Customer cust;

    // Receive customer name from the client
    char namePrompt[] = "Enter Customer name: ";
    send(clientSocket, namePrompt, strlen(namePrompt), 0);
    int readResult = recv(clientSocket, cust.cust_name, sizeof(cust.cust_name) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer name from client", strlen("Error receiving Customer name from client"), 0);
        return false;
    }
    cust.cust_name[readResult] = '\0';
    if (cust.cust_name[readResult - 1] == '\n')
    {
        cust.cust_name[readResult - 1] = '\0';
    }

    // Receive customer username from the client
    char usernamePrompt[] = "Enter Customer username: ";
    send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
    readResult = recv(clientSocket, cust.cust_username, sizeof(cust.cust_username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer username from client", strlen("Error receiving Customer username from client"), 0);
        return false;
    }
    cust.cust_username[readResult] = '\0';
    if (cust.cust_username[readResult - 1] == '\n')
    {
        cust.cust_username[readResult - 1] = '\0';
    }

    // Receive customer password from the client
    char passPrompt[] = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);
    readResult = recv(clientSocket, cust.cust_password, sizeof(cust.cust_password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer password from client", strlen("Error receiving Customer password from client"), 0);
        return false;
    }
    cust.cust_password[readResult] = '\0';
    if (cust.cust_password[readResult - 1] == '\n')
    {
        cust.cust_password[readResult - 1] = '\0';
    }

    int customerId = incrementIdCount("next_customer_id");
    if (customerId == -1) return false; // Error occurred while incrementing ID

    sprintf(cust.cust_id, "%d", customerId);

    // Receive customer balance from the client
    char balancePrompt[] = "Enter customer balance: ";
    send(clientSocket, balancePrompt, strlen(balancePrompt), 0);
    readResult = recv(clientSocket, cust.cust_account_balance, sizeof(cust.cust_account_balance) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
        return false;
    }
    cust.cust_account_balance[readResult] = '\0';
    if (cust.cust_account_balance[readResult - 1] == '\n')
    {
        cust.cust_account_balance[readResult - 1] = '\0';
    }

    // Set active status
    strcpy(cust.active, "true");
    // Set FD amount as 0
    strcpy(cust.cust_fd_amount, "0");
    // Set loan amount as 0
    strcpy(cust.loan_amount, "0");
    // set NA as loan status
    strcpy(cust.loan_status, "NA");

    // Open the database file for appending
    const char *dbFileName = "customer_db.txt";
    int dbFile = open(dbFileName, O_RDWR | O_APPEND | O_CREAT, 0666);
    if (dbFile == -1)
    {
        perror("Error opening or creating the database file");
        return false;
    }

    // Lock the file for writing
    if (flock(dbFile, LOCK_EX) == -1)
    {
        perror("Error locking the file");
        close(dbFile);
        return false;
    }

    // Format the customer data as a single string with newlines
    char custRecord[512];
    snprintf(custRecord, sizeof(custRecord), "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
             cust.cust_id, cust.cust_name, cust.cust_username, cust.cust_password, cust.active, cust.loan_amount, cust.loan_status, cust.cust_fd_amount, cust.cust_account_balance);

    // Write the customer information to the file as a single line
    ssize_t writeResult = write(dbFile, custRecord, strlen(custRecord));
    if (writeResult == -1)
    {
        perror("Error writing customer record to database file");
        flock(dbFile, LOCK_UN);
        close(dbFile);
        return false;
    }

    // Unlock the file
    if (flock(dbFile, LOCK_UN) == -1)
    {
        perror("Error unlocking the file");
        close(dbFile);
        return false;
    }

    // Close the database file
    close(dbFile);

    return true;
}

void getAllCustomer(int clientSocket)
{
    int dbFile = open("customer_db.txt", O_RDONLY);
    if (dbFile == -1)
    {
        send(clientSocket, "Error opening database file\n", strlen("Error opening database file\n"), 0);
        return;
    }

    // Lock the file for shared reading
    if (flock(dbFile, LOCK_SH) == -1)
    {
        close(dbFile);
        send(clientSocket, "Error locking the database file\n", strlen("Error locking the database file\n"), 0);
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
    char idPrompt[] = "Enter Customer ID to modify: ";
    send(clientSocket, idPrompt, strlen(idPrompt), 0);
    int readResult = recv(clientSocket, custId, sizeof(custId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
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
        perror("Error opening the database file");
        return false;
    }

    // Lock the file for writing
    if (flock(fileno(dbFile), LOCK_EX) == -1)
    {
        perror("Error locking the file");
        fclose(dbFile);
        return false;
    }

    // Temporary file to store updated records
    FILE *tempFile = fopen("temp_db.txt", "w+");
    if (tempFile == NULL)
    {
        perror("Error opening temporary file");
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
            char namePrompt[] = "Enter new Customer name (leave blank to keep current): ";
            send(clientSocket, namePrompt, strlen(namePrompt), 0);
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
            char usernamePrompt[] = "Enter new Customer username (leave blank to keep current): ";
            send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
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
            char passPrompt[] = "Enter new password (leave blank to keep current): ";
            send(clientSocket, passPrompt, strlen(passPrompt), 0);
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
            char loanAmountPrompt[] = "Enter new Loan Amount (leave blank to keep current): ";
            send(clientSocket, loanAmountPrompt, strlen(loanAmountPrompt), 0);
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
            char loanStatusPrompt[] = "Enter New Loan Status (leave blank to keep current): ";
            send(clientSocket, loanStatusPrompt, strlen(loanStatusPrompt), 0);
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
            char fdStatusPrompt[] = "Enter new FD Amount (leave blank to keep current): ";
            send(clientSocket, fdStatusPrompt, strlen(fdStatusPrompt), 0);
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
            char balancePrompt[] = "Enter new Account Balance (leave blank to keep current): ";
            send(clientSocket, balancePrompt, strlen(balancePrompt), 0);
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
        perror("Error unlocking the file");
    }

    // If the customer was found, replace the original file with the modified file
    if (customerFound)
    {
        rename("temp_db.txt", dbFileName); // Replace the original file with the temp file
        send(clientSocket, "Customer modified successfully", strlen("Customer modified successfully"), 0);
        return true;
    }
    else
    {
        send(clientSocket, "Customer not found", strlen("Customer not found"), 0);
        remove("temp_db.txt"); // Remove the temporary file
        return false;
    }
}

bool viewTransactionHistory(int clientSocket)
{

    char cust_id[10];

    // Receive customer receiver's ID from the client
    char idPrompt[] = "Enter receiver's ID: ";
    send(clientSocket, idPrompt, strlen(idPrompt), 0);
    int readResult = recv(clientSocket, cust_id, sizeof(cust_id) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
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
        perror("Error opening transaction log");
        send(clientSocket, "Error opening transaction log", strlen("Error opening transaction log"), 0);
        return false;
    }

    // Notify client that transaction history is being fetched
    send(clientSocket, "Fetching transaction history...\n", strlen("Fetching transaction history...\n"), 0);

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
    {
        send(clientSocket, "No transactions found for this customer.\n", strlen("No transactions found for this customer.\n"), 0);
    }
    else
    {
        send(clientSocket, "End of transaction history.\n", strlen("End of transaction history.\n"), 0);
    }

    return transactionFound;
}

bool assignEmployeeToLoan(int clientSocket)
{
    char custId[10];

    // Receive the customer ID 
    char cidPrompt[] = "Enter Customer ID: ";
    send(clientSocket, cidPrompt, strlen(cidPrompt), 0);
    int readResult = recv(clientSocket, custId, sizeof(custId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
        return false;
    }
    custId[readResult] = '\0';
    if (custId[readResult - 1] == '\n')
    {
        custId[readResult - 1] = '\0';
    }

    char empId[10];

    // Receive the employee ID
    char eidPrompt[] = "Enter Employee ID: ";
    send(clientSocket, eidPrompt, strlen(eidPrompt), 0);
    readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
        return false;
    }
    empId[readResult] = '\0';
    if (empId[readResult - 1] == '\n')
    {
        empId[readResult - 1] = '\0';
    }

    FILE *loanFile;
    struct Loan loan;
    char line[512];
    int found = 0;

    // Open the loan database file
    loanFile = fopen(LOAN_DB, "r+");
    if (loanFile == NULL)
    {
        perror("Error opening loan database");
        return false;
    }

    // Lock the loan database file
    if (lockFile(loanFile) == -1)
    {
        fclose(loanFile);
        return false;
    }

    // Create a temporary file to store updated loans
    FILE *tempFile = fopen("temp_loan_db.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error opening temp file");
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
            found = 1;
            strcpy(tempLoan.assigned_emp_id, empId); // Assign employee to this loan
            printf("Assigned Employee ID: %s to Customer ID: %s\n", empId, custId);
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
        char *successMsg = "Employee successfully assigned to loan.\n";
        send(clientSocket, successMsg, strlen(successMsg), 0);
    }
    else
    {
        char *failMsg = "Customer loan not found.\n";
        send(clientSocket, failMsg, strlen(failMsg), 0);
    }
    return true;
}

bool processLoan(int clientSocket, const char *emp_id) {
    FILE *loanFile, *custFile;
    struct Loan loan;
    struct Customer customer;
    char line[512];
    char loan_id[10];
    char decision[10];
    bool loanFound = false;

    // Step 1: Ask employee for the loan they want to process
    char loanPrompt[] = "Enter the Customer ID for the loan you want to process: ";
    send(clientSocket, loanPrompt, strlen(loanPrompt), 0);
    int readResult = recv(clientSocket, loan_id, sizeof(loan_id) - 1, 0);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving loan ID", strlen("Error receiving loan ID"), 0);
        return false;
    }
    loan_id[readResult] = '\0';
    if (loan_id[readResult - 1] == '\n') {
        loan_id[readResult - 1] = '\0'; // Trim newline
    }

    // Open loan_db.txt to find the loan for the given customer
    loanFile = fopen(LOAN_DB, "r+");
    if (loanFile == NULL) {
        perror("Error opening loan database");
        return false;
    }

    // Lock the loan file for reading and writing
    if (flock(fileno(loanFile), LOCK_EX) == -1) {
        perror("Error locking the loan file");
        fclose(loanFile);
        return false;
    }

    // Step 2: Find the loan corresponding to the given Customer ID
    while (fgets(line, sizeof(line), loanFile) != NULL) {
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %s\n",
               loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);

        if (strcmp(loan.cust_id, loan_id) == 0 && strcmp(loan.assigned_emp_id, emp_id) == 0 && strcmp(loan.loan_status, "Pending") == 0) {
            loanFound = true;
            break;
        }
    }

    if (!loanFound) {
        send(clientSocket, "Loan not found or already processed", strlen("Loan not found or already processed"), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    // Step 3: Ask employee to approve or reject the loan
    char decisionPrompt[] = "Do you want to approve or reject the loan? (Enter 'approve' or 'reject'): ";
    send(clientSocket, decisionPrompt, strlen(decisionPrompt), 0);
    readResult = recv(clientSocket, decision, sizeof(decision) - 1, 0);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving decision", strlen("Error receiving decision"), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }
    decision[readResult] = '\0';
    if (decision[readResult - 1] == '\n') {
        decision[readResult - 1] = '\0'; // Trim newline
    }

    // Step 4: Process the decision
    if (strcmp(decision, "approve") == 0) {
        strcpy(loan.loan_status, "Approved");
    } else if (strcmp(decision, "reject") == 0) {
        strcpy(loan.loan_status, "Rejected");
    } else {
        send(clientSocket, "Invalid decision", strlen("Invalid decision"), 0);
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    // Step 5: Update loan_db.txt with the new loan status
    fseek(loanFile, 0, SEEK_SET);
    FILE *tempFile = fopen("temp_loan_db.txt", "w+");
    if (tempFile == NULL) {
        perror("Error creating temp loan database");
        flock(fileno(loanFile), LOCK_UN);
        fclose(loanFile);
        return false;
    }

    while (fgets(line, sizeof(line), loanFile) != NULL) {
        struct Loan tempLoan;
        sscanf(line, "CustomerID: %[^,], LoanAmount: %[^,], Status: %[^,], AssignedEmployee: %[^,], AppliedDate: %s\n",
               tempLoan.cust_id, tempLoan.loan_amount, tempLoan.loan_status, tempLoan.assigned_emp_id, tempLoan.applied_date);

        if (strcmp(tempLoan.cust_id, loan.cust_id) == 0) {
            // Update loan status
            fprintf(tempFile, "CustomerID: %s, LoanAmount: %s, Status: %s, AssignedEmployee: %s, AppliedDate: %s\n",
                    loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);
        } else {
            // Write the existing loan entry
            fprintf(tempFile, "%s", line);
        }
    }

    // Close files
    fclose(loanFile);
    fclose(tempFile);

    // Replace the old loan database with the updated one
    rename("temp_loan_db.txt", LOAN_DB);

    // Unlock the loan file
    flock(fileno(loanFile), LOCK_UN);

    // Step 6: Update customer loan details in customer_db.txt
    custFile = fopen(CUSTOMER_DB, "r+");
    if (custFile == NULL) {
        perror("Error opening customer database");
        return false;
    }

    // Lock the customer file for reading and writing
    if (flock(fileno(custFile), LOCK_EX) == -1) {
        perror("Error locking the customer database file");
        fclose(custFile);
        return false;
    }

    fseek(custFile, 0, SEEK_SET);
    FILE *tempCustFile = fopen("temp_customer_db.txt", "w+");
    if (tempCustFile == NULL) {
        perror("Error creating temp customer database");
        flock(fileno(custFile), LOCK_UN);
        fclose(custFile);
        return false;
    }

    while (fgets(line, sizeof(line), custFile) != NULL) {
        struct Customer tempCust;
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
               tempCust.loan_amount, tempCust.loan_status, tempCust.cust_account_balance);

        if (strcmp(tempCust.cust_id, loan.cust_id) == 0) {
            // Update customer loan status and loan amount
            strcpy(tempCust.loan_amount, loan.loan_amount);
            strcpy(tempCust.loan_status, loan.loan_status);
            // Assume we want to keep the existing account balance
        }

        // Write updated or existing customer details to temp file
        fprintf(tempCustFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, Account Balance: %s\n",
                tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
                tempCust.loan_amount, tempCust.loan_status, tempCust.cust_account_balance);
    }

    // Close files
    fclose(custFile);
    fclose(tempCustFile);

    // Replace the old customer database with the updated one
    rename("temp_customer_db.txt", CUSTOMER_DB);

    send(clientSocket, "Loan Processed Successfully", strlen("Loan Processed Successfully"), 0);
    return true;
}

bool viewFeedbacks(int client_socket) {
    FILE *feedbackFile;
    char line[256];

    // Open feedback database for reading
    feedbackFile = fopen(FEEDBACK_DB, "r");
    if (feedbackFile == NULL) {
        perror("Error opening feedback database");
        char *errorMessage = "Unable to retrieve feedbacks.\n";
        send(client_socket, errorMessage, strlen(errorMessage), 0);
        return false;
    }

    // Lock the feedback file for reading
    if (flock(fileno(feedbackFile), LOCK_SH) == -1) {
        perror("Error locking the feedback file");
        fclose(feedbackFile);
        return false;
    }

    // Send all feedbacks line by line
    while (fgets(line, sizeof(line), feedbackFile) != NULL) {
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

bool changePassword(int clientSocket, const char *username) {
    FILE *empFile = fopen(EMPLOYEESETAILS_DB, "r+");
    if (empFile == NULL) {
        perror("Error opening employee database");
        return false;
    }

    struct Employee employee;
    char line[512];
    bool userFound = false;

    // Lock the file for reading and writing
    int fd = fileno(empFile);
    if (flock(fd, LOCK_EX) == -1) {
        perror("Error locking the employee database");
        fclose(empFile);
        return false;
    }

    // Step 1: Find the employee by username
    while (fgets(line, sizeof(line), empFile) != NULL) {
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               employee.emp_name, employee.emp_username, employee.emp_password, employee.role, employee.emp_id, employee.active);

        if (strcmp(employee.emp_username, username) == 0) {
            userFound = true;
            break; // Break on finding the employee
        }
    }

    if (!userFound) {
        send(clientSocket, "Username not found.", strlen("Username not found."), 0);
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Step 2: Verify the current password
    char currentPassword[20];
    send(clientSocket, "Please enter your current password: ", strlen("Please enter your current password: "), 0);
    recv(clientSocket, currentPassword, sizeof(currentPassword) - 1, 0);
    currentPassword[strcspn(currentPassword, "\n")] = 0; // Remove newline character

    if (strcmp(employee.emp_password, currentPassword) != 0) {
        send(clientSocket, "Current password is incorrect.", strlen("Current password is incorrect."), 0);
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Step 3: Allow user to set a new password
    char newPassword[20];
    send(clientSocket, "Please enter your new password: ", strlen("Please enter your new password: "), 0);
    recv(clientSocket, newPassword, sizeof(newPassword) - 1, 0);
    newPassword[strcspn(newPassword, "\n")] = 0; // Remove newline character

    // Step 4: Update the password in the database
    fseek(empFile, 0, SEEK_SET); // Go back to the start of the file
    FILE *tempFile = fopen("temp_employee_db.txt", "w"); // Temporary file for updates
    if (tempFile == NULL) {
        perror("Error creating temporary file");
        flock(fd, LOCK_UN);
        fclose(empFile);
        return false;
    }

    // Write back all employees, updating the password for the user who requested it
    while (fgets(line, sizeof(line), empFile) != NULL) {
        struct Employee tempEmployee;
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmployee.emp_name, tempEmployee.emp_username, tempEmployee.emp_password, tempEmployee.role, tempEmployee.emp_id, tempEmployee.active);

        // If we found the matching employee, update their password
        if (strcmp(tempEmployee.emp_username, username) == 0) {
            // Use the new password here
            fprintf(tempFile, "Name: %s, Username: %s, Password: %s, Role: %s, ID: %s, Active: %s\n",
                    tempEmployee.emp_name, tempEmployee.emp_username, newPassword, tempEmployee.role, tempEmployee.emp_id, tempEmployee.active);
        } else {
            // Otherwise, write the original line
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(empFile);
    fclose(tempFile);

    // Replace the old file with the updated one
    remove(EMPLOYEESETAILS_DB); // Delete the old employee database
    rename("temp_employee_db.txt", EMPLOYEESETAILS_DB); // Rename the temporary file to the original file name

    send(clientSocket, "Password has been successfully updated.", strlen("Password has been successfully updated."), 0);
    flock(fd, LOCK_UN);
    return true;
}

#endif