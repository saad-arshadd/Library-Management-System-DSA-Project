# 📚 Library Management System

A console-based Library Management System implemented in **C++**, developed as part of a **Data Structures and Algorithms (DSA)** course project. This system allows librarians and students to manage books, users, checkout records, and dependencies using core data structures for optimal performance.

---

## 🚀 Features

### 🧑‍💼 Admin (Librarian)
- Add / Remove books from the catalogue
- View book catalogue
- View action history (admin activity)
- Remove users
- Set due dates for borrowed books

### 👨‍🎓 Student (User)
- Login / Signup
- Browse book catalogue
- Checkout books (if available)
- Return books
- View due date notifications
- Request unavailable books (queued by priority)
- Search books by ID

---

## 🧠 Data Structures Used

| Data Structure | Purpose |
|----------------|---------|
| **Binary Search Tree (BST)** | Fast lookup and insertion of books by ID |
| **Stack** | Store admin activities & checkout history |
| **Queue** | Manage waitlist for requested books |
| **Graph (Adjacency List)** | Track dependencies between books |
| **Hash Maps (unordered_map)** | Constant time access for availability, checkout status |
| **File I/O** | Persistent storage of users and books |

---

## 🗂️ File Structure

- `LIBRARY_MANAGEMENT_SYSTEM.cpp` – Main source file
- `books.txt` – Stores book data
- `patrons.txt` – Stores user data
- `checkedoutBooks.txt` – Records of checked out books

---

## 🛠️ How to Run

1. Open the project in a C++ compiler (Dev-C++, Visual Studio, etc.)
2. Compile and run `LIBRARY_MANAGEMENT_SYSTEM.cpp`
3. Follow the on-screen menu for **Admin** or **Student** login

> ⚠️ Note: Ensure `books.txt` and `patrons.txt` are in the same directory for file operations.

---

## 📷 Screenshots (Optional)

_Add screenshots here if needed._

---

## ✅ Conclusion

This project demonstrates practical implementation of DSA concepts in a real-world system. It provides a foundational structure for more complex systems like library or inventory management applications.

---

