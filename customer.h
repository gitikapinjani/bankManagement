#ifndef CUSTOMER_H
#define CUSTOMER_H

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

#include "entities.h"
#include "employee.h"

bool authenticateCustomer(int clientSocket, struct Customer *authenticatedCustomer);
bool viewBalance(int clientSocket, const char *cust_id);
bool transferMoney(int clientSocket, const char *senderId);
bool viewTransaction(int clientSocket, const char *cust_id);
bool transferForFixedDeposit(int clientSocket, const char *customer_id);
void writeBankDetails(struct BankDetails *bankDetails);
bool updateCustomerInDB(struct Customer updatedCustomer);
void readBankDetails(struct BankDetails *bankDetails);
void addAmounts(char *result, const char *amount1, const char *amount2);
bool processLoanApplication(int clientSocket, char *cust_id);
bool sendFeedback(int socket, const char *customerId);
bool changePasswordForCustomer(int clientSocket, const char *username);

bool customerOperationHandler(int clientSocket)
{
    struct Customer authenticatedCust;

    if (authenticateCustomer(clientSocket, &authenticatedCust))
    {
        log_message("INFO", authenticatedCust.cust_name);
        send(clientSocket, "Login Successfully\n", strlen("Login Successfully\n"), 0);
        ssize_t writeBytes, readBytes;        // Number of bytes read from / written to the client
        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client
        while (1)
        {
            char leMenu[] = "Do you want to:\n - 1.view balance\n - 2.Transfer Money\n - 3. open fixed deposite \n - 4. apply for loan \n - 5. add feedback\n - 6. view transaction history\n - 7. Change Password\n - 8. logout\n - 9. Exit from this menu\n";
            send(clientSocket, leMenu, strlen(leMenu), 0);
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
                if (viewBalance(clientSocket, authenticatedCust.cust_id))
                {
                    send(clientSocket, "Successfully added the employee\n", strlen("Successfully added the employee\n"), 0);
                }
                break;
            case 2:
                if (transferMoney(clientSocket, authenticatedCust.cust_id))
                {
                    send(clientSocket, "Successfully modified employee!!\n", strlen("Successfully modified employee!!\n"), 0);
                }
                break;
            case 3:
                if (transferForFixedDeposit(clientSocket, authenticatedCust.cust_id))
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                break;
            case 4:
                if (processLoanApplication(clientSocket, authenticatedCust.cust_id))
                    ;
                send(clientSocket, "Successfully deactivate the faculty!!\n", strlen("Successfully deactivate the faculty!!\n"), 0);
                break;

            case 5:
                if (sendFeedback(clientSocket, authenticatedCust.cust_id))
                    send(clientSocket, "Successfully updated the student!!\n", strlen("Successfully updated the student!!\n"), 0);
                break;
            case 6:
                if (viewTransaction(clientSocket, authenticatedCust.cust_id))
                {
                    send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                }
                break;
            case 7:
                if (changePasswordForCustomer(clientSocket, authenticatedCust.cust_username))
                    send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
                break;
            case 8:
                send(clientSocket, "Successfully logout!!\n", strlen("Successfully logout!!\n"), 0);
                if (customerOperationHandler(clientSocket))
                    send(clientSocket, "Successfully completed!!\n", strlen("Successfully completed!!\n"), 0);
                break;
            case 9:
                close(clientSocket);
                break;
            default:
                send(clientSocket, "wrong Input...Choose again\n", strlen("wrong Input...Choose again\n"), 0);
                continue;
                break;
            }
        }
    }
    return false;
}
bool authenticateCustomer(int clientSocket, struct Customer *authenticatedCustomer)
{
    const char *dbFileName = "customer_db.txt";
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
        struct Customer tempCust;
        char active[10];

        // Parse the line to extract customer data (including username and password)
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active, tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        // Compare the username and password with the data in the file
        if (strcmp(username, tempCust.cust_username) == 0 && strcmp(password, tempCust.cust_password) == 0)
        {
            log_message("INFO", "successfully authenticated");
            *authenticatedCustomer = tempCust;
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

bool viewBalance(int clientSocket, const char *cust_id)
{
    const char *dbFileName = "customer_db.txt";
    FILE *dbFile = fopen(dbFileName, "r");

    if (dbFile == NULL)
    {
        perror("Error opening the customer database file");
        send(clientSocket, "Error opening the customer database", strlen("Error opening the customer database"), 0);
        return false;
    }

    // Lock the file for reading
    if (flock(fileno(dbFile), LOCK_SH) == -1)
    {
        perror("Error locking the file for reading");
        fclose(dbFile);
        send(clientSocket, "Error locking the customer database", strlen("Error locking the customer database"), 0);
        return false;
    }

    char line[512];
    bool found = false;
    char balance[50];

    // Read the file line by line to find the customer ID
    while (fgets(line, sizeof(line), dbFile) != NULL)
    {
        struct Customer tempCust;

        // Parse the line to extract customer data
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active, tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        // Compare the customer ID
        if (strcmp(cust_id, tempCust.cust_id) == 0)
        {
            // If customer is found, get the balance
            strcpy(balance, tempCust.cust_account_balance);
            found = true;
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

    // Check if the customer was found and send the balance to the client
    if (found)
    {
        char message[100];
        sprintf(message, "Your account balance is: %s\n", balance);
        send(clientSocket, message, strlen(message), 0);
        return true;
    }
    else
    {
        send(clientSocket, "Customer ID not found\n", strlen("Customer ID not found\n"), 0);
        return false;
    }
}

bool transferMoney(int clientSocket, const char *sender_id)
{
    char receiver_id[10];

    // Receive customer ID from the client
    char idPrompt[] = "Enter customer ID: ";
    send(clientSocket, idPrompt, strlen(idPrompt), 0);
    int readResult = recv(clientSocket, receiver_id, sizeof(receiver_id) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
        return false;
    }
    receiver_id[readResult] = '\0';
    if (receiver_id[readResult - 1] == '\n')
    {
        receiver_id[readResult - 1] = '\0';
    }

    char amount[10];
    // Receive transfer amount from the client
    char balancePrompt[] = "Enter amount to transfer: ";
    send(clientSocket, balancePrompt, strlen(balancePrompt), 0);
    readResult = recv(clientSocket, amount, sizeof(amount) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Customer ID from client", strlen("Error receiving Customer ID from client"), 0);
        return false;
    }
    amount[readResult] = '\0';
    if (amount[readResult - 1] == '\n')
    {
        amount[readResult - 1] = '\0';
    }

    FILE *custFile, *transFile;
    struct Customer sender, receiver;
    bool senderFound = false, receiverFound = false;
    char line[512];
    char customerData[100][512]; // Stores each customer entry for later rewriting to file
    int customerCount = 0;

    // Open the customer database file and lock it for reading and writing
    custFile = fopen(CUSTOMER_DB, "r+");
    if (custFile == NULL)
    {
        perror("Error opening customer database");
        return false;
    }

    // Lock the file
    if (flock(fileno(custFile), LOCK_EX) == -1)
    {
        perror("Error locking the customer database file");
        fclose(custFile);
        return false;
    }

    // Read the entire file into memory and look for the sender and receiver
    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        struct Customer tempCust;
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
               tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        // Store the current customer entry in memory
        strcpy(customerData[customerCount++], line);

        // Check if this customer is the sender or receiver
        if (strcmp(tempCust.cust_id, sender_id) == 0)
        {
            sender = tempCust;
            senderFound = true;
        }
        if (strcmp(tempCust.cust_id, receiver_id) == 0)
        {
            receiver = tempCust;
            receiverFound = true;
        }
    }

    int transferAmount = atoi(amount);

    // Convert the account balances from strings to float
    float senderBalance = atof(sender.cust_account_balance);
    float receiverBalance = atof(receiver.cust_account_balance);

    // Check if both customers are found and sender has sufficient balance
    if (!senderFound || !receiverFound || senderBalance < transferAmount)
    {
        flock(fileno(custFile), LOCK_UN);
        fclose(custFile);
        send(clientSocket, "Transaction Failed", strlen("Transaction Failed"), 0);
        return false;
    }

    // Deduct amount from sender, add to receiver
    senderBalance -= transferAmount;
    receiverBalance += transferAmount;

    // Convert the updated balances back to strings
    snprintf(sender.cust_account_balance, sizeof(sender.cust_account_balance), "%.2f", senderBalance);
    snprintf(receiver.cust_account_balance, sizeof(receiver.cust_account_balance), "%.2f", receiverBalance);

    // Now rewrite the customer data with the updated balances
    freopen(CUSTOMER_DB, "w", custFile); // Reopen the file in write mode to overwrite it
    for (int i = 0; i < customerCount; i++)
    {
        struct Customer tempCust;
        sscanf(customerData[i], "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
               tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        if (strcmp(tempCust.cust_id, sender.cust_id) == 0)
        {
            // Update sender details
            fprintf(custFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
                    sender.cust_id, sender.cust_name, sender.cust_username, sender.cust_password, sender.active,
                    sender.loan_amount, sender.loan_status, sender.cust_fd_amount, sender.cust_account_balance);
        }
        else if (strcmp(tempCust.cust_id, receiver.cust_id) == 0)
        {
            // Update receiver details
            fprintf(custFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
                    receiver.cust_id, receiver.cust_name, receiver.cust_username, receiver.cust_password, receiver.active,
                    receiver.loan_amount, receiver.loan_status, receiver.cust_fd_amount, receiver.cust_account_balance);
        }
        else
        {
            // Write other customers unchanged
            fputs(customerData[i], custFile);
        }
    }

    // Unlock and close the customer file
    flock(fileno(custFile), LOCK_UN);
    fclose(custFile);

    // Log the transaction
    transFile = fopen(TRANSACTION_DB, "a");
    if (transFile == NULL)
    {
        perror("Error opening transaction log");
        return false;
    }

    // Lock the transaction file
    if (flock(fileno(transFile), LOCK_EX) == -1)
    {
        perror("Error locking the transaction log file");
        fclose(transFile);
        return false;
    }

    // Get the current date and time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dateStr[20];
    sprintf(dateStr, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Write the transaction details
    fprintf(transFile, "Sender: %s, Receiver: %s, Amount: %s, Date: %s\n", sender.cust_id, receiver.cust_id, amount, dateStr);

    // Unlock and close the transaction file
    flock(fileno(transFile), LOCK_UN);
    fclose(transFile);

    // Notify the client of a successful transaction
    send(clientSocket, "Transaction Successful", strlen("Transaction Successful"), 0);

    return true;
}

bool viewTransaction(int clientSocket, const char *cust_id)
{

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

bool transferForFixedDeposit(int clientSocket, const char *customer_id)
{
    char amount[10];
    char balancePrompt[] = "Enter amount for fixed deposit: ";

    // Send prompt to client for amount
    send(clientSocket, balancePrompt, strlen(balancePrompt), 0);

    // Receive amount for fixed deposit
    int readResult = recv(clientSocket, amount, sizeof(amount) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving amount from client", strlen("Error receiving amount from client"), 0);
        return false;
    }
    amount[readResult] = '\0'; // Null terminate the received string

    // Open customer database file
    FILE *custFile = fopen(CUSTOMER_DB, "r");
    if (custFile == NULL)
    {
        perror("Error opening customer database");
        return false;
    }

    struct Customer customer;
    bool customerFound = false;
    char line[512];

    // Find the customer in the database
    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               customer.cust_id, customer.cust_name, customer.cust_username, customer.cust_password, customer.active,
               customer.loan_amount, customer.loan_status, customer.cust_fd_amount, customer.cust_account_balance);

        if (strcmp(customer.cust_id, customer_id) == 0)
        {
            customerFound = true;
            break;
        }
    }

    // Close the read file
    fclose(custFile);

    if (!customerFound)
    {
        send(clientSocket, "Customer not found", strlen("Customer not found"), 0);
        return false;
    }

    // Convert account balance to float
    float accountBalance = atof(customer.cust_account_balance);
    float fdAmount = atof(amount);

    // Check if the customer has sufficient balance
    if (accountBalance < fdAmount)
    {
        send(clientSocket, "Insufficient balance for fixed deposit", strlen("Insufficient balance for fixed deposit"), 0);
        return false;
    }

    // Deduct the amount from account balance and add to fixed deposit
    accountBalance -= fdAmount;
    snprintf(customer.cust_account_balance, sizeof(customer.cust_account_balance), "%.2f", accountBalance);

    float existingFDAmount = atof(customer.cust_fd_amount);
    existingFDAmount += fdAmount;
    snprintf(customer.cust_fd_amount, sizeof(customer.cust_fd_amount), "%.2f", existingFDAmount);

    // Update the customer details in the database
    updateCustomerInDB(customer);

    // Read and update the bank details
    struct BankDetails bankDetails;
    readBankDetails(&bankDetails);
    float totalBankFunds = atof(bankDetails.totalBankFunds) + fdAmount;
    snprintf(bankDetails.totalBankFunds, sizeof(bankDetails.totalBankFunds), "%.2f", totalBankFunds);
    // No changes to totalBankLoan in this transaction

    addAmounts(bankDetails.totalBankFunds, bankDetails.totalBankFunds, amount);
    // Write updated bank details to the file
    writeBankDetails(&bankDetails);

    // Notify the client that the transaction was successful
    send(clientSocket, "Fixed deposit successful", strlen("Fixed deposit successful"), 0);

    return true;
}

bool updateCustomerInDB(struct Customer updatedCustomer)
{
    FILE *custFile = fopen(CUSTOMER_DB, "r");
    if (custFile == NULL)
    {
        perror("Error opening customer database for reading");
        return false;
    }

    // Lock the file for exclusive access
    if (flock(fileno(custFile), LOCK_EX) == -1)
    {
        perror("Error locking the customer database file");
        fclose(custFile);
        return false;
    }

    // Read the entire customer database into memory
    struct Customer customers[100]; // Assuming we have less than 100 customers
    int customerCount = 0;
    char line[512];

    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        struct Customer tempCust;
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], FD Amount: %[^,], Account Balance: %s\n",
               tempCust.cust_id, tempCust.cust_name, tempCust.cust_username, tempCust.cust_password, tempCust.active,
               tempCust.loan_amount, tempCust.loan_status, tempCust.cust_fd_amount, tempCust.cust_account_balance);

        // Replace the customer details with the updated one if the IDs match
        if (strcmp(tempCust.cust_id, updatedCustomer.cust_id) == 0)
        {
            customers[customerCount] = updatedCustomer;
        }
        else
        {
            customers[customerCount] = tempCust;
        }
        customerCount++;
    }

    // Close the read file
    fclose(custFile);

    // Open the customer database for writing and rewrite all customer details
    custFile = fopen(CUSTOMER_DB, "w");
    if (custFile == NULL)
    {
        perror("Error opening customer database for writing");
        return false;
    }

    // Write updated customer details back to file
    for (int i = 0; i < customerCount; i++)
    {
        fprintf(custFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, FD Amount: %s, Account Balance: %s\n",
                customers[i].cust_id, customers[i].cust_name, customers[i].cust_username, customers[i].cust_password,
                customers[i].active, customers[i].loan_amount, customers[i].loan_status, customers[i].cust_fd_amount, customers[i].cust_account_balance);
    }

    // Unlock the file and close it
    flock(fileno(custFile), LOCK_UN);
    fclose(custFile);

    return true;
}

void readBankDetails(struct BankDetails *bankDetails)
{
    FILE *bankFile = fopen(BANKDETAILS_DB, "r");
    if (bankFile == NULL)
    {
        perror("Error opening bank details file");
        return;
    }

    // Read the details from the file
    fscanf(bankFile, "TotalBankFunds: %s\nTotalLoanTaken: %s\n", bankDetails->totalBankFunds, bankDetails->totalLoanTaken);

    fclose(bankFile);
}

void writeBankDetails(struct BankDetails *bankDetails)
{
    FILE *bankFile = fopen(BANKDETAILS_DB, "w");
    if (bankFile == NULL)
    {
        perror("Error opening bank details file for writing");
        return;
    }

    // Write updated bank details back to the file
    fprintf(bankFile, "TotalBankFunds: %s\nTotalLoanTaken: %s\n", bankDetails->totalBankFunds, bankDetails->totalLoanTaken);

    fclose(bankFile);
}

void addAmounts(char *result, const char *amount1, const char *amount2)
{
    float value1 = atof(amount1);
    float value2 = atof(amount2);
    sprintf(result, "%.2f", value1 + value2);
}

bool processLoanApplication(int clientSocket, char *cust_id)
{
    char loanAmount[10];
    char balancePrompt[] = "Enter amount for loan: ";

    // Send prompt to client for amount
    send(clientSocket, balancePrompt, strlen(balancePrompt), 0);

    // Receive amount for loan
    int readResult = recv(clientSocket, loanAmount, sizeof(loanAmount) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving amount from client", strlen("Error receiving amount from client"), 0);
        return false;
    }
    loanAmount[readResult] = '\0'; // Null terminate the received string

    // Trim the newline character if it exists
    size_t len = strlen(loanAmount);
    if (len > 0 && loanAmount[len - 1] == '\n')
    {
        loanAmount[len - 1] = '\0';
    }

    FILE *loanFile;
    struct Loan loan;
    char dateStr[20];

    // Open the loan database for appending
    loanFile = fopen(LOAN_DB, "a+");
    if (loanFile == NULL)
    {
        perror("Error opening loan database");
        return false;
    }

    // Lock the file
    if (lockFile(loanFile) == -1)
    {
        fclose(loanFile);
        return false;
    }

    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Fill the loan structure
    strcpy(loan.cust_id, cust_id);
    strcpy(loan.loan_amount, loanAmount);
    strcpy(loan.loan_status, "Pending");
    strcpy(loan.assigned_emp_id, "None");
    strcpy(loan.applied_date, dateStr);

    // Write loan to the file without adding extra newline
    fprintf(loanFile, "CustomerID: %s, LoanAmount: %s, Status: %s, AssignedEmployee: %s, AppliedDate: %s\n",
            loan.cust_id, loan.loan_amount, loan.loan_status, loan.assigned_emp_id, loan.applied_date);

    // Unlock and close file
    unlockFile(loanFile);
    fclose(loanFile);

    // Send confirmation to client
    char *message = "Loan application received.\n";
    send(clientSocket, message, strlen(message), 0);
    return true;
}

bool sendFeedback(int clientSocket, const char *customerId)
{
    char feedbackMessage[100];

    // Receive feedback from the client
    char feedbackPrompt[] = "Enter Feedback: ";
    send(clientSocket, feedbackPrompt, strlen(feedbackPrompt), 0);
    int readResult = recv(clientSocket, feedbackMessage, sizeof(feedbackMessage) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving feedback from client", strlen("Error receiving feedback from client"), 0);
        return false;
    }
    feedbackMessage[readResult] = '\0';
    if (feedbackMessage[readResult - 1] == '\n')
    {
        feedbackMessage[readResult - 1] = '\0';
    }

    // Open feedback database for appending
    FILE *feedbackFile = fopen(FEEDBACK_DB, "a+");
    if (feedbackFile == NULL)
    {
        perror("Error opening feedback database");
        return false;
    }

    // Lock the file for writing
    if (lockFile(feedbackFile) == -1)
    {
        perror("Error locking feedback file");
        fclose(feedbackFile);
        return false;
    }

    // Get the current date
    char dateStr[20];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Write feedback to the file
    fprintf(feedbackFile, "CustomerID: %s, Feedback: %s, Date: %s\n",
            customerId, feedbackMessage, dateStr);

    // Unlock and close the file
    unlockFile(feedbackFile);
    fclose(feedbackFile);

    // Send confirmation to client
    char *confirmationMessage = "Feedback submitted successfully.\n";
    send(clientSocket, confirmationMessage, strlen(confirmationMessage), 0);
    return true;
}

bool changePasswordForCustomer(int clientSocket, const char *username)
{
    FILE *custFile = fopen(CUSTOMER_DB, "r+");
    if (custFile == NULL)
    {
        perror("Error opening customer database");
        return false;
    }

    struct Customer customer;
    char line[512];
    bool userFound = false;

    // Lock the file for reading and writing
    int fd = fileno(custFile);
    if (flock(fd, LOCK_EX) == -1)
    {
        perror("Error locking the customer database");
        fclose(custFile);
        return false;
    }

    // Step 1: Find the customer by username
    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], Account Balance: %s\n",
               customer.cust_id, customer.cust_name, customer.cust_username, customer.cust_password, customer.active,
               customer.loan_amount, customer.loan_status, customer.cust_account_balance);

        if (strcmp(customer.cust_username, username) == 0)
        {
            userFound = true;
            break; // Break on finding the user
        }
    }

    if (!userFound)
    {
        send(clientSocket, "Username not found.", strlen("Username not found."), 0);
        flock(fd, LOCK_UN);
        fclose(custFile);
        return false;
    }

    // Step 2: Verify the current password
    char currentPassword[20];
    send(clientSocket, "Please enter your current password: ", strlen("Please enter your current password: "), 0);
    recv(clientSocket, currentPassword, sizeof(currentPassword) - 1, 0);
    currentPassword[strcspn(currentPassword, "\n")] = 0; // Remove newline character

    if (strcmp(customer.cust_password, currentPassword) != 0)
    {
        send(clientSocket, "Current password is incorrect.", strlen("Current password is incorrect."), 0);
        flock(fd, LOCK_UN);
        fclose(custFile);
        return false;
    }

    // Step 3: Allow user to set a new password
    char newPassword[20];
    send(clientSocket, "Please enter your new password: ", strlen("Please enter your new password: "), 0);
    recv(clientSocket, newPassword, sizeof(newPassword) - 1, 0);
    newPassword[strcspn(newPassword, "\n")] = 0; // Remove newline character

    // Step 4: Update the password in the database
    fseek(custFile, 0, SEEK_SET);                        // Go back to the start of the file
    FILE *tempFile = fopen("temp_customer_db.txt", "w"); // Temporary file for updates
    if (tempFile == NULL)
    {
        perror("Error creating temporary file");
        flock(fd, LOCK_UN);
        fclose(custFile);
        return false;
    }

    // Write back all customers, updating the password for the user who requested it
    while (fgets(line, sizeof(line), custFile) != NULL)
    {
        struct Customer tempCustomer;
        sscanf(line, "ID: %[^,], Name: %[^,], Username: %[^,], Password: %[^,], Active: %[^,], Loan Amount: %[^,], Loan Status: %[^,], Account Balance: %s\n",
               tempCustomer.cust_id, tempCustomer.cust_name, tempCustomer.cust_username, tempCustomer.cust_password, tempCustomer.active,
               tempCustomer.loan_amount, tempCustomer.loan_status, tempCustomer.cust_account_balance);

        // If we found the matching customer, update their password
        if (strcmp(tempCustomer.cust_username, username) == 0)
        {
            fprintf(tempFile, "ID: %s, Name: %s, Username: %s, Password: %s, Active: %s, Loan Amount: %s, Loan Status: %s, Account Balance: %s\n",
                    tempCustomer.cust_id, tempCustomer.cust_name, tempCustomer.cust_username, newPassword, tempCustomer.active,
                    tempCustomer.loan_amount, tempCustomer.loan_status, tempCustomer.cust_account_balance);
        }
        else
        {
            // Otherwise, write the original line
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(custFile);
    fclose(tempFile);

    // Replace the old file with the updated one
    remove(CUSTOMER_DB);                         // Delete the old customer database
    rename("temp_customer_db.txt", CUSTOMER_DB); // Rename the temporary file to the original file name

    send(clientSocket, "Password has been successfully updated.", strlen("Password has been successfully updated."), 0);
    flock(fd, LOCK_UN);
    return true;
}

#endif