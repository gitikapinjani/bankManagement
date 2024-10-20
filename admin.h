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
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <time.h>

#include "customer.h"
#include "entities.h"
#include "employee.h"

bool authenticateAdmin(int clientSocket);
bool addEmployee(int clientSocket);
bool modifyEmployee(int clientSocket);
void getAllEmployees(int clientSocket);
int getLastEmployeeId(FILE *file);
void log_message(const char *level, const char *message);


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
        send(clientSocket, "Login Successfully\n", strlen("Login Successfully\n"), 0);
        ssize_t writeBytes, readBytes;        // Number of bytes read from / written to the client
        char readbuff[1000], writebuff[1000]; // A buffer used for reading & writing to the client
        while (1)
        {
            char leMenu[] = "Do you want to:\n - 1.Add Employee\n - 2.Modify Employee Detail\n - 3.Get All Employee Detail \n - 4. Manage User Role\n - 5. Change Password\n - 6. logout\n - 7.Exit from this menu\n";
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
                if (addEmployee(clientSocket))
                {
                    send(clientSocket, "Successfully added the employee\n", strlen("Successfully added the employee\n"), 0);
                }
                break;
            case 2:
                if (modifyEmployee(clientSocket))
                {
                    send(clientSocket, "Successfully modified employee!!\n", strlen("Successfully modified employee!!\n"), 0);
                }
                break;

            case 3:
                    getAllEmployees(clientSocket);
                    send(clientSocket, "Successfully displayed all employee!!\n", strlen("Successfully displayed all employee!!\n"), 0);
                break;
            // case 4:
            //     if (deactivate_faculty(clientSocket))
            //     {
            //         send(clientSocket, "Successfully deactivate the faculty!!\n", strlen("Successfully deactivate the faculty!!\n"), 0);
            //     }
            //     else
            //     {
            //         send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
            //     }
            //     break;

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
            // case 6:
            //     if (update_faculty(clientSocket))
            //     {
            //         send(clientSocket, "Successfully updated the faculty!!\n", strlen("Successfully updated the faculty!!\n"), 0);
            //     }
            //     else
            //     {
            //         send(clientSocket, "Please try again....\n", strlen("Please try again....\n"), 0);
            //     }
            //     break;
            case 7:
                close(clientSocket);
                break;
            default:
                send(clientSocket, "Sorry...Had to logout", strlen("Sorry...Had to logout"), 0);
                close(clientSocket);
                return false;
            }
        }
    }
}
bool authenticateAdmin(int clientSocket)
{
    char username[20];
    char pass[20];

    // Send a prompt for the username
    const char *userPrompt = "Enter username: ";
    send(clientSocket, userPrompt, strlen(userPrompt), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(clientSocket, username, sizeof(username), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (username[bytesRead - 1] == '\n')
    {
        username[bytesRead - 1] = '\0';
    }
    else
    {
        username[bytesRead] = '\0';
    }

    // Send a prompt for the password
    const char *passPrompt = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);
    // Receive the password from the client
    bytesRead = recv(clientSocket, pass, sizeof(pass), 0);
    if (bytesRead <= 0)
    {
        close(clientSocket);
        return false;
    }
    if (pass[bytesRead - 1] == '\n')
    {
        pass[bytesRead - 1] = '\0';
    }
    else
    {
        pass[bytesRead] = '\0';
    }

    // Compare the received username and password with stored credentials
    if (strcmp(username, adminCredentials.username) == 0 &&
        strcmp(pass, adminCredentials.pass) == 0)
    {
        // Authentication successful
        const char *successMessage = "Authentication successful";
        send(clientSocket, successMessage, strlen(successMessage), 0);
        return true;
    }
    else
    {
        // Authentication failed
        const char *failureMessage = "Authentication failed";
        send(clientSocket, failureMessage, strlen(failureMessage), 0);
        close(clientSocket);
        return false;
    }
}

// bool addEmployee(int clientSocket)
// {
//     struct Employee emp;

//     // Receive employee name from the client
//     char namePrompt[] = "Enter Employee name: ";
//     send(clientSocket, namePrompt, strlen(namePrompt), 0);
//     int readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee name from client", strlen("Error receiving Employee name from client"), 0);
//         return false;
//     }
//     emp.emp_name[readResult] = '\0';
//     if (emp.emp_name[readResult - 1] == '\n')
//     {
//         emp.emp_name[readResult - 1] = '\0';
//     }

//     // Receive employee userName from the client
//     char usernamePrompt[] = "Enter Employee userName: ";
//     send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
//     readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee username from client", strlen("Error receiving Employee username from client"), 0);
//         return false;
//     }
//     emp.emp_username[readResult] = '\0';
//     if (emp.emp_username[readResult - 1] == '\n')
//     {
//         emp.emp_username[readResult - 1] = '\0';
//     }

//     // Receive employee password from the client
//     char passPrompt[] = "Enter password: ";
//     send(clientSocket, passPrompt, strlen(passPrompt), 0);
//     readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee password from client", strlen("Error receiving Employee password from client"), 0);
//         return false;
//     }
//     emp.emp_password[readResult] = '\0';
//     if (emp.emp_password[readResult - 1] == '\n')
//     {
//         emp.emp_password[readResult - 1] = '\0';
//     }

//     // Receive employee role from the client
//     char rolePrompt[] = "Enter role of employee - \n1. employee \n2. manager \n";
//     send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
//     char c[10];
//     readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee role from client", strlen("Error receiving Employee role from client"), 0);
//         return false;
//     }
//     int choice = atoi(c);
//     switch (choice)
//     {
//     case 1:
//         strcpy(emp.role, "employee");
//         break;
//     case 2:
//         strcpy(emp.role, "manager");
//         break;
//     }

//     // Receive employee Id from the client
//     char idPrompt[] = "Enter employeeId : \n";
//     send(clientSocket, idPrompt, strlen(idPrompt), 0);
//     readResult = recv(clientSocket, emp.emp_id, sizeof(emp.emp_id) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee role from client", strlen("Error receiving Employee role from client"), 0);
//         return false;
//     }

//         emp.emp_id[readResult] = '\0';
//     if (emp.emp_id[readResult - 1] == '\n')
//     {
//         emp.emp_id[readResult - 1] = '\0';
//     }
//     // Set the employee ID and active status (these could be dynamically generated)
//     // Example, you could generate this dynamically
//     strcpy(emp.active, "true");

//     const char *dbFileName = "employee_db.txt";
//     // Open the database file
//     int dbFile = open(dbFileName, O_RDWR | O_APPEND);
//     if (dbFile == -1)
//     {
//         perror("Error opening or creating the database file");
//         return false;
//     }

//     // Lock the file for writing
//     if (flock(dbFile, LOCK_EX) == -1)
//     {
//         perror("Error locking the file");
//         close(dbFile);
//         return false;
//     }

//     // Write the employee information to the file
//     ssize_t writeResult = write(dbFile, &emp, sizeof(struct Employee));
//     write(dbFile, "\n", 1);  // Add newline after each employee record
//     if (writeResult == -1)
//     {
//         perror("Error writing employee record to database file");
//         flock(dbFile, LOCK_UN); // Unlock the file before closing
//         close(dbFile);
//         return false;
//     }

//     // Unlock the file
//     if (flock(dbFile, LOCK_UN) == -1)
//     {
//         perror("Error unlocking the file");
//         close(dbFile);
//         return false;
//     }

//     // Close the database file
//     close(dbFile);

//     return true;
// }


// // bool addEmployee(int clientSocket)
// // {
// //     struct Employee emp;

// //     // Receive employee name from the client
// //     char namePrompt[] = "Enter Employee name: ";
// //     send(clientSocket, namePrompt, strlen(namePrompt), 0);
// //     int readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
// //     if (readResult <= 0)
// //     {
// //         send(clientSocket, "Error receiving Employee name from client", strlen("Error receiving Employee name from client"), 0);
// //         return false;
// //     }
// //     emp.emp_name[readResult] = '\0';
// //     if (emp.emp_name[readResult - 1] == '\n')
// //     {
// //         emp.emp_name[readResult - 1] = '\0';
// //     }

// //     // Receive employee userName from the client
// //     char usernamePrompt[] = "Enter Employee userName: ";
// //     send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
// //     readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
// //     if (readResult <= 0)
// //     {
// //         send(clientSocket, "Error receiving Employee username from client", strlen("Error receiving Employee username from client"), 0);
// //         return false;
// //     }
// //     emp.emp_username[readResult] = '\0';
// //     if (emp.emp_username[readResult - 1] == '\n')
// //     {
// //         emp.emp_username[readResult - 1] = '\0';
// //     }

// //     // Receive employee password from the client
// //     char passPrompt[] = "Enter password: ";
// //     send(clientSocket, passPrompt, strlen(passPrompt), 0);
// //     readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
// //     if (readResult <= 0)
// //     {
// //         send(clientSocket, "Error receiving faculty password from client", strlen("Error receiving faculty password from client"), 0);
// //         return false;
// //     }
// //     emp.emp_password[readResult] = '\0';
// //     if (emp.emp_password[readResult - 1] == '\n')
// //     {
// //         emp.emp_password[readResult - 1] = '\0';
// //     }

// //     // Receive employee role from the client
// //     char rolePrompt[] = "Enter role of employee - \n 1. employee \n2. manager: \n";
// //     send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
// //     char c[10];
// //     readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
// //     log_message("INFO", c);
// //     log_message("INFO", "hello");
// //     if (readResult <= 0)
// //     {
// //         send(clientSocket, "Error receiving faculty password from client", strlen("Error receiving faculty password from client"), 0);
// //         return false;
// //     }
// //     int choice = atoi(c);
// //     switch (choice)
// //     {
// //     case 1:
// //         strcpy(emp.role, "employee");
// //         break;
// //     case 2:
// //         strcpy(emp.role, "manager");
// //         break;
// //     }
// //     // emp.emp_password[readResult] = '\0';
// //     // if (emp.emp_password[readResult - 1] == '\n')
// //     // {
// //     //     emp.emp_password[readResult - 1] = '\0';
// //     // }
// //     // strcpy(emp.emp_name, "bhavya");
// //     // strcpy(emp.emp_username, "bhavya");
// //     // strcpy(emp.emp_password, "demo");
// //     // strcpy(emp.role, "employee");
// //     const char *dbFileName = "employee_db.txt";
// //     log_message("INFO", "hello abc");
// //     // FILE *file = fopen("employee_db.txt", "rb");
// //     // if (file == NULL)
// //     // {
// //     //     fprintf(stderr, "Failed to open file.\n");
// //     //     return 1;
// //     // }
// //     // // int lastEmployeeId = getLastEmployeeId(file);
// //     // fclose(file);
// //     // log_message("INFO", atoi(lastEmployeeId));
// //     strcpy(emp.emp_id, "a1");
// //     strcpy(emp.active, "true");
// //     // Open the database file
// //     int dbFile = open(dbFileName, O_RDWR);
// //     log_message("INFO", "after file opening");
// //     if (dbFile == -1)
// //     {
// //         perror("Error opening or creating the database file");
// //         return false;
// //     }
// //     // Calculate the offset for the specific record based on its size
// //     off_t recordOffset = lseek(dbFile, 0, SEEK_END);

// //     // Lock the specific record for writing (advisory lock)
// //     if (flock(dbFile, LOCK_EX) == -1)
// //     {
// //         perror("Error locking the record for writing");
// //         close(dbFile);
// //         return false;
// //     }

// //     // Write the employee information to the file at the calculated offset
// //     ssize_t writeResult = pwrite(dbFile, &emp, sizeof(struct Employee), recordOffset);
// //     write(dbFile, "\n", sizeof("\n") - 1);
// //     if (writeResult == -1 || writeResult < sizeof(struct Employee))
// //     {
// //         perror("Error writing employee record to database file");
// //         flock(dbFile, LOCK_UN); // Unlock the record before closing
// //         close(dbFile);
// //         return false;
// //     }

// //     // Unlock the specific record
// //     if (flock(dbFile, LOCK_UN) == -1)
// //     {
// //         perror("Error unlocking the record");
// //         close(dbFile);
// //         return false;
// //     }

// //     // Close the database file
// //     close(dbFile);

// //     return true;
// // }

// bool modifyEmployee(int clientSocket)
// {
//     struct Employee emp;
//     const char *dbFileName = "employee_db.txt";
//     char empId[10];

//     // Receive the employee ID from the client
//     char idPrompt[] = "Enter Employee ID to modify: ";
//     send(clientSocket, idPrompt, strlen(idPrompt), 0);
//     int readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
//     if (readResult <= 0)
//     {
//         send(clientSocket, "Error receiving Employee ID from client", strlen("Error receiving Employee ID from client"), 0);
//         return false;
//     }
//     empId[readResult] = '\0';
//     if (empId[readResult - 1] == '\n')
//     {
//         empId[readResult - 1] = '\0';
//     }

//     // Open the database file
//     int dbFile = open(dbFileName, O_RDWR);
//     if (dbFile == -1)
//     {
//         perror("Error opening the database file");
//         return false;
//     }

//     // Search for the employee record by ID
//     struct Employee tempEmp;
//     off_t recordOffset = 0;
//     bool employeeFound = false;

//     while (read(dbFile, &tempEmp, sizeof(struct Employee)) > 0)
//     {
//         log_message("INFO", tempEmp.emp_id);
//         int num1 = atoi(empId);
//         int num2 = atoi(tempEmp.emp_id);
//         if (num1==num2)
//         {
//             employeeFound = true;
//             break;
//         }
//         recordOffset += sizeof(struct Employee) + 1; // Include newline after each record
//     }

//     if (!employeeFound)
//     {
//         send(clientSocket, "Employee not found", strlen("Employee not found"), 0);
//         close(dbFile);
//         return false;
//     }

//     // Now ask the client for the new details and modify them
//     // Receive updated employee name
//     char namePrompt[] = "Enter new Employee name (leave blank to keep current): ";
//     send(clientSocket, namePrompt, strlen(namePrompt), 0);
//     readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
//     if (readResult > 0 && readResult != 1) // Ignore if just pressing enter
//     {
//         emp.emp_name[readResult] = '\0';
//         if (emp.emp_name[readResult - 1] == '\n')
//         {
//             emp.emp_name[readResult - 1] = '\0';
//         }
//         if (strlen(emp.emp_name) > 0)
//         {
//             strcpy(tempEmp.emp_name, emp.emp_name);
//         }
//     }

//     // Receive updated employee username
//     char usernamePrompt[] = "Enter new Employee username (leave blank to keep current): ";
//     send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
//     readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
//     if (readResult > 0 && readResult != 1)
//     {
//         emp.emp_username[readResult] = '\0';
//         if (emp.emp_username[readResult - 1] == '\n')
//         {
//             emp.emp_username[readResult - 1] = '\0';
//         }
//         if (strlen(emp.emp_username) > 0)
//         {
//             strcpy(tempEmp.emp_username, emp.emp_username);
//         }
//     }

//     // Receive updated employee password
//     char passPrompt[] = "Enter new password (leave blank to keep current): ";
//     send(clientSocket, passPrompt, strlen(passPrompt), 0);
//     readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
//     if (readResult > 0 && readResult != 1)
//     {
//         emp.emp_password[readResult] = '\0';
//         if (emp.emp_password[readResult - 1] == '\n')
//         {
//             emp.emp_password[readResult - 1] = '\0';
//         }
//         if (strlen(emp.emp_password) > 0)
//         {
//             strcpy(tempEmp.emp_password, emp.emp_password);
//         }
//     }

//     // Receive updated employee role
//     char rolePrompt[] = "Enter new role of employee (leave blank to keep current, 1 for employee, 2 for manager): ";
//     send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
//     char c[10];
//     readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
//     if (readResult > 0 && readResult != 1)
//     {
//         c[readResult] = '\0';
//         int choice = atoi(c);
//         if (choice == 1)
//         {
//             strcpy(tempEmp.role, "employee");
//         }
//         else if (choice == 2)
//         {
//             strcpy(tempEmp.role, "manager");
//         }
//     }

//     // Lock the file for writing
//     if (flock(dbFile, LOCK_EX) == -1)
//     {
//         perror("Error locking the record for writing");
//         close(dbFile);
//         return false;
//     }

//     // Write the modified employee information to the file at the specific offset
//     if (pwrite(dbFile, &tempEmp, sizeof(struct Employee), recordOffset) == -1)
//     {
//         perror("Error writing modified employee record to database file");
//         flock(dbFile, LOCK_UN); // Unlock the record before closing
//         close(dbFile);
//         return false;
//     }

//     // Unlock the record
//     if (flock(dbFile, LOCK_UN) == -1)
//     {
//         perror("Error unlocking the record");
//         close(dbFile);
//         return false;
//     }

//     // Close the database file
//     close(dbFile);

//     return true;
// }


// // bool modifyEmployee(int clientSocket)
// // {
// //     struct Employee emp;
// //     const char *dbFileName = "employee_db.txt";
// //     char empId[10];

// //     // Receive the employee ID from the client
// //     char idPrompt[] = "Enter Employee ID to modify: ";
// //     send(clientSocket, idPrompt, strlen(idPrompt), 0);
// //     int readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
// //     if (readResult <= 0)
// //     {
// //         send(clientSocket, "Error receiving Employee ID from client", strlen("Error receiving Employee ID from client"), 0);
// //         return false;
// //     }
// //     empId[readResult] = '\0';
// //     if (empId[readResult - 1] == '\n')
// //     {
// //         empId[readResult - 1] = '\0';
// //     }

// //     // Open the database file
// //     int dbFile = open(dbFileName, O_RDWR);
// //     if (dbFile == -1)
// //     {
// //         perror("Error opening the database file");
// //         return false;
// //     }

// //     // Search for the employee record by ID
// //     struct Employee tempEmp;
// //     off_t recordOffset = 0;
// //     bool employeeFound = false;

// //     while (read(dbFile, &tempEmp, sizeof(struct Employee)) > 0)
// //     {
// //         log_message("INFO", tempEmp.emp_id);
// //         if (strcmp(tempEmp.emp_id, empId) == 0)
// //         {
// //             employeeFound = true;
// //             break;
// //         }
// //         recordOffset += sizeof(struct Employee) + 1; // Include newline after each record
// //     }

// //     if (!employeeFound)
// //     {
// //         send(clientSocket, "Employee not found", strlen("Employee not found"), 0);
// //         close(dbFile);
// //         return false;
// //     }

// //     // Now ask the client for the new details and modify them
// //     // Receive updated employee name
// //     char namePrompt[] = "Enter new Employee name (leave blank to keep current): ";
// //     send(clientSocket, namePrompt, strlen(namePrompt), 0);
// //     readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
// //     if (readResult > 0)
// //     {
// //         emp.emp_name[readResult] = '\0';
// //         if (emp.emp_name[readResult - 1] == '\n')
// //         {
// //             emp.emp_name[readResult - 1] = '\0';
// //         }
// //         if (strlen(emp.emp_name) > 0)
// //         {
// //             strcpy(tempEmp.emp_name, emp.emp_name);
// //         }
// //     }

// //     // Receive updated employee username
// //     char usernamePrompt[] = "Enter new Employee username (leave blank to keep current): ";
// //     send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
// //     readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
// //     if (readResult > 0)
// //     {
// //         emp.emp_username[readResult] = '\0';
// //         if (emp.emp_username[readResult - 1] == '\n')
// //         {
// //             emp.emp_username[readResult - 1] = '\0';
// //         }
// //         if (strlen(emp.emp_username) > 0)
// //         {
// //             strcpy(tempEmp.emp_username, emp.emp_username);
// //         }
// //     }

// //     // Receive updated employee password
// //     char passPrompt[] = "Enter new password (leave blank to keep current): ";
// //     send(clientSocket, passPrompt, strlen(passPrompt), 0);
// //     readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
// //     if (readResult > 0)
// //     {
// //         emp.emp_password[readResult] = '\0';
// //         if (emp.emp_password[readResult - 1] == '\n')
// //         {
// //             emp.emp_password[readResult - 1] = '\0';
// //         }
// //         if (strlen(emp.emp_password) > 0)
// //         {
// //             strcpy(tempEmp.emp_password, emp.emp_password);
// //         }
// //     }

// //     // Receive updated employee role
// //     char rolePrompt[] = "Enter new role of employee (leave blank to keep current, 1 for employee, 2 for manager): ";
// //     send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
// //     char c[10];
// //     readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
// //     if (readResult > 0)
// //     {
// //         c[readResult] = '\0';
// //         int choice = atoi(c);
// //         if (choice == 1)
// //         {
// //             strcpy(tempEmp.role, "employee");
// //         }
// //         else if (choice == 2)
// //         {
// //             strcpy(tempEmp.role, "manager");
// //         }
// //     }

// //     // Lock the file for writing
// //     if (flock(dbFile, LOCK_EX) == -1)
// //     {
// //         perror("Error locking the record for writing");
// //         close(dbFile);
// //         return false;
// //     }

// //     // Write the modified employee information to the file at the specific offset
// //     if (pwrite(dbFile, &tempEmp, sizeof(struct Employee), recordOffset) == -1)
// //     {
// //         perror("Error writing modified employee record to database file");
// //         flock(dbFile, LOCK_UN); // Unlock the record before closing
// //         close(dbFile);
// //         return false;
// //     }

// //     // Unlock the record
// //     if (flock(dbFile, LOCK_UN) == -1)
// //     {
// //         perror("Error unlocking the record");
// //         close(dbFile);
// //         return false;
// //     }

// //     // Close the database file
// //     close(dbFile);

// //     return true;
// // }


// int getLastEmployeeId(FILE *file)
// {
//     struct Employee emp;
//     int last_id = 0;

//     if (file == NULL)
//     {
//         fprintf(stderr, "File pointer is NULL.\n");
//         return -1; // Return an error code or handle it appropriately
//     }

//     // Move the file pointer to the beginning of the file
//     fseek(file, 0, SEEK_SET);

//     // Read each record until the end of the file
//     while (fread(&emp, sizeof(struct Employee), 1, file))
//     {
//         last_id = atoi(emp.emp_id); 
//         // Converts the string to an integer    
//         }

//         // Check if fread ended due to an error
//         if (feof(file))
//         {
//             // End of file reached, return last valid employee ID
//             return last_id;
//         }
//         else if (ferror(file))
//         {
//             // An error occurred during fread
//             fprintf(stderr, "Error reading file.\n");
//             return -1; // Return an error code or handle it appropriately
//         }

//         return last_id;
//     }

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


bool addEmployee(int clientSocket)
{
    struct Employee emp;

    // Receive employee name from the client
    char namePrompt[] = "Enter Employee name: ";
    send(clientSocket, namePrompt, strlen(namePrompt), 0);
    int readResult = recv(clientSocket, emp.emp_name, sizeof(emp.emp_name) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Employee name from client", strlen("Error receiving Employee name from client"), 0);
        return false;
    }
    emp.emp_name[readResult] = '\0';
    if (emp.emp_name[readResult - 1] == '\n')
    {
        emp.emp_name[readResult - 1] = '\0';
    }

    // Receive employee username from the client
    char usernamePrompt[] = "Enter Employee username: ";
    send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
    readResult = recv(clientSocket, emp.emp_username, sizeof(emp.emp_username) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Employee username from client", strlen("Error receiving Employee username from client"), 0);
        return false;
    }
    emp.emp_username[readResult] = '\0';
    if (emp.emp_username[readResult - 1] == '\n')
    {
        emp.emp_username[readResult - 1] = '\0';
    }

    // Receive employee password from the client
    char passPrompt[] = "Enter password: ";
    send(clientSocket, passPrompt, strlen(passPrompt), 0);
    readResult = recv(clientSocket, emp.emp_password, sizeof(emp.emp_password) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Employee password from client", strlen("Error receiving Employee password from client"), 0);
        return false;
    }
    emp.emp_password[readResult] = '\0';
    if (emp.emp_password[readResult - 1] == '\n')
    {
        emp.emp_password[readResult - 1] = '\0';
    }

    // Receive employee role from the client
    char rolePrompt[] = "Enter role of employee (1 for employee, 2 for manager): ";
    send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
    char c[10];
    readResult = recv(clientSocket, &c, sizeof(c) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Employee role from client", strlen("Error receiving Employee role from client"), 0);
        return false;
    }
    int choice = atoi(c);
    if (choice == 1)
    {
        strcpy(emp.role, "employee");
    }
    else if (choice == 2)
    {
        strcpy(emp.role, "manager");
    }

int employeeId = incrementIdCount("next_employee_id");
    if (employeeId == -1) return false; // Error occurred while incrementing ID

    sprintf(emp.emp_id, "%d", employeeId);

    // Set active status
    strcpy(emp.active, "true");

    // Open the database file for appending
    const char *dbFileName = "employee_db.txt";
    int dbFile = open(dbFileName, O_RDWR | O_APPEND);
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

    // Format the employee data as a single string with newlines
    char empRecord[512];
    snprintf(empRecord, sizeof(empRecord), "Name: %s, Username: %s, Password: %s, Role: %s, ID: %s, Active: %s\n",
             emp.emp_name, emp.emp_username, emp.emp_password, emp.role, emp.emp_id, emp.active);

    // Write the employee information to the file as a single line
    ssize_t writeResult = write(dbFile, empRecord, strlen(empRecord));
    if (writeResult == -1)
    {
        perror("Error writing employee record to database file");
        flock(dbFile, LOCK_UN); // Unlock the file before closing
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


void getAllEmployees(int clientSocket) {
    int dbFile = open("employee_db.txt", O_RDONLY);
    if (dbFile == -1) {
        send(clientSocket, "Error opening database file\n", strlen("Error opening database file\n"), 0);
        return;
    }

    // Lock the file for shared reading
    if (flock(dbFile, LOCK_SH) == -1) {
        close(dbFile);
        send(clientSocket, "Error locking the database file\n", strlen("Error locking the database file\n"), 0);
        return;
    }

    // Read and send the employee data
    char buffer[256];
    ssize_t bytesRead;
    while ((bytesRead = read(dbFile, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        send(clientSocket, buffer, bytesRead, 0);
    }

    // Unlock and close the file
    flock(dbFile, LOCK_UN);
    close(dbFile);
}

bool modifyEmployee(int clientSocket)
{
    struct Employee emp;
    const char *dbFileName = "employee_db.txt";
    char empId[10];

    // Receive the employee ID from the client
    char idPrompt[] = "Enter Employee ID to modify: ";
    send(clientSocket, idPrompt, strlen(idPrompt), 0);
    int readResult = recv(clientSocket, empId, sizeof(empId) - 1, 0);
    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving Employee ID from client", strlen("Error receiving Employee ID from client"), 0);
        return false;
    }
    empId[readResult] = '\0';
    if (empId[readResult - 1] == '\n')
    {
        empId[readResult - 1] = '\0';
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
        char tempId[10];

        // Parse the employee ID from the line (assuming the format is consistent)
        sscanf(line, "Name: %[^,], Username: %[^,], Password: %[^,], Role: %[^,], ID: %[^,], Active: %s\n",
               tempEmp.emp_name, tempEmp.emp_username, tempEmp.emp_password, tempEmp.role, tempEmp.emp_id, tempEmp.active);

        // Compare the ID from the file with the one provided by the client
        if (strcmp(empId, tempEmp.emp_id) == 0)
        {
            employeeFound = true;

            // Now ask the client for the new details and modify them
            // Receive updated employee name
            char namePrompt[] = "Enter new Employee name (leave blank to keep current): ";
            send(clientSocket, namePrompt, strlen(namePrompt), 0);
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
            char usernamePrompt[] = "Enter new Employee username (leave blank to keep current): ";
            send(clientSocket, usernamePrompt, strlen(usernamePrompt), 0);
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
            char passPrompt[] = "Enter new password (leave blank to keep current): ";
            send(clientSocket, passPrompt, strlen(passPrompt), 0);
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
            char rolePrompt[] = "Enter new role of employee (leave blank to keep current, 1 for employee, 2 for manager): ";
            send(clientSocket, rolePrompt, strlen(rolePrompt), 0);
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
        perror("Error unlocking the file");
    }

    // If the employee was found, replace the original file with the modified file
    if (employeeFound)
    {
        rename("temp_db.txt", dbFileName); // Replace the original file with the temp file
        send(clientSocket, "Employee modified successfully", strlen("Employee modified successfully"), 0);
        return true;
    }
    else
    {
        send(clientSocket, "Employee not found", strlen("Employee not found"), 0);
        remove("temp_db.txt"); // Remove the temporary file
        return false;
    }
}




 #endif