//============================================================================
// Name        : HashTable.cpp
// Author      : Dante Lee
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : ABCU University Course Planner
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold course information
struct Course {
    string courseID; // unique identifier
    string title;
    vector<string> prerequisites;
    Course() {}
};

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a bid and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseID);
    Course Search(string courseID);
};


// Display course info
void displayCourse(Course course) {

    cout << course.courseID << ": " << course.title << " | Prerequisites: ";

    for (string pre : course.prerequisites) {
        cout << pre << " - ";
    }

    return;
}
 

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;

    // resize nodes size
    nodes.resize(tableSize);
}


// Destructor
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.erase(nodes.begin());
}

// Calculate the hash value of a given key.
unsigned int HashTable::hash(int key) {
    return key % tableSize;
}

// Insert a Course
void HashTable::Insert(Course course) {

    // create the key for the given course
    unsigned key = hash(atoi(course.courseID.c_str()));

    // retrieve node using key
    Node* prevNode = &(nodes.at(key));

    // if no entry found for the key
    if (prevNode == nullptr) {

        // assign this node to the key position
        Node* nextNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*nextNode));
    }

    // else if node is not used
    else {

        // if node is found
        if (prevNode->key == UINT_MAX) {

            // assing old node key to UNIT_MAX, set to key, set old node to course
            // and old node next to null pointer
            prevNode->key = key;
            prevNode->course = course;
            prevNode->next = nullptr;
        }

        // else find the next open node
        else {

            while (prevNode->next != nullptr) {

                // add new newNode to end
                prevNode = prevNode->next;
            }
        }
    }
}

// Print all courses
void HashTable::PrintAll() {

    // for node begin to end iterate
    for (unsigned int i = 0; i < nodes.size(); ++i) {

        Node* currNode = &nodes.at(i);

        // if key not equal to UINT_MAX
        if (nodes.at(i).key != UINT_MAX) {

            // while node not equal to nullptr
            while (currNode->next != nullptr) {

                // output courseID, title, and prerequisites
                displayCourse(currNode->course);

                // node is equal to next node
                currNode = currNode->next;
            }
        }
    }
}

// Remove a course
void HashTable::Remove(string courseId) {

    // set key equal to hash atoi courseID cstring
    unsigned key = hash(atoi(courseId.c_str()));

    // erase node begin and key
    nodes.erase(nodes.begin() + key);
}

// Search for a specified course by ID
Course HashTable::Search(string courseId) {
    Course course;

    // create the key for the given bid
    unsigned key = hash(atoi(courseId.c_str()));

    Node* node = &(nodes.at(key));

    // if entry found for the key
    if (node != nullptr && node->key != UINT_MAX
        && node->course.courseID.compare(courseId) == 0) {

        //return node bid
        return node->course;
    }

    // if no entry found for the key
    if (node == nullptr || node->key == UINT_MAX) {

        // return bid
        return course;
    }

    // while node not equal to nullptr
    while (node != nullptr) {

        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseID.compare(courseId) == 0) {
            return node->course;
        }

        //node is equal to next node
        node = node->next;
    }

    return course;
}


// Load CSV file into container
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of courses
            Course course;
            course.courseID = file[i][0];
            course.title = file[i][1];
            
            // Add all remaining fields to the prerequisites list
            for (unsigned int j = 2; j < file[i].size(); j++) {
                course.prerequisites.push_back(file[i][j]);
            }

            // cout << "Course ID: " << course.courseID << ", Title: " << course.title << endl;

            // add course to hash table
            hashTable->Insert(course);
        }

    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}


// Convert a string to a double
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}


void displayCourse(Course course);
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        break;
    case 3:
        csvPath = argv[1];
        break;
    default:
        csvPath = "ABCU_Advising_Program_Input.csv";
    }

    // Define a hash table to hold all the bids
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();
    
    int choice = 0;
    string course_ID;

    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        // Load hash table with courses from file
        case 1:
            loadCourses(csvPath, courseTable);
            break;

        // Print all courses
        case 2:
            courseTable->PrintAll();
            break;

        // Print specific course
        case 3:

            // Get desired course ID
            cout << "Enter the course ID: ";
            cin >> course_ID;

            // Search for course
            course = courseTable->Search(course_ID);

            // Display course info if found
            if (!course.courseID.empty()) {
                displayCourse(course);
            }
            else {
                cout << "Course ID " << course_ID << " not found." << endl;
            }

            break;
        }
    }

    cout << "Thank you for using the course planner!" << endl;

    return 0;
}
