                                                 //-------------------------------//
                                                //  LIBRARY MANAGEMENT SYSTEM    //
                                               //         SUBMITTED BY:         // 
                                              //    Muhammad Hamza 22K-4523    //
                                             //     Saad Arshad    22K-4141   //
                                            //      Abdul Wasey    22K-4172  //
                                           //-------------------------------//







#include <iostream>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<stdlib.h>
#include<unordered_map>
#include<queue>
#include<stack>
#include <Windows.h>
#include <ctime>
#include <map>
#include<set>

using namespace std;
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE "\033[34m"
#define ITALIC "\033[3m"
#define BOLD "\033[1m"
#define UL "\033[4m"


struct checkoutRecord {
    int bookId;
    int patronId;
    bool checkoutStatus;
    int dueDate;
};



class BookGraph {
public:
    unordered_map<int, set<int>> dependencies; // BookId -> Set of dependent BookIds

    // Add a dependency between two books
    void addDependency(int bookId, int dependentBookId) {
        dependencies[bookId].insert(dependentBookId);
    }

    // Check if a book has dependencies
    bool hasDependencies(int bookId) const {
        return dependencies.find(bookId) != dependencies.end() && !dependencies.at(bookId).empty();
    }

    // Display dependencies for a book
    void displayDependencies(int bookId) const {
        if (hasDependencies(bookId)) {
            cout << "Dependencies for Book ID " << bookId << ": ";
            for (int depBookId : dependencies.at(bookId)) {
                cout << depBookId << " ";
            }
            cout << "\n";
        }
        else
        {
            cout << "No dependencies for Book ID " << bookId << "\n";
        }
    }
};



class Book {
public:
    string title;
    string author;
    int id;
    bool availability;
    set<int> dependencies;

    // Function to add a dependency for the book
    void addDependency(int dependentBookId) {
        dependencies.insert(dependentBookId);
    }
public:
    Book() : id(0), availability(false) {}

    Book(string t, string a, int Id, bool ava) : title(t), author(a), id(Id), availability(ava) {}
};

class Node {
public:
    Book book;
    Node* left;
    Node* right;

    Node(Book b) : book(b), left(NULL), right(NULL) {}
};


class BST {
private:
    Node* root;

    Node* insert(Node* node, Book book) {
        if (node == NULL) {
            return new Node(book);
        }

        if (book.id < node->book.id) {
            node->left = insert(node->left, book);
        }
        else if (book.id > node->book.id) {
            node->right = insert(node->right, book);
        }

        return node;
    }

    Node* searchById(Node* node, int bookId) {
        if (node == NULL || node->book.id == bookId) {
            return node;
        }

        if (bookId < node->book.id) {
            return searchById(node->left, bookId);
        }
        else {
            return searchById(node->right, bookId);
        }
    }

  

   

public:
    BST() : root(NULL) {}

    void insert(Book book) {
        root = insert(root, book);
    }

    Node* searchById(int bookId) {
        return searchById(root, bookId);
    }

   
};


class Patron {
public:
    string name;
    int id;

    Patron() : name(""), id(0) {}

    Patron(string Name, int Id) : name(Name), id(Id) {}
};

class Library {
private:
    vector<Book> catalogue;
   
    unordered_map<int, bool> bookCopies; //int is bookId and bool is book's availability
    
    unordered_map<int, queue<Patron>> bookRequests;  //int is bookId mapped to queue of patrons waiting for that book 
    
    stack<checkoutRecord> checkoutHistory;
    
    stack<string> adminHistory;
    
    const string patronFileName = "patrons.txt";
    
    BookGraph bookGraph;
  

    void displayReversedStack(stack<string> tempStack) const {
        while (!tempStack.empty()) {
            cout << tempStack.top() << "\n";
            tempStack.pop();
        }
    }

    //load patrons from file in patrons vector
    void loadPatrons() {
        ifstream file(patronFileName);
        if (file.is_open()) {
            patrons.clear();  // clear patrons vector

            Patron tempPatron;
            while (file >> tempPatron.name >> tempPatron.id) {
                patrons.push_back(tempPatron);
            }

            file.close();
        }
    }

    // Save patrons to file from patrons vector
    void savePatrons() {
        ofstream file(patronFileName,ios::app);
        if (file.is_open()) {
            for (const Patron& patron : patrons) 
            {
                file << patron.name << " " << patron.id << "\n";
            }

            file.close();
        }
    }

public:
    vector<Patron> patrons;
    unordered_map<int, vector<int>> Checkedout_books;
    string getFileName() {
        return patronFileName;
    }

    const unordered_map<int, queue<Patron>>& getBookRequests() const {
        return bookRequests;
    }

    void addBook() {
        system("CLS");
        cout << BLUE << BOLD << ITALIC << "\t\t\t\tBOOK ADDITION SECTION\n\n" << RESET;

        int bookId;
        string bookTitle, bookAuthor;
        cout << ITALIC << "Enter details of the book you want to add.\n";
        cout << BOLD << ITALIC << YELLOW << "Book Id: ";
        cin >> bookId;
        cin.ignore();
        cout << "Book Title: ";
        getline(cin, bookTitle);
        cout << "Book Author: ";
        getline(cin, bookAuthor);

        Book newBook(bookTitle, bookAuthor, bookId, true);
        catalogue.push_back(newBook);
        bookCopies[bookId] = true;

        ofstream bookFile("books.txt", ios::app);
        if (bookFile.is_open()) {
            bookFile << newBook.id << "\t" << newBook.title << "\t" << newBook.author << "\t" << "1\n";
            bookFile.close();
            cout << GREEN << BOLD << ITALIC << "\nBOOK ADDED SUCCESSFULLY!\n";

            // Update the book graph with dependencies
            for (int depBookId : newBook.dependencies) {
                bookGraph.addDependency(newBook.id, depBookId);
            }


        }
        else {
            cout << RED << BOLD << ITALIC << "\nError opening the file to save book information.\n" << RESET;
        }
        system("pause");
        adminHistory.push("Admin added a book: " + newBook.title);
    }


    void removeBook(int bookId)       //modified after first evaluation,,, now instead of making the book unavailable it is just deleting the book from file and everywehere.
    {
        ifstream bookFile("books.txt");
        bool bookFound = false;

        if (bookFile.is_open()) {
            int catalogBookId;
            string catalogBookTitle, catalogBookAuthor;
            bool catalogAvailability;

            vector<string> lines; // file se lines leke vector main daalni hain

            while (bookFile >> catalogBookId >> ws && getline(bookFile, catalogBookTitle, '\t') && getline(bookFile, catalogBookAuthor, '\t') >> catalogAvailability) {
                if (catalogBookId == bookId) {
                    // Book found, mark it for removal
                    bookFound = true;
                }
                else {
                    stringstream ss;
                    ss << catalogBookId << "\t" << catalogBookTitle << "\t" << catalogBookAuthor << "\t" << (catalogAvailability ? "1" : "0");
                    lines.push_back(ss.str());
                }
            }

            bookFile.close();

            ofstream writeToFile("books.txt");
            if (writeToFile.is_open()) {
                for (const string& line : lines) {
                    writeToFile << line << "\n";
                }
                writeToFile.close();
            }
            else {
                cout << RED << "Error opening the file to update book information.\n" << RESET;
                system("pause");
                return;
            }
        }
        else {
            cout << RED << "Error opening the file to read book information.\n" << RESET;
            system("pause");
            return;
        }

        if (bookFound) {
            cout << GREEN << BOLD << ITALIC << "\nBOOK REMOVED SUCCESSFULLY!\n" << RESET;

            // Update bookCopies
            bookCopies.erase(bookId);

            adminHistory.push("Admin removed a book with ID: " + to_string(bookId));
        }
        else {
            cout << "\nThere is no book with such Id.\n";
        }

        system("pause");
    }


    void addUser(const Patron& patron) {
        patrons.push_back(patron);
        savePatrons();
        cout << GREEN << "\nUSER ADDED SUCCESSFULLY!\n" << RESET;
        system("pause");
    }

    void removeUser(int userId) {
        auto foundPatron = find_if(patrons.begin(), patrons.end(), [userId](const Patron& patron) {return patron.id == userId; });

        if (foundPatron != patrons.end()) {
            patrons.erase(foundPatron);
            savePatrons();
            cout << GREEN << "\nUSER REMOVED SUCCESSFULLY!\n" << RESET;
        }
        else {
            cout << ITALIC << RED << "\nThere is no user with such ID.\n" << RESET;
        }
        system("pause");
    }

    void displayCatalogue() const {
        system("CLS");
        cout << ITALIC << BOLD << GREEN << "\n\t\t\t\tBOOK CATALOGUE\n\n" << RESET;

        ifstream bookFile("books.txt");
        if (!bookFile.is_open()) {
            cout << RED << "Error opening the file to read book information.\n" << RESET;
            system("pause");
            return;
        }

        cout << BLUE << BOLD << ITALIC << "Book ID\t\tTitle\t\tAuthor\t\tAvailability\n" << RESET;
        cout << "---------------------------------------------------------------\n";

        int bookId;
        string bookTitle, bookAuthor;
        bool availability;

        while (bookFile >> bookId >> ws && getline(bookFile, bookTitle, '\t') && getline(bookFile, bookAuthor, '\t') >> availability) {
            cout << bookId << "\t\t" << bookTitle << "\t\t" << bookAuthor << "\t\t" << (availability ? "Available" : "Not Available") << "\n";
        }

        bookFile.close();

        system("pause");
    }

    void requestBook(int bookId, const Patron& patron) {
        bookRequests[bookId].push(patron);
        cout << BOLD << ITALIC << GREEN << "Your request for the book has been added" << RESET << GREEN << BOLD << " You will be notified when it's available.\n" << RESET;

    }


    void checkoutBook(int bookId, const Patron& patron) {
        ifstream bookFile("books.txt");
        bool bookAvailable = false;

        if (bookFile.is_open()) {
            int catalogBookId;
            string catalogBookTitle, catalogBookAuthor;
            bool catalogAvailability;

            vector<string> lines; //file se lines store kara raha is vector main

            while (bookFile >> catalogBookId >> ws && getline(bookFile, catalogBookTitle, '\t') && getline(bookFile, catalogBookAuthor, '\t') >> catalogAvailability) {
                if (catalogBookId == bookId && catalogAvailability) {
                    catalogAvailability = false;
                    bookCopies[bookId] = true;
                    bookAvailable = true;

                }

                stringstream ss;
                ss << catalogBookId << "\t" << catalogBookTitle << "\t" << catalogBookAuthor << "\t" << (catalogAvailability ? "1" : "0");
                lines.push_back(ss.str());
            }

            bookFile.close();

            ofstream writeToFile("books.txt");
            if (writeToFile.is_open()) {
                for (const string& line : lines) {
                    writeToFile << line << "\n";
                }
                writeToFile.close();
            }
            else {
                cout << RED << "Error opening the file to update book information.\n" << RESET;
                system("pause");
                return;
            }

        }
        else {
            cout << RED << "Error opening the file to read book information.\n" << RESET;
            system("pause");
            return;
        }

        if (bookAvailable && bookCopies[bookId] == true) {
            bookCopies[bookId] = false;
            checkoutRecord record;
            record.bookId = bookId;
            record.patronId = patron.id;
            record.checkoutStatus = true;

            Checkedout_books[patron.id].push_back(bookId);

            ofstream checkedOutFile("checkedoutBooks.txt", ios::app);
            if (checkedOutFile.is_open()) {
                checkedOutFile << patron.id << "\t" << bookId << "\n";
                checkedOutFile.close();
            }
            else {
                cout << RED << BOLD << ITALIC << "\nError opening the file to save checked-out book information.\n" << RESET;
            }


            //To show the due date of the checkedout book.
            time_t now = time(0);                    //VISUAL STUDIO might show error so run in Devc++ or comment these lines out
            tm* localTime = localtime(&now);         // 
            localTime->tm_mday += 7;                 //
            mktime(localTime);                       //
                                                     // 
            record.dueDate = localTime->tm_mday;    // 
           
           checkoutHistory.push(record);
           //
           cout << GREEN << BOLD << ITALIC << "\nBook checked out successfully!" << RESET;
    //comment out to the following line avoid errors in VISUAL STUDIO COMM. 
         << BLUE << BOLD << ITALIC << " Due date: " << record.dueDate << " th of this month.\n" << RESET;
        }
        else {
            cout << RED << BOLD << ITALIC << "\nSorry, the book is currently not available.\n" << RESET;

            cout << YELLOW << BOLD << ITALIC << "Do you want to request this book? (1 for yes, 0 for no): " << RESET;
            int reqOpt;
            cin >> reqOpt;

            if (reqOpt == 1) {
                requestBook(bookId, patron);
            }
        }
        system("pause");
    }
 
    unordered_map<int, vector<int>> checkedOut;  // Map of userId to vector of checked out bookIds





    void updateBookAvailability(int bookId, bool availability) {
        ifstream bookFile("books.txt");
        bool bookFound = false;

        if (bookFile.is_open()) {
            int catalogBookId;
            string catalogBookTitle, catalogBookAuthor;
            bool catalogAvailability;

            vector<string> lines; // file se lines leke vector main daalni hain

            while (bookFile >> catalogBookId >> ws && getline(bookFile, catalogBookTitle, '\t') && getline(bookFile, catalogBookAuthor, '\t') >> catalogAvailability) {
                if (catalogBookId == bookId) {
                    // if Book found, update its availability
                    catalogAvailability = availability;
                    bookFound = true;
                }

                stringstream ss;
                ss << catalogBookId << "\t" << catalogBookTitle << "\t" << catalogBookAuthor << "\t" << (catalogAvailability ? "1" : "0");
                lines.push_back(ss.str());
            }

            bookFile.close();

            ofstream writeToFile("books.txt");
            if (writeToFile.is_open()) {
                for (const string& line : lines) {
                    writeToFile << line << "\n";
                }
                writeToFile.close();
            }
            else {
                cout << RED << "Error opening the file to update book information.\n" << RESET;
                system("pause");
                return;
            }
        }
        else {
            cout << RED << "Error opening the file to read book information.\n" << RESET;
            system("pause");
            return;
        }

        if (!bookFound) {
            cout << RED << "Error updating book availability. Book ID not found.\n" << RESET;
        }
    }

    const vector<int>& getCheckedOutBooks(int userId) const {
        auto it = checkedOut.find(userId);
        if (it != checkedOut.end()) {
            return it->second;
        }
        else {
            static const vector<int> emptyVector;  // Return an empty vector if user not found
            return emptyVector;
        }
    }



   


    void setDueDate(int bookId, int dueDate)
    {
        stack<checkoutRecord> tempStack;
        while (!checkoutHistory.empty()) {
            checkoutRecord record = checkoutHistory.top();
            checkoutHistory.pop();

            if (record.bookId == bookId) {
                record.dueDate = dueDate;
            }

            tempStack.push(record);
        }

        while (!tempStack.empty()) {
            checkoutHistory.push(tempStack.top());
            tempStack.pop();
        }

        cout << YELLOW << "\nDue date set successfully!\n" << RESET;
        system("pause");
    }

    void notifyUsers() {
        system("CLS");
        cout << "\n\t\t\t\tNOTIFICATIONS\n\n";

        const auto& allRequests = getBookRequests();

        for (auto it = allRequests.begin(); it != allRequests.end(); ++it) {
            int bookId = it->first;
            const queue<Patron>& requestQueue = it->second;

            if (!requestQueue.empty()) {
                cout << "Book ID: " << bookId << " has pending requests.\n";
                cout << "Patron(s) waiting for the book:\n";

                queue<Patron> tempQueue = requestQueue;
                while (!tempQueue.empty()) {
                    cout << "\t- " << tempQueue.front().name << " (ID: " << tempQueue.front().id << ")\n";
                    tempQueue.pop();
                }

                //display due date
                stack<checkoutRecord> tempStack;
                bool found = false;

                while (!checkoutHistory.empty()) {
                    checkoutRecord record = checkoutHistory.top();
                    checkoutHistory.pop();

                    if (record.bookId == bookId && record.checkoutStatus) {
                        found = true;
                        cout << "Due date: " << record.dueDate << " th of this month.\n";
                    }

                    tempStack.push(record);
                }

                while (!tempStack.empty()) {
                    checkoutHistory.push(tempStack.top());
                    tempStack.pop();
                }

                //                if (!found) {
                //                    cout << "The book is not currently checked out.\n";
                //                }

                cout << "-----------------------------------------\n";
            }
        }
    }

    void checkoutProcess(const Patron& patron) {
        displayCatalogue();

        int bookId;
        cout << "Enter the ID of the book you want to checkout (0 to go back): ";
        cin >> bookId;


        if (bookId != 0) {
            checkoutBook(bookId, patron);
        }
    }

    void displayHistory() const {
        system("CLS");
        cout << YELLOW << BOLD << ITALIC << "\n\t\t\t\tRECENT ACTIVITY\n\n";

        if (adminHistory.empty()) {
            cout << "No recent activity.\n";
        }
        else {
            stack<string> tempStack = adminHistory;

            // Display the reversed order
            displayReversedStack(tempStack);
        }

        system("pause");
    }

    void displayBookDependencies(int bookId) const {
        bookGraph.displayDependencies(bookId);
        system("pause");
    }


    BST bookIdBST;

    void BSTFromFile(BST& bookBST) {
        ifstream bookFile("books.txt");
        if (!bookFile.is_open()) {
            cout << RED << "Error opening the file to read book information.\n" << RESET;
            return;
        }

        int catalogBookId;
        string catalogBookTitle, catalogBookAuthor;
        bool catalogAvailability;

        while (bookFile >> catalogBookId >> ws && getline(bookFile, catalogBookTitle, '\t') && getline(bookFile, catalogBookAuthor, '\t') >> catalogAvailability) {
            Book newBook(catalogBookTitle, catalogBookAuthor, catalogBookId, catalogAvailability);
            bookBST.insert(newBook);
        }

        bookFile.close();
    }

 void searchBookById(int bookId) {
        Node* result = bookIdBST.searchById(bookId);

        if (result == NULL) {
            cout << "\nBOOK NOT FOUND!\n";
            cout << endl;
            system("pause");
        }
        else {
            cout << "\nBOOK FOUND!\n";
            cout << "ID: " << result->book.id << "\n";
            cout << "Title: " << result->book.title << "\n";
            cout << "Author: " << result->book.author << "\n";
            cout << "Availability: " << (result->book.availability ? "Available" : "Not Available") << "\n";
        }
       
    }
    

 void displayCheckedOutBooks(const Patron& patron) const {
     system("CLS");
     cout << BOLD << ITALIC << GREEN << "\n\t\t\t\tCHECKED-OUT BOOKS\n\n" << RESET;

     auto it = Checkedout_books.find(patron.id);
     if (it != Checkedout_books.end()) {
         const vector<int>& checkedOutBooks = it->second;

         if (checkedOutBooks.empty()) {
             cout << "No books checked out by " << patron.name << " (ID: " << patron.id << ").\n";
         }
         else {
             cout << BLUE << BOLD << ITALIC << "Book ID\t\tTitle\t\tAuthor\n" << RESET;
             cout << "---------------------------------------------------------------\n";

             for (int bookId : checkedOutBooks) {
                 auto bookIt = find_if(catalogue.begin(), catalogue.end(), [bookId](const Book& book) { return book.id == bookId; });

                 if (bookIt != catalogue.end()) {
                     cout << bookId << "\t\t" << bookIt->title << "\t\t" << bookIt->author << "\n";
                 }
             }
         }
     }
     else {
         cout << "No books checked out by " << patron.name << " (ID: " << patron.id << ").\n";
     }

     system("pause");
 }





 void returnBook(int bookId) //Added after first evaluation
 {
     auto it = find_if(catalogue.begin(), catalogue.end(), [bookId](const Book& book) { return book.id == bookId; });

     if (it != catalogue.end()) {
         // Update book availability in the catalogue
         it->availability = true;

         // Update book availability in the "books.txt" file
         ifstream bookFile("books.txt");
         bool bookFound = false;

         if (bookFile.is_open()) {
             int catalogBookId;
             string catalogBookTitle, catalogBookAuthor;
             bool catalogAvailability;

             vector<string> lines; // file se lines leke vector main daalni hain

             while (bookFile >> catalogBookId >> ws && getline(bookFile, catalogBookTitle, '\t') && getline(bookFile, catalogBookAuthor, '\t') >> catalogAvailability) {
                 if (catalogBookId == bookId) {
                     // Book found, update its availability
                     catalogAvailability = true;
                     bookFound = true;
                 }

                 stringstream ss;
                 ss << catalogBookId << "\t" << catalogBookTitle << "\t" << catalogBookAuthor << "\t" << (catalogAvailability ? "1" : "0");
                 lines.push_back(ss.str());
             }

             bookFile.close();

             ofstream writeToFile("books.txt");
             if (writeToFile.is_open()) {
                 for (const string& line : lines) {
                     writeToFile << line << "\n";
                 }
                 writeToFile.close();
             }
             else {
                 cout << RED << "Error opening the file to update book information.\n" << RESET;
                 system("pause");
                 return;
             }
         }
         else {
             cout << RED << "Error opening the file to read book information.\n" << RESET;
             system("pause");
             return;
         }

         if (bookFound) {
             cout << GREEN << BOLD << ITALIC << "\nBook returned successfully!\n" << RESET;

             // Remove the book from the checked-out books map
             for (auto& patronBooks : Checkedout_books) {
                 auto& bookIds = patronBooks.second;
                 bookIds.erase(remove(bookIds.begin(), bookIds.end(), bookId), bookIds.end());
             }

             // Remove the book from the checked-out books file
             ifstream checkedOutFile("checkedoutBooks.txt");
             vector<string> checkedOutLines;

             if (checkedOutFile.is_open()) {
                 int patronId, checkedOutBookId;
                 while (checkedOutFile >> patronId >> checkedOutBookId) {
                     if (checkedOutBookId != bookId) {
                         stringstream ss;
                         ss << patronId << "\t" << checkedOutBookId;
                         checkedOutLines.push_back(ss.str());
                     }
                 }

                 checkedOutFile.close();

                 ofstream checkedOutWriteToFile("checkedoutBooks.txt");
                 if (checkedOutWriteToFile.is_open()) {
                     for (const string& line : checkedOutLines) {
                         checkedOutWriteToFile << line << "\n";
                     }
                     checkedOutWriteToFile.close();
                 }
                 else {
                     cout << RED << BOLD << ITALIC << "\nError opening the file to update checked-out book information.\n" << RESET;
                 }
             }
             else {
                 cout << RED << BOLD << ITALIC << "\nError opening the file to read checked-out book information.\n" << RESET;
             }
         }
         else {
             cout << RED << BOLD << ITALIC << "\nThere is no book with such ID.\n" << RESET;
         }
     }
     else {
         cout << RED << BOLD << ITALIC << "\nThere is no book with such ID.\n" << RESET;
     }

     system("pause");
 }
















};



void LOGO()
{
    cout << "\n\n";
    cout << GREEN << BOLD << "\t\t\t\t     ****************************************\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *     ----------------------------     *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *      LIBRARY MANAGEMENT SYSTEM       *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *               MADE BY                *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *        ABDUL WASEY (22K-4172)        *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *        SAAD ARSHAD (22K-4141)        *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *       MUHAMMAD HAMZA (22K-4523)      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *     ----------------------------     *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     *                                      *\n";
    cout << GREEN << BOLD << "\t\t\t\t     ****************************************\n";
}



void LoginAs()
{
    cout << UL << BOLD << ITALIC << "DO YOU WANT TO LOGIN:\n\n" << RESET;
    cout << BOLD << YELLOW << "1) " << RESET << BOLD << ITALIC << "As Librarian (Admin)\n" << RESET;
    cout << BOLD << YELLOW << "2) " << RESET << BOLD << ITALIC << "As Student\n" << RESET;
    cout << BOLD << YELLOW << "0) " << RESET << BOLD << ITALIC << "To EXIT!\n" << RESET;
    cout << BOLD << ITALIC << YELLOW << "\nEnter your choice: " << RESET;

}




void AsAdmin()
{


    cout << UL << "\n\n\t\tLOGGED IN AS AN ADMIN\n\n" << RESET;
    cout << ITALIC << BOLD << YELLOW << "Press\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "1)" << RESET << BOLD << ITALIC << " Add a book in the catalogue.\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "2)" << RESET << BOLD << ITALIC << " Remove a book from the catalogue.\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "3)" << RESET << BOLD << ITALIC << " Display the catalogue.\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "4)" << RESET << BOLD << ITALIC << " History.\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "5)" << RESET << BOLD << ITALIC << " Remove User.\n" << RESET;
    cout << YELLOW << BOLD << ITALIC << "0)" << RESET << BOLD << ITALIC << " To exit.\n" << RESET;
    cout << UL << YELLOW << ITALIC << "\nEnter your choice: " << RESET;

}


void asStudent()
{
    cout << UL << "Press\n" << RESET;
    cout << YELLOW << "1) Login\n" << RESET;
    cout << YELLOW << "2) Signup\n" << RESET;
    cout << YELLOW << "0) To go back.\n" << RESET;
    cout << YELLOW << "Enter your choice: " << RESET;


}



void  loggedIn()
{
    system("CLS");

    cout << UL << "Press\n" << RESET;
    cout << YELLOW << "1) Book catalogue\n" << RESET;
    cout << YELLOW << "2) Notification Centre\n" << RESET;
    cout << YELLOW << "3) Books To return.\n" << RESET;
    cout << YELLOW << "4) Search Books by ID.\n" << RESET;
    cout << YELLOW << "0) Log Out.\n" << RESET;
    cout << YELLOW << "Enter your choice: " << RESET;




}


int main() {
    Library lib;

    int choice1;


    LOGO();


 loginas:
    LoginAs();  // 1- Admin  2- Student  0- Exit


    cin >> choice1;

    system("CLS");



    int passCount = 0;
    while (choice1 != 0)
    {





















        if (choice1 == 1)
        {
            string pass;
            cout << YELLOW << BOLD << ITALIC << "Enter password to login as an admin (pass = admin123): " << RESET;
            cin >> pass;
            system("CLS");



            while(passCount < 5)
            {


            if (pass == "admin123")
            {
                int choice2;
                
                AsAdmin();   //1- add  2- remove   3- disp cat  4- history  0- back
                cin >> choice2;


                system("CLS");
                while (choice2 != 0)
                {

                    if (choice2 == 1)
                    {
                        //case 1:
                        lib.addBook();
                    }

                    else if (choice2 == 2)
                    {
                        //case 2:
                        system("ClS");
                        cout << UL << "\t\t\t\tBOOK REMOVAL SECTION\n";
                        int bookId;
                        cout << YELLOW << "Enter id of the book you want to remove: " << RESET;
                        cin >> bookId;
                        lib.removeBook(bookId);
                        // break;
                    }
                    else if (choice2 == 3)
                    {

                        //case 3:
                        lib.displayCatalogue();
                        // break;
                    }
                    else if (choice2 == 4)
                    {
                        lib.displayHistory();
                    }
                    

                    else if (choice2 == 0)
                    {
                        break;
                    }
                    system("CLS");
                    AsAdmin();
                    cin >> choice2;
                }

                if (choice2 == 0)
                {

                    system("CLS");
                    goto loginas;
                }
            }
            else {
                passCount++;
                system("CLS");

                cout << RED << ITALIC << BOLD << "Wrong password! Please try again.\n";
                cout << YELLOW << BOLD << ITALIC << "Enter password to login as an admin: " << RESET;
                cin >> pass;

            }
            }

            if (passCount >= 5)
            {
                system("CLS");
                goto loginas;


            }

        }




        else if (choice1 == 2) //student
        
        
        {
            //student functionality
            system("CLS");
            int studentChoice;

logout:
            asStudent();   //  1- Login   2-Signup  0- go back

            cin >> studentChoice;
            
            system("CLS");

            switch (studentChoice) 
            
            {
            case 1: 
            
            {
                int userId;
                cout << UL << "Enter your user ID: " << RESET;
                cin >> userId;

                ifstream file(lib.getFileName());
                bool userExists = false;

                if (file.is_open()) 
                {
                    Patron tempPatron;
                    while (file >> tempPatron.name >> tempPatron.id) {
                        if (tempPatron.id == userId) 
                        {
                            userExists = true;
                            cout << UL << "Login successful!\n" << RESET;
                            login2:
                            loggedIn();   //1 - book catalougue  2- noti  3- books RET   0- Logout
                            int logopt;
                            cin >> logopt;
                            while (logopt != 0)
                            {


                                if (logopt == 1)
                                {

                                   lib.checkoutProcess(tempPatron);

                                }

                                else if (logopt == 2)
                                {
                                    system("CLS");
                                    lib.notifyUsers();
                                    system("pause");

                                }


                                if (logopt == 3)
                                {
                                    system("CLS");

                                    int retId;

                                    lib.displayCheckedOutBooks(tempPatron);

                                    cout << "\n\nEnter the ID of the book you want to return (0- back): \n\n";
                                    
                                    cin >> retId;
                                    if (retId == 0)
                                    {
                                        goto login2;
                                    }
                                    lib.returnBook(retId);

                                    
                                    system("cls");
                                   
                                    //lib.returnBook(tempPatron.id, returnID);


                                }
                                if (logopt == 4)
                                {
                                    system("CLS");
                                    lib.BSTFromFile(lib.bookIdBST);
                                    int bookId;
                                    cout << "Enter the id of the book you want to search: ";
                                    cin >> bookId;
                                    lib.searchBookById(bookId);
                                    cout << "\n\n\n\n";
                                    system("pause");
                                    system("CLS");

                                }

                                if (logopt == 0)
                                {
                                    break;
                                }

                                loggedIn();
                                cin >> logopt;
                            }
                            if (logopt == 0)
                            {
                                system("CLS");
                                goto logout;
                            }



                            cout << "\n\n\n\n";
                            system("pause");
                            system("CLS");
                            break;
                        }
                    }

                    file.close();
                }
                if (userExists == false) 
                {   
                    cout << RED << ITALIC << BOLD << "User not found. Please signup.\n" << RESET;
                    system("pause");
                    system("cls");
                    goto logout;
                }

                system("pause");
                break;
            }
            case 2: {
                string userName;
                int userId;

                do {
                    cout << "Enter your name: ";
                    cin >> userName;
                    cout << "Enter your desired user ID: ";
                    cin >> userId;

                    auto existingUser = find_if(lib.patrons.begin(), lib.patrons.end(),
                        [userId](const Patron& patron) { return patron.id == userId; });

                    if (existingUser != lib.patrons.end()) {
                        cout << "User ID already exists. Please choose a different one.\n";
                    }
                    else {
                        break;
                    }
                } while (true);

                Patron newPatron(userName, userId);
                lib.addUser(newPatron);

                cout << "Signup successful! You can now login.\n";
                system("pause");
                break;
            }
            default:
                break;
            }
        }
        else {
            system("CLS");
            cout << "Invalid choice! Please try again.\n";
        }

        system("cls");
        LoginAs();
        cin >> choice1;
    



























}

    return 0;
}