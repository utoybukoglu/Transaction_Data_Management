#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Customer
{
    char name[81];
    int transactions;
    int items_purchased;
    float amount_paid;
};

typedef struct Customer customer;

void menu();
void printCustomers(customer *, int);
int countCustomers(FILE *);
void readTransactions(FILE *, customer *, int);
void swap(customer *, customer *);
void heapify(customer *, int, int, int);
void BuildMaxHeap(customer *, int, int);
void heapSort(customer *, int, int);

int main()
{
    FILE *inFile;
    int no_of_customers = 0;
    customer *customers;

    inFile = fopen("transactions.txt", "r");
    if (inFile == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }
    no_of_customers = countCustomers(inFile);
    printf("There are %d unique customers\n", no_of_customers);

    customers = (customer *)malloc(no_of_customers * sizeof(customer));
    fclose(inFile);
    inFile = fopen("transactions.txt", "r");
    if (inFile == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    readTransactions(inFile, customers, no_of_customers);
    if (customers == NULL)
    {
        printf("\nCustomers have NOT been read successfully!\n");
        exit(1);
    }
    printCustomers(customers, no_of_customers);


    char command = 0;
    int exit = 0;
    while (exit != 1)
    {
        menu();
        printf("\nCommand: ");
        fflush(stdin);
        scanf("%c", &command);

        if (command == '1' || command == '2' || command == '3' || command == '4')
        {
            heapSort(customers, no_of_customers, command - '0');
            printCustomers(customers, no_of_customers);
        }
        else if (command == '5')
        {
            exit = 1;
            printf("Goodbye!\n");
        }
        else
        {
            printf("Please enter a valid command!\n");
        }

    }

    free(customers);
    return 0;
}

void menu()
{
    printf("Please choose one of the following options:\n"
           "(1) Display customers sorted by number of transactions\n"
           "(2) Display customers sorted by number of items purchased\n"
           "(3) Display customers sorted by total amount paid\n"
           "(4) Display customers sorted by average amount paid per transaction\n"
           "(5) Exit\n");
}

void printCustomers(customer *customers, int no_of_customers)
{
    printf("List of customers:\n");
    int i;
    for (i = 0; i < no_of_customers; ++i)
    {
        printf("Name: %s, number of transactions = %d, number of items = %d, total amount paid = %.2f,"
               " average amount paid per transaction = %.2f\n",
               customers[i].name, customers[i].transactions, customers[i].items_purchased, customers[i].amount_paid,
               customers[i].amount_paid / customers[i].transactions);
    }
}

int countCustomers(FILE *inFile)
{
    int size = 0, notUnique;// size is number of unique customers, notUnique = 1 means customer is not Unique
    const int guess = 40; // my assumption about size of the file
    char line[200], name[81];
    char **nameArray = NULL;

    nameArray = (char **)malloc(guess * sizeof(char *));
    if (nameArray == NULL)
        printf("Memory allocation failed");

    for (int i = 0; i < guess; i++)
        nameArray[i] = (char *)malloc(81 * sizeof(char));

    if (fgets(line, sizeof(line), inFile) == NULL)
    {
        perror("Error reading the first line\n");
        exit(1);
    } // Skipped the first line

    while (fgets(line, sizeof(line), inFile))
    {
        notUnique = 0; // reset notUnique

        if (sscanf(line, "%[^;]", name)) // reading only names
        {
            for (int i = 0; i < size; i++)// checks if current name has a match in nameArray
            {
                if (strcasecmp(nameArray[i], name) == 0) // checking if the customer is unique or not
                {
                    notUnique = 1;
                    break;
                }
            }

            if (notUnique == 0 && size < guess)// if customer is unique and nameArray has enough capacity adding name of the customer
            {
                strcpy(nameArray[size], name);
                size++;// increasing number of unique customers
            }

            else if (notUnique == 0 && size >= guess)// if unique customer size is greater than my guess I'm re-allocate the nameArray
            {
                nameArray = (char **)realloc(nameArray, (size + 1) * sizeof(char *));
                nameArray[size] = (char *)malloc(81 * sizeof(char));
                strcpy(nameArray[size], name);
                size++;
            }
        }
    }

    if (size < guess) // free operation if array is not re-allocated
    {
        for (int i = 0; i < guess; i++)
            free(nameArray[i]);
    }

    else // free operation if array is re-allocated
    {
        for (int i = 0; i < size; i++)
            free(nameArray[i]);
    }

    free(nameArray);

    return size; // returns number of unique customers
}

void readTransactions(FILE *inFile, customer *customers, int no_of_customers)
{
    int temp1, items_purchased, counter = 0, notUnique; // notUnique = 1 means customer is not Unique
    char line[200], name[81];
    float amount_paid;

    for (int i = 0; i < no_of_customers; i++)
        customers[i].transactions = 1; // assign 1 to the transaction number for each customer in order to do count the transaction number properly

    if (fgets(line, sizeof(line), inFile) == NULL)
    {
        printf("Error reading the first line\n");
        exit(1);
    } // Skipped the first line

    while (fgets(line, sizeof(line), inFile))
    {
        notUnique = 0;// reset the value
        if (sscanf(line, "%[^;];%d;%*[^;];%*[^;];%d;%f;%*[^\n]", name,
                   &temp1, &items_purchased, &amount_paid))
        {
            for (int i = 0; i < no_of_customers; i++)// I'm comparing each name in the file with the all names in customers array
            {
                if (strcasecmp(customers[i].name, name) == 0) // Finds a match
                {
                    notUnique = 1;
                    customers[i].transactions++;
                    customers[i].items_purchased += items_purchased;
                    customers[i].amount_paid += amount_paid * items_purchased;
                    break;
                }
            }

            if (notUnique == 0 && strcasecmp(customers[counter].name, "") == 0) // if name is unique and that index(counter) is empty, I'm adding the customer and necessary information
            {
                strcpy(customers[counter].name, name);
                customers[counter].items_purchased = items_purchased;
                customers[counter].amount_paid = amount_paid * items_purchased;
                counter++;
            }
        }
    }

    fclose(inFile);
}

void swap(customer *first, customer *second)
{
    customer temp;

    temp = *first;
    *first = *second;
    *second = temp;
}

// https://www.geeksforgeeks.org/heap-sort/
void heapify(customer *customers, int no_of_customers, int i, int criteria)
{
    int max = i, left = i * 2 + 1, right = 2 * i + 2;

    if (criteria == 1)
    {
        if (left < no_of_customers && customers[left].transactions > customers[max].transactions)
            max = left;
        if (right < no_of_customers && customers[right].transactions > customers[max].transactions)
            max = right;
    }

    else if (criteria == 2)
    {
        if (left < no_of_customers && customers[left].items_purchased > customers[max].items_purchased)
            max = left;
        if (right < no_of_customers && customers[right].items_purchased > customers[max].items_purchased)
            max = right;
    }

    else if (criteria == 3)
    {
        if (left < no_of_customers && customers[left].amount_paid > customers[max].amount_paid)
            max = left;
        if (right < no_of_customers && customers[right].amount_paid > customers[max].amount_paid)
            max = right;
    }

    else if (criteria == 4)
    {
        if (left < no_of_customers && (customers[left].amount_paid / customers[left].transactions) > (customers[max].amount_paid / customers[max].transactions))
            max = left;
        if (right < no_of_customers && (customers[right].amount_paid / customers[right].transactions) > (customers[max].amount_paid / customers[max].transactions))
            max = right;
    }

    if (i != max)
    {
        swap(&customers[i], &customers[max]);
        heapify(customers, no_of_customers, max, criteria);
    }
}

void BuildMaxHeap(customer *customers, int no_of_customers, int criteria)// building a max heap in order to apply heap sort
{
    for (int i = no_of_customers / 2 - 1; i >= 0; i--)
        heapify(customers, no_of_customers, i, criteria);
}

// https://www.geeksforgeeks.org/heap-sort/
void heapSort(customer *customers, int no_of_customers, int criteria)
{
    BuildMaxHeap(customers, no_of_customers, criteria);

    for (int i = no_of_customers - 1; i > 0; i--)
    {
        swap(&customers[0], &customers[i]);
        heapify(customers, i, 0, criteria);
    }
}