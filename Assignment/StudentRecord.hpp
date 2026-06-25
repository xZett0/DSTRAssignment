#pragma once
#include <string>

using namespace std;

struct Student {
	string studentID;
	string fullName;
	string programme;
	int yearOfStudy = 1;
	float cgpa = 0.0f;
	string contactNum;
};

struct Node {
	Student data;
	Node* next;

	Node(Student s) {
		data = s;
		next = nullptr;
	}
};

class StudentRecord {
private: 
	Node* head;

public:
	StudentRecord(); // Constructor
	~StudentRecord();

	void insertAtPosition(Student s, int position, bool silent = false);
	bool deleteByID(string id, bool silent = false);

	void searchByName(string name); // should print all matches since names are not unique like ID
	Node* searchByID(string id, bool silent = false);
	string getStudentIDAt(int position);

	void displayAll(bool silent = false);
	int getTotalCount();

	void sortListByCGPA();

private:
	Node* mergeSortRec(Node* headRef);
	Node* sortedMerge(Node* a, Node* b);
	void frontBackSplit(Node* source, Node** frontRef, Node** backRef);
};
