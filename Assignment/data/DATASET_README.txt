DATASET DESIGN NOTES — CT077-3-2-DSTR Lab Work #1
====================================================

FILES INCLUDED
--------------
  students_500.csv    —     500 student records  (Level 1 — warm-up)
  students_2000.csv   —   2,000 student records  (Level 2 — gap appears)
  students_8000.csv   —   8,000 student records  (Level 3 — gap is clear)
  students_30000.csv  —  30,000 student records  (Level 4 — gap is dramatic)
  programmes.csv      —      25 programme records

====================================================
WHY FOUR SIZES?  —  Big-O is about GROWTH RATE, not a single time
====================================================
Big-O notation (O(n), O(n log n), O(n^2), O(log n)) describes how an
algorithm's running time GROWS as input size n increases.
A single dataset cannot prove Big-O — you need to measure at multiple
sizes and observe the growth pattern.

Run your implementation on each file and record the time. Then answer:
  "When n quadrupled from 2,000 to 8,000, did my sort time also ~16x?
   That would confirm O(n^2). Did it only ~4x? That confirms O(n log n)."

====================================================
THEORETICAL OPERATION COUNTS (to guide your Big-O analysis)
====================================================

  n          n^2 ops        n*log2(n) ops   log2(n) steps   O(n^2) / O(nlogn)
  -------    -----------    -------------   -------------   -----------------
     500         250,000            4,482               8            ~56x slower
   2,000       4,000,000           21,931              10           ~182x slower
   8,000      64,000,000          103,726              12           ~617x slower
  30,000     900,000,000          446,180              14          ~2017x slower

KEY INSIGHT: An O(n^2) sort at n=30,000 requires 900 MILLION comparisons.
An O(n log n) sort at the same n requires only 446,180 comparisons.
O(n^2) is more than 2,000x slower at n=30,000 — this WILL be visible in
your measured times even on fast hardware.

====================================================
STUDENTS.CSV COLUMNS
====================================================
  StudentID      — unique TP number (e.g., TP154321), SHUFFLED (not sorted)
                   IDs are NOT sequential — ensures your sort and search
                   algorithms perform realistic work
  FullName       — Malaysian names (Malay ~55%, Chinese ~30%, Indian ~15%)
  ProgrammeCode  — references programmes.csv
  YearOfStudy    — 1 to programme duration (1–4)
  CGPA           — realistic bell-curve distribution:
                     ~5%   3.75–4.00  (Distinction)
                     ~15%  3.50–3.74  (High Credit)
                     ~25%  3.00–3.49  (Credit)
                     ~30%  2.50–2.99  (Pass)
                     ~17%  2.00–2.49  (Low Pass)
                     ~8%   1.00–1.99  (At Risk)
  ContactNumber  — Malaysian mobile format (e.g., 012-3456789)

IMPORTANT: IDs are shuffled — do NOT assume the data is pre-sorted.
Your sort algorithm must actually sort it.

====================================================
PROGRAMMES.CSV COLUMNS (25 records, small reference table)
====================================================
  ProgrammeCode  — CT101–CT107, BM101–BM105, EE101–EE105, DT101–DT103,
                   HS101–HS103, PM101–PM102
  ProgrammeName  — Full official programme title
  Faculty        — 6 schools (Computing, Business, Engineering, Design,
                   Humanities, Professional Development)
  DurationYears  — 1 (Foundation), 2 (Diploma), 3–4 (Degree)

====================================================
HOW TO USE THE 4 FILES: STEP-BY-STEP FOR YOUR REPORT
====================================================
Step 1 — Load each file into your data structure (Array or Linked List).
Step 2 — For EACH operation, time it at all 4 sizes.
Step 3 — Record times in your Individual Performance Record table.
Step 4 — Calculate the growth ratio between sizes.
Step 5 — Match the ratio to a Big-O class (see table below).

MEASURING TIP (C++):
  #include <chrono>
  auto start = chrono::high_resolution_clock::now();
  // ... your operation here ...
  auto end   = chrono::high_resolution_clock::now();
  auto ms    = chrono::duration_cast<chrono::microseconds>(end-start).count();
  cout << "Time: " << ms << " microseconds" << endl;

  Use microseconds (not milliseconds) — some operations at n=500 will
  be sub-millisecond and would round to 0ms.

====================================================
EXPECTED GROWTH RATIOS — HOW TO CONFIRM YOUR BIG-O
====================================================
Compare time at n=2000 vs n=500 (4x increase in n):

  If time grew by ~4x    → confirms O(n)        [linear search, display]
  If time grew by ~16x   → confirms O(n^2)       [bubble/selection sort]
  If time grew by ~4.9x  → confirms O(n log n)   [merge/quick sort]
  If time barely changed → confirms O(log n)      [binary search]
  If time barely changed → confirms O(1)          [insert at head in LL]

Compare time at n=8000 vs n=500 (16x increase in n):

  If time grew by ~16x   → O(n)
  If time grew by ~256x  → O(n^2)   ← this will be VERY obvious
  If time grew by ~21x   → O(n log n)
  If time barely changed → O(log n) or O(1)

====================================================
RECOMMENDED TEST SEQUENCE
====================================================
For each of the 4 files (n = 500 / 2000 / 8000 / 30000):
  T1. Load:      Load all n records into your structure.  Time it.
  T2. Display:   Traverse and print all records.          Time it.
  T3. Add:       Insert 10 new records. Average the time.
  T4. Search(L): Linear search for 3 IDs — one near start,
                 one in the middle, one near the end.     Average.
  T5. Sort:      Sort all n records by CGPA ascending.   Time it.
  T6. Search(B): Binary search (on sorted data) for same
                 3 IDs. Average. Compare with T4.
  T7. Delete:    Delete 10 records by StudentID. Average.
  T8. Count:     (Linked List only) Count total nodes.   Time it.

TEST IDs TO SEARCH FOR (valid across all files):
  These IDs appear in all four dataset files:
  Search for StudentIDs near the END of the loaded list to get
  worst-case O(n) linear search times for the most informative
  measurement. Pick 1 ID from the first row of your loaded data,
  1 from the middle, and 1 from the last row.

EDGE CASES TO TEST (use these in addition to normal records):
  - Search for "TP000001"   → does not exist  (tests not-found path)
  - Insert a record with CGPA = 4.00          (tests maximum boundary)
  - Insert a record with CGPA = 1.00          (tests minimum boundary)
  - Delete a record twice                     (tests duplicate delete)
  - Search on an empty structure              (tests empty edge case)
  - Insert 1 record, then delete it, then display (tests empty-after-delete)

====================================================
YOUR INDIVIDUAL PERFORMANCE RECORD TABLE FORMAT
====================================================
Fill in actual measured times at each n. Leave N/A where operation
does not apply to your implementation type.

Operation       | n=500 | n=2000 | n=8000 | n=30000 | Big-O  | Justification
----------------|-------|--------|--------|---------|--------|---------------
Add/Insert      |       |        |        |         |        |
Delete          |       |        |        |         |        |
Linear Search   |       |        |        |         |        |
Binary Search   |       |        |   N/A  |   N/A   |        | (sort first)
Sort (CGPA)     |       |        |        |         |        |
Display All     |       |        |        |         |        |
Count (LL only) |  N/A  |  N/A   |  N/A   |   N/A   |        |

In your report's cross-member comparison section, all 4 members
paste their completed tables — the group then discusses WHY times
differ between members who implemented the same data structure
(e.g., M1 used Bubble Sort, M2 used Merge Sort — their n=30000
times will be dramatically different, directly proving Big-O).
