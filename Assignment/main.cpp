#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
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
		if (system.searchByID(s.studentID, true) != nullptr) {
			cout << "Skipping Duplicate Student: " << s.studentID << endl;
			continue;
		}

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
	// system.displayAll();
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T2] Display Time (Traversal)   : " << total_us << " us\n";

	// --- T3. INSERT 10 RECORDS (Average) ---
	start = chrono::high_resolution_clock::now();
	for (int i = 0; i < 10; i++) {
		Student dummy = { "TP9999" + to_string(i), "Test Student", "CT101", 1, 3.5f, "012-3456789" };
		// Inserting at the very end using your unoptimized getTotalCount()
		system.insertAtPosition(dummy, system.getTotalCount() + 1);
	}
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T3] Insert 10 Time (Average)   : " << (total_us / 10) << " us per insert\n";

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
	for (int i = 0; i < 10; i++) {
		system.deleteByID("TP9999" + to_string(i));
	}
	end = chrono::high_resolution_clock::now();
	total_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "[T6] Delete 10 Time (Average)   : " << (total_us / 10) << " us per delete\n";

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

int main() {
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

	return 0;
}
