#include <iostream>
#include <iomanip>
#include "StudentRecord.hpp"

using namespace std;

StudentRecord::StudentRecord() {
	head = nullptr;
}

StudentRecord::~StudentRecord() {
	Node* curr = head; 
	while (curr != nullptr) {
		Node* nextNode = curr->next;
		delete curr;
		curr = nextNode;
	}

	head = nullptr;
}


void StudentRecord::insertAtPosition(Student s, int position, bool silent) {
	if (position < 1) {
		cout << "Insertion out of range for " << s.studentID << endl;
		return;
	}

	if (searchByID(s.studentID, true) != nullptr) {
		if (!silent) cout << "[Insert Error] Duplicate Student ID " << s.studentID << " rejected!\n";
		return; 
	}

	Node* newNode = new Node(s);
	
	// our linked-list is empty or inserting at first position
	if (position == 1 || head == nullptr) {
		newNode->next = head;
		head = newNode;
		return;
	}
	
	// inserting in the middle or end
	Node* runner = head;
	int currentPos = 1;

	while (runner->next != nullptr && currentPos < position - 1) {
		runner = runner->next;
		currentPos++;
	}

	newNode->next = runner->next;
	runner->next = newNode;
}

bool StudentRecord::deleteByID(string id, bool silent) {
	if (head == nullptr) {
		if (!silent) cout << "[Delete Error] Cannot delete from an empty database.\n";
		return false;
	}
	
	if (head->data.studentID == id) {
		Node* nodeToDelete = head;
		head = head->next;
		delete nodeToDelete;
		return true;
	}

	Node* prev = head;
	Node* curr = head->next;

	while (curr != nullptr) {
		if (curr->data.studentID == id) {
			prev->next = curr->next;
			delete curr;
			return true;
		}

		prev = curr;
		curr = curr->next;
	}

	if (!silent) cout << "[Delete Result] Student ID " << id << " not found. No deletion made.\n";
	return false;
}

void StudentRecord::searchByName(string name) {
	if (head == nullptr) {
		cout << "[Search Error] Cannot search an empty database.\n";
		return;
	}

	Node* runner = head;
	bool foundAny = false;

	cout << "\n--- Search Results for Name: \"" << name << "\" ---\n";

	while (runner != nullptr) {
		if (runner->data.fullName == name) {
			cout << left << setw(12) << runner->data.studentID
				<< setw(30) << runner->data.fullName
				<< fixed << setprecision(2) << " | CGPA: " << runner->data.cgpa << "\n";
			foundAny = true;
		}

		runner = runner->next;
	}

	if (!foundAny) {
		cout << "No student records found with the name \"" << name << "\".\n";
	}
	cout << "--------------------------------------------------\n";
}

Node* StudentRecord::searchByID(string id, bool silent) {
	if (head == nullptr) {
		if (!silent) cout << "[Search Error] Cannot search an empty database.\n";
		return nullptr;
	}

	Node* runner = head;
	while (runner != nullptr) {
		if (runner->data.studentID == id) {
			return runner;
		}
		runner = runner->next;
	}

	if (!silent) cout << "[Search Result] Student ID: " << id << " does not exist.\n";
	return nullptr;
}

string StudentRecord::getStudentIDAt(int position) {
	Node* runner = head;
	int currPos = 1;

	while (runner != nullptr && currPos < position) {
		runner = runner->next;
		currPos++;
	}

	if (runner != nullptr) {
		return runner->data.studentID;
	}
	return "";
}

void StudentRecord::displayAll(bool silent) {
	if (head == nullptr) {
		if (!silent) cout << "System holds 0 records. List is empty";
		return;
	}
	
	if (!silent) {
		cout << "\n" << string(85, '=') << "\n";
		cout << left
			<< setw(12) << "Student ID"
			<< setw(30) << "Full Name"
			<< setw(12) << "Programme"
			<< setw(10) << "Year"
			<< setw(8) << "CGPA"
			<< setw(15) << "Contact Num" << "\n";
		cout << string(85, '-') << "\n";
	}

	Node* runner = head;
	while (runner != nullptr) {
		if (!silent) {
			cout << left
				<< setw(12) << runner->data.studentID
				<< setw(30) << runner->data.fullName
				<< setw(12) << runner->data.programme
				<< setw(10) << runner->data.yearOfStudy;

			cout << fixed << setprecision(2) << setw(8) << runner->data.cgpa;
			cout << setw(15) << runner->data.contactNum << "\n";
		}

		runner = runner->next;
	}

	if (!silent) cout << string(85, '=') << "\n\n";
}

int StudentRecord::getTotalCount() {
	int total = 0;
	Node* runner = head;
	
	while (runner != nullptr) {
		total++;
		runner = runner->next;
	}

	return total;
}

void StudentRecord::sortListByCGPA() {
	if (head == nullptr || head->next == nullptr) {
		cout << "[Sort Error] Can not sort empty database.\n";
		return;
	}

	head = mergeSortRec(head);

	Node* runner = head;
}

Node* StudentRecord::mergeSortRec(Node* headRef) {
	if (headRef == nullptr || headRef->next == nullptr) {
		return headRef;
	}

	Node* a = nullptr;
	Node* b = nullptr;

	frontBackSplit(headRef, &a, &b);

	a = mergeSortRec(a);
	b = mergeSortRec(b);

	return sortedMerge(a, b);
}

Node* StudentRecord::sortedMerge(Node* a, Node* b) {
	if (a == nullptr) return b;
	if (b == nullptr) return a;

	Node* result = nullptr;

	if (a->data.cgpa <= b->data.cgpa) {
		result = a;
		a = a->next;
	}
	else {
		result = b;
		b = b->next;
	}

	Node* mergeTail = result;

	while (a != nullptr && b != nullptr) {
		if (a->data.cgpa <= b->data.cgpa) {
			mergeTail->next = a;
			a = a->next;
		}
		else {
			mergeTail->next = b;
			b = b->next;
		}

		mergeTail = mergeTail->next;
	}

	if (a != nullptr) {
		mergeTail->next = a;
	}
	else {
		mergeTail->next = b;
	}

	return result;
}

void StudentRecord::frontBackSplit(Node* source, Node** frontRef, Node** backRef) {
	if (source == nullptr || source->next == nullptr) {
		*frontRef = source;
		*backRef = nullptr;
		return;
	}

	int length = 0;
	Node* counter = source;
	while (counter != nullptr) {
		length++;
		counter = counter->next;
	}

	int midpoint = length / 2;
	if (length % 2 != 0) {
		midpoint++;
	}

	Node* curr = source;
	for (int i = 1; i < midpoint; i++) {
		curr = curr->next;
	}
	
	*frontRef = source;
	*backRef = curr->next;
	curr->next = nullptr;
}



