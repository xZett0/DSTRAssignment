#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits> 
#include <iomanip>
#include "StudentRecord.hpp"

using namespace std;

struct ProgrammeDetails {
	string code;
	string name;
	string faculty;
	int durationYears = -1;
};

ProgrammeDetails validProgrammes[50];
int numProgrammes = 0;

void loadProgrammesTable() {
	string filename = ".\\data\\programmes.csv";
	ifstream file;

	file.open(filename);

	if (!file.is_open()) {
		file.clear();
		filename = "programmes.csv";
		file.open(filename);
	}

	if (!file.is_open()) {
		cout << "[System] Warning: Could not open programmes.csv. Advanced validation disabled.\n";
		return;
	}

	string line;
	getline(file, line); 

	while (getline(file, line) && numProgrammes < 50) {
		stringstream ss(line);
		string token;

		getline(ss, token, ',');
		validProgrammes[numProgrammes].code = token;

		getline(ss, token, ',');
		validProgrammes[numProgrammes].name = token;

		getline(ss, token, ',');
		validProgrammes[numProgrammes].faculty = token;

		getline(ss, token, ',');
		try {
			validProgrammes[numProgrammes].durationYears = stoi(token);
		}
		catch (...) {
			validProgrammes[numProgrammes].durationYears = 4; 
		}

		numProgrammes++;
	}
	file.close();
	cout << "[System] Loaded " << numProgrammes << " valid programmes for relational reference.\n";
}
int getProgrammeDuration(string code) {
	if (numProgrammes == 0) return 5;
	for (int i = 0; i < numProgrammes; i++) {
		if (validProgrammes[i].code == code) {
			return validProgrammes[i].durationYears;
		}
	}
	return -1;
}

void displayAllProgrammes() {
	cout << "\n======================================================\n";
	cout << "           VALID PROGRAMMES REFERENCE TABLE           \n";
	cout << "======================================================\n";

	if (numProgrammes == 0) {
		cout << "[System Error] No programmes loaded. Check programmes.csv.\n";
		return;
	}

	// Setting up clean columns
	cout << left << setw(12) << "Code" << setw(15) << "Max Years" << "Faculty\n";
	cout << string(70, '-') << "\n";

	for (int i = 0; i < numProgrammes; i++) {
		cout << left << setw(12) << validProgrammes[i].code
			<< setw(15) << validProgrammes[i].durationYears
			<< validProgrammes[i].faculty << "\n";
	}
	cout << "======================================================\n";
}

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
			if (!token.empty() && token.back() == '\r') {
				token.pop_back();
			}

			s.contactNum = token;

			if (getProgrammeDuration(s.programme) == -1) {
				throw invalid_argument("Invalid Programme Code");
			}

			system.insertAtPosition(s, system.getTotalCount() + 1, true);
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

	// --- TEST 1: EMPTY DATABASE PROTECTIONS ---
	cout << "\n[TEST 1] Empty Database Protections:\n";
	cout << "-> Attempting to Search ID 'TP123456'...\n";
	system.searchByID("TP123456");

	cout << "-> Attempting to Delete ID 'TP123456'...\n";
	system.deleteByID("TP123456");

	// --- TEST 2: OUT OF BOUNDS INSERTION ---
	cout << "\n[TEST 2] Out-of-Bounds Insertion Protection:\n";
	Student dummy1 = { "TP000001", "Bounds Tester", "CT101", 1, 3.0f, "012-345" };
	cout << "-> Attempting to insert at Position 5 (List is currently empty)...\n";
	system.insertAtPosition(dummy1, 5);

	// Load data for the remaining tests
	cout << "\n[System] Loading students_500.csv for further tests...\n";
	loadCSVData("students_500.csv", system);

	// --- TEST 3: NON-EXISTENT RECORDS ---
	cout << "\n[TEST 3] Non-Existent Records in Populated Database:\n";
	cout << "-> Attempting to Search missing ID 'TP999999'...\n";
	system.searchByID("TP999999");

	cout << "-> Attempting to Delete missing ID 'TP999999'...\n";
	system.deleteByID("TP999999");

	// --- TEST 4: DUPLICATE ID INSERTION (The Safety Tax) ---
	cout << "\n[TEST 4] Duplicate ID Protection (The 'Safety Tax'):\n";
	Student dummy2 = { "TP888888", "Duplicate Tester", "CT101", 1, 4.0f, "012-345" };

	cout << "-> Attempting first insertion of TP888888 (Should Succeed)...\n";
	if (system.insertAtPosition(dummy2, 1)) cout << "[Insert Result] Success!\n";

	cout << "-> Attempting duplicate insertion of TP888888 (Should be blocked)...\n";
	system.insertAtPosition(dummy2, 2);

	// --- TEST 5: RELATIONAL DATA INTEGRITY ---
	cout << "\n[TEST 5] Relational Data & Bounds Validation (Menu Simulation):\n";

	string testProg = "JEDI101";
	cout << "-> Validating Fake Programme '" << testProg << "':\n   Result: ";
	int duration = getProgrammeDuration(testProg);
	if (duration == -1) cout << "REJECTED (Not found in programmes.csv reference table)\n";

	testProg = "BM102"; 
	int testYear = 4;
	cout << "-> Validating Year " << testYear << " for 3-Year Programme '" << testProg << "':\n   Result: ";
	duration = getProgrammeDuration(testProg);
	if (testYear > duration) cout << "REJECTED (Exceeds max duration of " << duration << " years)\n";

	float testCGPA = 4.5f;
	cout << "-> Validating CGPA " << testCGPA << ":\n   Result: ";
	if (testCGPA < 0.0f || testCGPA > 4.0f) cout << "REJECTED (Strictly bounded between 0.0 and 4.0)\n";

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
	loadProgrammesTable();

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
		cout << "9. View Valid Programmes Reference Table\n"; 
		cout << "10. Show Edge Case and Error Handling\n"; 
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
			while (true) {
				getline(cin, s.fullName);
				if (s.fullName.empty() || s.fullName.find_first_not_of(' ') == string::npos) {
					cout << "Error: Name cannot be empty. Please enter a valid Full Name: ";
				}
				else {
					break;
				}
			}

			int maxYears = -1;
			cout << "Programme Code (e.g., CT101): ";
			while (true) {
				getline(cin, s.programme);
				maxYears = getProgrammeDuration(s.programme);
				if (maxYears != -1) {
					break;
				}
				else {
					cout << "Error: '" << s.programme << "' is not a recognized programme.\n";
					cout << "Please enter a valid Programme Code: ";
				}
			}

			cout << "Year of Study (1 - " << maxYears << "): ";
			while (!(cin >> s.yearOfStudy) || s.yearOfStudy < 1 || s.yearOfStudy > maxYears || cin.peek() == '.') {
				cout << "Invalid input. This programme has a max duration of " << maxYears << " years.\n";
				cout << "Please enter a whole number between 1 and " << maxYears << ": ";
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
		}		case 4: {
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
		case 9: {
			displayAllProgrammes();
			break;
		}
		case 10: {
			showEdgeCaseHandling();
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
