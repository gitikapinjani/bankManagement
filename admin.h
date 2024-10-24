#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <time.h>

#include "entities.h"
#include "message.h" 

bool authenticateAdmin(int clientSocket);
bool addEmployee(int clientSocket);
bool modifyEmployee(int clientSocket);
void getAllEmployees(int clientSocket);
int getLastEmployeeId(FILE *file);

struct UserCredentials
{
    char username[20];
    char pass[20];
};

struct UserCredentials adminCredentials = {"gitika", "demo"};

bool adminOperationHandler(int clientSocket)
{
    if (authenticateAdmin(clientSocket))
    {
        send(clientSocket, LOGIN_SUCCESS, strlen(LOGIN_SUCCESS), 0);
        ssize_t writeBytes, readBytes;
        char readbuff[1000], writebuff[1000];
        while (1)
        {
            send(clientSocket, ADMIN_MENU, strlen(ADMIN_MENU), 0);
            readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
            if (readBytes == -1)
            {
                log_message("ERROR", ERROR_READING_ADMIN_MENU);
                return false;
            }

            int choice = atoi(readbuff);
            switch (choice)
            {
            case 1:
                if (addEmployee(clientSocket))
                    send(clientSocket, ADD_EMPLOYEE_SUCCESS, strlen(ADD_EMPLOYEE_SUCCESS), 0);
                break;
            case 2:
                if (modifyEmployee(clientSocket))
                    send(clientSocket, MODIFY_EMPLOYEE_SUCCESS, strlen(MODIFY_EMPLOYEE_SUCCESS), 0);
                break;
            case 3:
                getAllEmployees(clientSocket);
                send(clientSocket, GET_ALL_EMPLOYEE_SUCCESS, strlen(GET_ALL_EMPLOYEE_SUCCESS), 0);
                break;
            case 4:
                send(clientSocket, LOGOUT_SUCCESS, strlen(LOGOUT_SUCCESS), 0);
                return true;
            case 5:
                close(clientSocket);
                return true;
            default:
                send(clientSocket, INVALID_INPUT, strlen(INVALID_INPUT), 0);
                continue;
            }
        }
    }
    return false;
}

bool authenticateAdmin(int clientSocket)
{
    char username[20];
    char pass[20];

    // Send a prompt for the username
    send(clientSocket, ENTER_USERNAME, strlen(ENTER_USERNAME), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(clientSocket, username, sizeof(username), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    username[bytesRead - 1] = '\0';

    // Send a prompt for the password
    send(clientSocket, ENTER_PASSWORD, strlen(ENTER_PASSWORD), 0);
    // Receive the password from the client
    bytesRead = recv(clientSocket, pass, sizeof(pass), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    pass[bytesRead - 1] = '\0';

    // Compare the received username and password with stored credentials
    if (strcmp(username, adminCredentials.username) == 0 &&
        strcmp(pass, adminCredentials.pass) == 0)
    {
        return true;
    }
    else
    {
        send(clientSocket, AUTH_FAILURE, strlen(AUTH_FAILURE), 0);
        close(clientSocket);
        return false;
    }
}

bool addEmployee(int clientSocket)
{
    struct Employee emp;

    send(clientSocket, ENTER_EMP_NAME, strlen(ENTER_EMP_NAME), 0);
    int readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVE_EMP_NAME, strlen(ERROR_RECEIVE_EMP_NAME), 0);
        return false;
    }
    emp.emp_name[readResult - 1] = '\0';

    send(clientSocket, ENTER_EMP_USERNAME, strlen(ENTER_EMP_USERNAME), 0);
    readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVE_EMP_USERNAME, strlen(ERROR_RECEIVE_EMP_USERNAME), 0);
        return false;
    }
    emp.emp_username[readResult - 1] = '\0';

    send(clientSocket, ENTER_EMP_PASSWORD, strlen(ENTER_EMP_PASSWORD), 0);
    readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVE_EMP_PASSWORD, strlen(ERROR_RECEIVE_EMP_PASSWORD), 0);
        return false;
    }
    emp.emp_password[readResult - 1] = '\0';

    send(clientSocket, ENTER_EMP_ROLE, strlen(ENTER_EMP_ROLE), 0);
    char c[10];
    readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, ERROR_RECEIVE_EMP_ROLE, strlen(ERROR_RECEIVE_EMP_ROLE), 0);
        return false;
    }
    int choice = atoi(c);
    strcpy(emp.role, (choice == 1) ? "employee" : "manager");

    int employeeId = incrementIdCount("next_employee_id");
    if (employeeId == -1)
        return false; // Error occurred while incrementing ID

    sprintf(emp.emp_id, "%d", employeeId);
    strcpy(emp.active, "true");

    const char *dbFileName = "employee_db.txt";
    int dbFile = open(dbFileName, O_RDWR | O_APPEND);
    if (dbFile == -1)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return false;
    }

    if (flock(dbFile, LOCK_EX) == -1)
    {
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        close(dbFile);
        return false;
    }

    char empRecord[512];
    snprintf(empRecord, sizeof(empRecord), "Name: %s, Username: %s, Password: %s, Role: %s, ID: %s, Active: %s\n",
             emp.emp_name, emp.emp_username, emp.emp_password, emp.role, emp.emp_id, emp.active);

    ssize_t writeResult = write(dbFile, empRecord, strlen(empRecord));
    if (writeResult == -1)
    {
        send(clientSocket, ERROR_WRITE_DB_FILE, strlen(ERROR_WRITE_DB_FILE), 0);
        flock(dbFile, LOCK_UN);
        close(dbFile);
        return false;
    }

    if (flock(dbFile, LOCK_UN) == -1)
    {
        send(clientSocket, ERROR_UNLOCK_DB_FILE, strlen(ERROR_UNLOCK_DB_FILE), 0);
        close(dbFile);
        return false;
    }

    close(dbFile);
    return true;
}

void getAllEmployees(int clientSocket)
{
    int dbFile = open("employee_db.txt", O_RDONLY);
    if (dbFile == -1)
    {
        send(clientSocket, ERROR_OPEN_DB_FILE, strlen(ERROR_OPEN_DB_FILE), 0);
        return;
    }

    if (flock(dbFile, LOCK_SH) == -1)
    {
        close(dbFile);
        send(clientSocket, ERROR_LOCK_DB_FILE, strlen(ERROR_LOCK_DB_FILE), 0);
        return;
    }

    char buffer[256];
    ssize_t bytesRead;
    while ((bytesRead = read(dbFile, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        send(clientSocket, buffer, bytesRead, 0);
    }

    flock(dbFile, LOCK_UN);
    close(dbFile);
}

bool modifyEmployee(int clientSocket)
{
    struct Employee emp;
    char empId[10];

    // Send prompt for employee ID to modify
    send(clientSocket, ENTER_EMP_ID, strlen(ENTER_EMP_ID), 0);
    int readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
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

    // Open the database file for reading and writing
    FILE *dbFile = fopen(EMPLOYEESETAILS_DB, "r+");
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
        flock(fileno(dbFile), LOCK_UN); // Unlock the file before closing
        fclose(dbFile);
        return false;
    }

    // Read the file line by line, search for the employee by ID
    char line[512];
    bool employeeFound = false;

    while (fgets(line, sizeof(line), dbFile) != NULL)
    {
        struct Employee tempEmp;

        // Parse the employee details from the line (assuming the format is consistent)
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmp.emp_name, tempEmp.emp_username, tempEmp.emp_password, tempEmp.role, tempEmp.emp_id, tempEmp.active);

        // Compare the ID from the file with the one provided by the client
        if (strcmp(empId, tempEmp.emp_id) == 0)
        {
            employeeFound = true;

            // Now ask the client for the new details and modify them
            // Receive updated employee name
            send(clientSocket, ENTER_NEW_EMP_NAME, strlen(ENTER_NEW_EMP_NAME), 0);
            readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
            if (readResult > 0 && readResult != 1) // Ignore if just pressing enter
            {
                emp.emp_name[readResult] = '\0';
                if (emp.emp_name[readResult - 1] == '\n')
                {
                    emp.emp_name[readResult - 1] = '\0';
                }
                if (strlen(emp.emp_name) > 0)
                {
                    strcpy(tempEmp.emp_name, emp.emp_name);
                }
            }

            // Receive updated employee username
            send(clientSocket, ENTER_NEW_EMP_USERNAME, strlen(ENTER_NEW_EMP_USERNAME), 0);
            readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                emp.emp_username[readResult] = '\0';
                if (emp.emp_username[readResult - 1] == '\n')
                {
                    emp.emp_username[readResult - 1] = '\0';
                }
                if (strlen(emp.emp_username) > 0)
                {
                    strcpy(tempEmp.emp_username, emp.emp_username);
                }
            }

            // Receive updated employee password
            send(clientSocket, ENTER_NEW_EMP_PASSWORD, strlen(ENTER_NEW_EMP_PASSWORD), 0);
            readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                emp.emp_password[readResult] = '\0';
                if (emp.emp_password[readResult - 1] == '\n')
                {
                    emp.emp_password[readResult - 1] = '\0';
                }
                if (strlen(emp.emp_password) > 0)
                {
                    strcpy(tempEmp.emp_password, emp.emp_password);
                }
            }

            // Receive updated employee role
            send(clientSocket, ENTER_NEW_EMP_ROLE, strlen(ENTER_NEW_EMP_ROLE), 0);
            char c[10];
            readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
            if (readResult > 0 && readResult != 1)
            {
                c[readResult] = '\0';
                int choice = atoi(c);
                if (choice == 1)
                {
                    strcpy(tempEmp.role, "employee");
                }
                else if (choice == 2)
                {
                    strcpy(tempEmp.role, "manager");
                }
            }

            // Write the modified employee back to the temp file
            fprintf(tempFile, "Name: %s, Username: %s, Password: %s, Role: %s, ID: %s, Active: %s\n",
                    tempEmp.emp_name, tempEmp.emp_username, tempEmp.emp_password, tempEmp.role, tempEmp.emp_id, tempEmp.active);
        }
        else
        {
            // Write the unchanged employee to the temp file
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

    // If the employee was found, replace the original file with the modified file
    if (employeeFound)
    {
        rename("temp_db.txt", EMPLOYEESETAILS_DB); // Replace the original file with the temp file
        send(clientSocket, EMP_MODIFY_SUCCESS, strlen(EMP_MODIFY_SUCCESS), 0);
        return true;
    }
    else
    {
        send(clientSocket, EMP_NOT_FOUND, strlen(EMP_NOT_FOUND), 0);
        remove("temp_db.txt"); // Remove the temporary file
        return false;
    }
}

#endif