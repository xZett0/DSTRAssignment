#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits> 
#include "StudentRecord.hpp"

using namespace std;

void loadCSVData(string filename, StudentRecord& system) {
	if (filename.find("data") == string::npos) {
		filename = ".\\data\\" + filename;
	}

	ifstream file(filename);
	string line;

	if (!file.is_open()) {
		cout << "Error: Could not open file " << filename << endl;
		return;
	}

	getline(file, line); // get rid of the header (column names)
	
	while (getline(file, line)) {
		stringstream ss(line);
		string token;
		Student s;

		getline(ss, token, ','); // StudentID
		s.studentID = token;

		getline(ss, token, ','); // FullName
		s.fullName= token;

		getline(ss, token, ','); // Programme
		s.programme = token;

		// for number stuff

		try {
			getline(ss, token, ','); // YearOfStudy
			s.yearOfStudy = stoi(token); 
	
			getline(ss, token, ','); // CGPA
			float parsedCGPA = stof(token);
			if (parsedCGPA < 0.0f || parsedCGPA > 4.0f) {
				throw out_of_range("CGPA Out of Bounds");
			}
			s.cgpa = parsedCGPA;

			getline(ss, token, ','); // ContactNumber
			s.contactNum = token;
			
			// finally insert the data 
			system.insertAtPosition(s, system.getTotalCount() + 1);

		}
		catch (...) {
			cout << "Bad line skipped for ID: " << s.studentID << "\n";
			continue;
		}
	}

	file.close();
	cout << "Finished loading " << filename << endl;
}

void runPerformanceTests(string filename) {
	StudentRecord system;
	long long total_us;

	cout << "\n======================================================\n";
	cout << "   RUNNING PERFORMANCE TESTS FOR: " << filename << "\n";
	cout << "======================================================\n";

	auto start = chrono::high_resolution_clock::now();
	loadCSVData(filename, system);
	auto end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T1] Load Time (n records)      : " << total_us << " us\n";

	// --- T2. DISPLAY ALL ---
	start = chrono::high_resolution_clock::now();
	system.displayAll(true);
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T2] Display Time (Traversal)   : " << total_us << " us\n";

	// --- T3. INSERT x RECORDS (Average) ---
	int number = 10000;
	start = chrono::high_resolution_clock::now();
	for (int i = 0; i < number; i++) {
		Student dummy = { "TP9999" + to_string(i), "Test Student", "CT101", 1, 3.5f, "012-3456789" };
		// Inserting at the very end using your unoptimized getTotalCount()
		system.insertAtPosition(dummy, system.getTotalCount() + 1);
	}
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T3] Insert " << number << " Time (Average)   : " << (total_us / number) << " us per insert\n";

	// --- T4. SEARCH 3 IDs (Average) ---
	// 1. Dynamically find the real First, Middle, and Last IDs for THIS specific file
	int currentTotal = system.getTotalCount();
	string idFirst = system.getStudentIDAt(1);
	string idMid = system.getStudentIDAt(currentTotal / 2);
	string idLast = system.getStudentIDAt(currentTotal);

	// 2. Run the Edge Case (Untimed) to prove error handling works
	system.searchByID("TP000001");

	// 3. Time the actual searches
	start = chrono::high_resolution_clock::now();

	system.searchByID(idFirst, true); // Best Case O(1)
	system.searchByID(idMid, true);   // Average Case O(n/2)
	system.searchByID(idLast, true);  // Worst Case O(n)

	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T4] Linear Search (Average)    : " << (total_us / 3) << " us per search\n";

	// --- T5. SORT (CGPA) ---
	start = chrono::high_resolution_clock::now();
	system.sortListByCGPA();
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T5] Merge Sort Time            : " << total_us << " us\n";

	// --- T6. DELETE 10 RECORDS (Average) ---
	start = chrono::high_resolution_clock::now();
	for (int i = 0; i < number; i++) {
		system.deleteByID("TP9999" + to_string(i));
	}
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T6] Delete " << number << " Time (Average)   : " << (total_us / number) << " us per delete\n";

	// --- T7. COUNT ---
	start = chrono::high_resolution_clock::now();
	currentTotal = system.getTotalCount();
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T7] Count Time (" << currentTotal << " nodes)   : " << total_us << " us\n";

	cout << "======================================================\n";
}

void showEdgeCaseHandling() {
	StudentRecord system;

	cout << "\n======================================================\n";
	cout << "         TESTING ERROR HANDLING & EDGE CASES          \n";
	cout << "======================================================\n";

	// --- EDGE CASE 1: EMPTY STRUCTURE OPERATIONS ---
	cout << "\n[TEST 1] Operations on Empty Database:\n";
	system.searchByID("TP123456");
	system.deleteByID("TP123456");

	// Load data for the remaining tests
	cout << "\n[System] Loading students_500.csv for further tests...\n";
	loadCSVData("students_500.csv", system);

	// --- EDGE CASE 2: NON-EXISTENT RECORDS ---
	cout << "\n[TEST 2] Non-Existent Records in Populated Database:\n";
	system.searchByID("TP000001");
	system.deleteByID("TP000001");

	// --- EDGE CASE 3: DUPLICATE ID INSERTION ---
	cout << "\n[TEST 3] Duplicate ID Insertion:\n";
	Student dummy = { "TP999999", "Duplicate Tester", "CT101", 1, 4.0f, "012-345" };

	cout << "-> Attempting first insertion (Should Succeed silently)...\n";
	system.insertAtPosition(dummy, 1);

	cout << "-> Attempting duplicate insertion (Should throw custom error)...\n";
	system.insertAtPosition(dummy, 2);

	cout << "\n======================================================\n";
}

void proofMergeWorks() {
	StudentRecord visualTest;
	cout << "--- LOADING 500 STUDENTS ---\n";
	loadCSVData("students_500.csv", visualTest);

	cout << "\n--- SORTING BY CGPA ---\n";
	visualTest.sortListByCGPA();

	cout << "\n--- DISPLAYING TOP RESULTS ---\n";
	visualTest.displayAll();
}

void runAllBenchmarks() {
	string files[] = {
		"students_500.csv",
		"students_2000.csv",
		"students_8000.csv",
		"students_30000.csv"
	};

	for (int i = 0; i < 4; i++) {
		runPerformanceTests(files[i]);
	}

	showEdgeCaseHandling();
	cout << endl << endl;
	proofMergeWorks();
	cout << endl;
}

int main() {
	StudentRecord activeSystem;
	int choice = -1;

	while (choice != 0) {
		cout << "\n======================================================\n";
		cout << "      STUDENT RECORD MANAGEMENT SYSTEM (LINKED LIST)  \n";
		cout << "======================================================\n";
		cout << "1. Load Data from CSV\n";
		cout << "2. Display All Students\n";
		cout << "3. Insert New Student\n";
		cout << "4. Delete Student by ID\n";
		cout << "5. Search Student by ID\n";
		cout << "6. Search Student by Name\n";
		cout << "7. Sort Database by CGPA\n";
		cout << "8. Run Automated Performance Benchmarks\n";
		cout << "0. Exit System\n";
		cout << "======================================================\n";
		cout << "Enter your choice: ";

		if (!(cin >> choice)) {
			cout << "Invalid input. Please enter a number.\n";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		switch (choice) {
		case 1: {
			string filename;
			cout << "Enter filename (e.g., students_500.csv): ";
			cin >> filename;
			loadCSVData(filename, activeSystem);
			break;
		}
		case 2: {
			activeSystem.displayAll();
			cout << "\nTotal Records: " << activeSystem.getTotalCount() << "\n";
			break;
		}
		case 3: {
			Student s;
			cout << "\n--- Enter New Student Details ---\n";
			cout << "Student ID (e.g., TP012345): ";
			cin >> s.studentID;

			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			cout << "Full Name: ";
			while (getline(cin, s.fullName) && s.fullName.empty()) {
				cout << "Name cannot be empty. Full Name: ";
			}

			cout << "Programme (e.g., CT101): ";
			while (getline(cin, s.programme) && s.programme.empty()) {
				cout << "Programme name cannot be empty. Programme (e.g., CT101): ";
			}

			cout << "Year of Study (1 - 5): ";
			while (!(cin >> s.yearOfStudy) || s.yearOfStudy < 1 || s.yearOfStudy > 5 || cin.peek() == '.') {
				cout << "Invalid input. Please enter a whole number between 1 and 5: ";
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			cout << "CGPA (0.0 - 4.0): ";
			while (!(cin >> s.cgpa) || s.cgpa < 0.0f || s.cgpa > 4.0f) {
				cout << "Invalid CGPA. Please enter a decimal between 0.0 and 4.0: ";
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			cout << "Contact Number: ";
			getline(cin, s.contactNum);

			if (activeSystem.insertAtPosition(s, activeSystem.getTotalCount() + 1)) {
				cout << "Insertion attempt complete. Student added successfully.\n";
			}
			break;
		}
		case 4: {
			string id;
			cout << "Enter Student ID to delete: ";
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover enters
			getline(cin, id); // Grabs the whole line safely
			if (activeSystem.deleteByID(id)) {
				cout << "Successfully deleted " << id << "\n";
			}
			break;
		}
		case 5: {
			string id;
			cout << "Enter Student ID to search: ";
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, id);
			Node* found = activeSystem.searchByID(id);
			if (found != nullptr) {
				cout << "\n--- Student Found ---\n";
				cout << "Name:  " << found->data.fullName << "\n";
				cout << "ID:    " << found->data.studentID << "\n";
				cout << "Prog:  " << found->data.programme << " (Year " << found->data.yearOfStudy << ")\n";
				cout << "CGPA:  " << found->data.cgpa << "\n";
				cout << "Phone: " << found->data.contactNum << "\n";
			}
			break;
		}
		case 6: {
			string name;
			cout << "Enter Full Name to search (exact match): ";
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, name);
			activeSystem.searchByName(name);
			break;
		}
		case 7: {
			cout << "Sorting records by CGPA...\n";
			activeSystem.sortListByCGPA();
			cout << "Sorting complete! Select option 2 to view the sorted list.\n";
			break;
		}
		case 8: {
			cout << "\nWARNING: This will run tests on separate instances and may take some time.\n";
			runAllBenchmarks();
			break;
		}
		case 0: {
			cout << "Exiting system. Goodbye!\n";
			break;
		}
		default: {
			cout << "Invalid choice. Please select a valid option.\n";
			break;
		}
		}
	}

	return 0;
}
