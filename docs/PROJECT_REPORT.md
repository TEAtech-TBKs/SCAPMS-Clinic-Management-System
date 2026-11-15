# SMART CLINIC APPOINTMENT & PATIENT MANAGEMENT SYSTEM (SCAPMS)
## COS202 Programming II — Lab Assessment Report

### 1. Project description
The Smart Clinic Appointment & Patient Management System is a C++17 desktop application designed for a university clinic. It provides a graphical interface for registering and managing patient records, scheduling and cancelling appointments, maintaining a first-in-first-out waiting queue, searching and sorting records, saving/loading data and producing simple clinic reports. The GUI is developed with Qt Widgets, while the application logic is separated into model, controller and utility components.

### 2. System features
The application supports patient registration, editing and deletion. A patient can be classified as General, Emergency or Follow-Up. Appointments can be scheduled for registered patients and pending appointments can be cancelled. The waiting queue follows FIFO order: the first patient added is the first patient called. The dashboard displays the number of registered patients, pending appointments and emergency patients. Reports show category distribution, pending appointments and an appointment-status summary. The user can save and load the clinic data through file dialogs.

### 3. Class hierarchy and OOP concepts
`PatientRecord` is an abstract base class containing patient ID, name, age, contact information and appointment identifiers. It declares pure virtual functions for category, care note, type code and cloning. `GeneralPatient`, `EmergencyPatient` and `FollowUpPatient` inherit from it and provide their own implementations. This demonstrates inheritance and runtime polymorphism: the application can process different patient types through `PatientRecord` pointers while virtual functions return the correct category and behaviour.

`Appointable` is an interface-like abstract class. It declares `scheduleAppointment()` and `cancelAppointment()`, which are implemented by `PatientRecord`. `Appointment` stores appointment information and refers to a patient through the patient ID. `ClinicManager` manages the patient collection and appointments. This relationship demonstrates association; the manager contains the records it manages.

### 4. Data structures
- **Vector:** stores registered patients and appointments. It supports dynamic growth as records are added or removed.
- **Queue:** stores patient IDs waiting for consultation. FIFO ordering reflects the normal waiting-room process.
- **Stack:** stores snapshots of the patient and appointment data before administrative changes. The most recent snapshot is restored first, implementing undo in last-in-first-out order.
- **Fixed-size array:** a ten-element array caches pointers to the first ten patient records for a small frequently accessed record cache. The cache is refreshed when patient storage changes.

### 5. Searching algorithms
Three student-implemented searching methods are included. Linear search checks patient IDs one by one and works on unsorted records. Binary search compares the target ID with the middle record and repeatedly halves the search interval. It requires sorted IDs, so the program creates a temporary copy and manually sorts it by ID before searching. Recursive search checks patient names one index at a time and calls itself with the next index until a match is found or the end is reached.

### 6. Sorting algorithms and reasons
Selection sort and insertion sort are implemented manually. Selection sort repeatedly finds the smallest remaining value and places it in the next position. It is simple to understand and suitable for demonstrating sorting on small clinic datasets. Insertion sort inserts each item into its correct position within the already-processed part of the list. It is useful for small or nearly sorted datasets and is also used to prepare the temporary ID-ordered collection for binary search. The interface lets the user choose the algorithm and sort field: patient name, ID or age. No standard-library sorting function is used for these required algorithms.

### 7. Recursive component
The recursive name search has a base case when the index reaches the number of patient records. Otherwise, it compares the current patient's name with the search term and either returns the index or calls itself for the next index. This demonstrates a recursive algorithm and its termination condition.

### 8. GUI design and event-driven programming
The interface uses Qt Widgets and contains tabs for Patient Records, Appointments, Search & Sort, Waiting Queue and Reports. Tables display patient and appointment information. Buttons trigger registration, editing, deletion, scheduling, cancellation, queue operations, searching, sorting and report generation. The File menu supports New, Save, Load and Exit. Keyboard shortcuts include Ctrl+S, Ctrl+F and Ctrl+Q. A timer refreshes dashboard and table information periodically.

`QVBoxLayout` arranges major sections vertically, while `QHBoxLayout` places related buttons and controls horizontally. `QFormLayout` is used for patient and appointment forms because labels and input fields are aligned in rows. These layouts adjust to window resizing instead of relying on fixed coordinates. Emergency patient rows are highlighted to distinguish their priority. Tooltips explain important controls, and input validation provides meaningful dialog messages.

### 9. Exception handling
Operations that can fail validate their input and throw standard exceptions such as `std::invalid_argument` or `std::runtime_error`. GUI event handlers catch these exceptions and display a warning dialog instead of allowing the application to terminate. Examples include duplicate patient IDs, missing fields, invalid ages, missing patients, invalid appointment data and file access errors.

### 10. Challenges and resolutions
One challenge is that binary search only works correctly when the data is sorted by the searched field. The program resolves this by making a temporary patient copy and applying insertion sort by ID before binary search. Another challenge is preserving patient type information when loading saved records. A one-letter type code is stored with each patient and used to reconstruct the correct derived class. Undo also requires more than remembering a single deleted value, so the program stores a snapshot of the patient and appointment collections before administrative changes. Finally, separating the GUI from the data model keeps button event handlers from becoming responsible for the underlying storage rules.

### 11. Conclusion
SCAPMS demonstrates advanced object-oriented programming, data structures, manually implemented searching and sorting algorithms, recursion, event-driven GUI programming, persistence and exception handling in a C++17 application. The modular structure makes the system easier to understand, test and extend.
