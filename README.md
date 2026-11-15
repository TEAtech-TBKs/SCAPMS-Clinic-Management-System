# Smart Clinic Appointment & Patient Management System (SCAPMS)

A C++17 desktop application using Qt Widgets. The project is arranged into model,
controller, GUI and utility layers.

## Requirements
- C++17 compiler (MSVC, GCC or Clang)
- CMake 3.16+
- Qt 5.15+ or Qt 6 (Widgets)
- VS Code with the C/C++ and CMake Tools extensions

## Build in VS Code
1. Install Qt (Qt Online Installer) and a C++ compiler.
2. Install the VS Code extensions **C/C++** and **CMake Tools**.
3. Open this folder in VS Code.
4. Configure CMake with a kit that uses your C++17 compiler.
5. If CMake cannot find Qt, set `CMAKE_PREFIX_PATH` to your Qt installation's
   `lib/cmake` directory. Example (adjust the path to your installation):
   `cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"`
6. Build and run:
   `cmake -S . -B build -DCMAKE_PREFIX_PATH="<YOUR_QT_PREFIX>"`
   `cmake --build build`
   Then run `build/SCAPMS.exe` on Windows or `build/SCAPMS` on Linux/macOS.

For a Qt installation using MSVC, select an MSVC CMake kit. For MinGW, select
the matching MinGW kit supplied with Qt.

## Included functionality
- Patient registration, editing and deletion
- General, Emergency and Follow-up patient types
- Appointment scheduling and cancellation
- FIFO waiting queue
- Undo of the latest patient administration change
- Fixed-size recent-patient cache
- Manual linear, binary and recursive searches
- Manual selection and insertion sorting by name, ID or age
- CSV-style persistence, validation and exception handling
- Dashboard, queue and three reports
- Menus, keyboard shortcuts, layouts, tooltips, dialogs and timer refresh

## Important
The required search and sorting algorithms are implemented in `SearchEngine.cpp`.
They deliberately do not call `std::sort`, `std::find` or other library search/sort
algorithms. Standard containers are used only for storage/management.

This is an educational clinic-management demonstration. It does not store real
medical records and should only be tested with fictional sample data.
