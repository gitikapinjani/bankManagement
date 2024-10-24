#ifndef MESSAGES_H
#define MESSAGES_H

// General messages
#define LOGOUT_SUCCESS "Successfully logout!!\n"
#define AUTH_FAILURE "Authentication failed. Please try again.\n"
#define ERROR_READING_ADMIN_MENU "Error reading admin menu\n"

// Admin operation messages
#define ADMIN_MENU "1. Add Employee\n2. Modify Employee\n3. Get All Employees\n4. Logout\n5. Exit\n\nChoose option: "
#define ADD_EMPLOYEE_SUCCESS "Employee added successfully!\n"
#define MODIFY_EMPLOYEE_SUCCESS "Employee modified successfully!\n"
#define GET_ALL_EMPLOYEE_SUCCESS "Employee details fetched successfully!\n"

// Employee addition related messages
#define ENTER_EMP_NAME "Enter Employee Name: "
#define ENTER_EMP_USERNAME "Enter Employee Username: "
#define ENTER_EMP_PASSWORD "Enter Employee Password: "
#define ENTER_EMP_ROLE "Enter Employee Role (1: Employee, 2: Manager): "

// Error messages
#define ERROR_RECEIVE_EMP_NAME "Error receiving employee name\n"
#define ERROR_RECEIVE_EMP_USERNAME "Error receiving employee username\n"
#define ERROR_RECEIVE_EMP_PASSWORD "Error receiving employee password\n"
#define ERROR_RECEIVE_EMP_ROLE "Error receiving employee role\n"
#define ERROR_OPEN_DB_FILE "Error opening database file\n"
#define ERROR_LOCK_DB_FILE "Error locking database file\n"
#define ERROR_WRITE_DB_FILE "Error writing to database file\n"
#define ERROR_OPENING_LOG_FILE "Error opening log file\n"

// Other constants
#define ENTER_USERNAME "Enter username: "
#define ENTER_PASSWORD "Enter password: "

#define ENTER_EMP_ID "Enter Employee ID: "
#define ERROR_RECEIVE_EMP_ID "Error receiving Employee ID from client"
#define ENTER_NEW_EMP_NAME "Enter new Employee name (leave blank to keep current): "
#define ENTER_NEW_EMP_USERNAME "Enter new Employee username (leave blank to keep current): "
#define ENTER_NEW_EMP_PASSWORD "Enter new password (leave blank to keep current): "
#define ENTER_NEW_EMP_ROLE "Enter new role of employee (leave blank to keep current, 1 for employee, 2 for manager): "
#define EMP_MODIFY_SUCCESS "Employee modified successfully"
#define EMP_NOT_FOUND "Employee not found"
#define ERROR_OPEN_TEMP_FILE "Error opening temporary file"
#define ERROR_UNLOCK_DB_FILE "Error unlocking the file"


#define LOGIN_SUCCESS "Login Successfully\n"
#define ERROR_ADMIN_MENU "Error while reading client's choice for ADMIN_MENU\n"
#define INVALID_INPUT "Wrong input. Please try again\n"
#define AUTHENTICATION_SUCCESS "Authentication successful\n"
#define AUTHENTICATION_FAILED "Authentication failed\n"
#define ENTER_USERNAME "Enter username: "
#define ENTER_PASSWORD "Enter password: "
#define ERROR_RECEIVING_USERNAME "Error receiving username\n"
#define ERROR_RECEIVING_PASSWORD "Error receiving password\n"

// Employee menu messages
#define EMPLOYEE_MENU "Do you want to:\n1. Add new Customer\n2. Modify Customer Detail\n3. View all customers\n4. Reject/Approve Loan\n5. View assigned loan application\n6. View transaction of customer\n7. Change Password\n8. Change password for customer\n9. Logout\n10. Exit from this menu\n"
#define CUSTOMER_ADDED "Successfully added the customer\n"
#define CUSTOMER_MODIFIED "Successfully modified customer!\n"
#define CUSTOMERS_DISPLAYED "Successfully displayed all customers!\n"
#define LOAN_PROCESSED "Successfully processed the loan!\n"
#define TRANSACTION_HISTORY "Successfully viewed transaction history!\n"
#define PASSWORD_CHANGED "Successfully changed the password!\n"
#define LOGOUT_SUCCESSFUL "Successfully logged out!\n"

// Manager menu messages
#define MANAGER_MENU "Do you want to:\n1. Add new Customer\n2. Modify Customer Detail\n3. View all customers\n4. Reject/Approve Loan\n5. View assigned loan application\n6. View transaction of customer\n7. Activate/deactivate customer account\n8. Assign Loan application to employee\n9. Review customer feedback\n10. Change Password\n11. Change password for customer\n12. Logout\n13. Exit from this menu\n"

// Error messages
#define TRY_AGAIN "Please try again\n"

// add customer
#define ENTER_CUSTOMER_NAME "Enter Customer name: "
#define ERROR_RECEIVING_CUSTOMER_NAME "Error receiving Customer name from client\n"
#define ENTER_CUSTOMER_USERNAME "Enter Customer username: "
#define ERROR_RECEIVING_CUSTOMER_USERNAME "Error receiving Customer username from client\n"
#define ENTER_CUSTOMER_PASSWORD "Enter Customer password: "
#define ERROR_RECEIVING_CUSTOMER_PASSWORD "Error receiving Customer password from client\n"
#define ENTER_CUSTOMER_BALANCE "Enter customer balance: "
#define ERROR_RECEIVING_CUSTOMER_BALANCE "Error receiving Customer balance from client\n"

#define ENTER_CUSTOMER_ID "Enter Customer ID: "
#define ERROR_RECEIVING_CUSTOMER_ID "Error receiving Customer ID from client\n"
#define CUSTOMER_NOT_FOUND "Customer not found\n"
#define CUSTOMER_MODIFIED_SUCCESS "Customer modified successfully\n"

// Update CUSTOMER messages
#define ENTER_NEW_CUSTOMER_NAME "Enter new Customer name (leave blank to keep current): "
#define ENTER_NEW_CUSTOMER_USERNAME "Enter new Customer username (leave blank to keep current): "
#define ENTER_NEW_CUSTOMER_PASSWORD "Enter new password (leave blank to keep current): "
#define ENTER_NEW_LOAN_AMOUNT "Enter new Loan Amount (leave blank to keep current): "
#define ENTER_NEW_LOAN_STATUS "Enter New Loan Status (leave blank to keep current): "
#define ENTER_NEW_FD_AMOUNT "Enter new FD Amount (leave blank to keep current): "
#define ENTER_NEW_ACCOUNT_BALANCE "Enter new Account Balance (leave blank to keep current): "

// view Transaction History
#define NO_TRANSACTION_FOUND "No transactions found for this customer.\n"
#define END_OF_TRANSACTION_HISTORY "End of transaction history.\n"

// Assign Employee to Loan messages
#define EMPLOYEE_ASSIGNED_SUCCESS "Employee successfully assigned to loan.\n"
#define CUSTOMER_LOAN_NOT_FOUND "Customer loan not found.\n"

// Process Loan messages
#define LOAN_NOT_FOUND "Loan not found or already processed\n"
#define PROMPT_DECISION "Do you want to approve or reject the loan? (Enter 'approve' or 'reject'): "
#define ERROR_RECEIVING_DECISION "Error receiving decision\n"
#define INVALID_DECISION "Invalid decision\n"
#define ERROR_CREATING_TEMP_LOAN_DB "Error creating temp loan database\n"
#define ERROR_CREATING_TEMP_CUSTOMER_DB "Error creating temp customer database\n"
#define LOAN_PROCESSED_SUCCESS "Loan Processed Successfully\n"

// change password employee
#define USERNAME_NOT_FOUND "Username not found."
#define ENTER_CURRENT_PASSWORD "Please enter your current password: "
#define CURRENT_PASSWORD_INCORRECT "Current password is incorrect."
#define ENTER_NEW_PASSWORD "Please enter your new password: "

#define LOANS_DISPLAYED "Successfully displayed all loans!\n"

#endif // MESSAGES_H
