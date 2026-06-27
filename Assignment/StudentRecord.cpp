#include <iostream>
#include <iomanip>
#include "StudentRecord.hpp"

using namespace std;

StudentRecord::StudentRecord() {
	head = nullptr;
	tail = nullptr;
	count = 0;
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


bool StudentRecord::insertAtPosition(Student s, int position, bool silent) {
	if (position < 1 || position > count + 1) {
		if (!silent) cout << "[Insert Error] Position " << position << " is out of bounds! Valid range is 1 to " << (count + 1) << ".\n";
		return false;
	}
	if (searchByID(s.studentID, true) != nullptr) {
		if (!silent) cout << "[Insert Error] Duplicate Student ID " << s.studentID << " rejected!\n";
		return false; 
	}

	Node* newNode = new Node(s);
	newNode->data = s;
	newNode->next = nullptr;
	
	if (head == nullptr) { // inserting into an empty list
		head = newNode;
		tail = newNode;
		count++;
		return true;
	}
	if (position == 1) { // inserting at first position
		newNode->next = head;
		head = newNode;
		count++;
		return true;
	}
	if (position == count + 1) { // inserting at last position
		tail->next = newNode;
		tail = newNode;
		count++;
		return true;
	}

	Node* runner = head; // inserting in the middle
	for (int i = 1; i < position - 1; i++) {
		runner = runner->next;
	}

	newNode->next = runner->next;
	runner->next = newNode;
	count++;

	return true;
}

bool StudentRecord::deleteByID(string id, bool silent) {
	if (head == nullptr) {
		if (!silent) cout << "[Delete Error] Cannot delete from an empty database.\n";
		return false;
	}
	
	if (head->data.studentID == id) {
		Node* temp = head;
		head = head->next;
		if (head == nullptr) tail = nullptr; // list is now empty tail = nullptr
		delete temp;
		count--;
		return true;
	}
	
	Node* runner = head;
	while (runner->next != nullptr && runner->next->data.studentID != id) {
		runner = runner->next;
	}

	if (runner->next != nullptr) {
		Node* temp = runner->next;
		runner->next = temp->next;

		if (runner->next == nullptr) {
			tail = runner;
		}

		delete temp;
		count--;
		return true;
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
	return count;
}

void StudentRecord::sortListByCGPA() {
	if (head == nullptr || head->next == nullptr) {
		cout << "[Sort Error] Can not sort empty database.\n";
		return;
	}

	head = mergeSortRec(head);

	Node* runner = head;
	while (runner->next != nullptr) {
		runner = runner->next;
	}
	tail = runner;
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

	if (a->data.cgpa >= b->data.cgpa) {
		result = a;
		a = a->next;
	}
	else {
		result = b;
		b = b->next;
	}

	Node* mergeTail = result;

	while (a != nullptr && b != nullptr) {
		if (a->data.cgpa >= b->data.cgpa) {
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

	Node* slow = source;
	Node* fast = source->next;

	while (fast != nullptr) {
		fast = fast->next;
		if (fast != nullptr) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	*frontRef = source;
	*backRef = slow->next;
	slow->next = nullptr;
}



