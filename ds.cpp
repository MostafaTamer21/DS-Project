#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

struct Course {
    string code;  // should we change it to int ?
    string title;
    int creditHours;
    string instructor;
    vector<string> prerequisites;  // should we change it into course?
};

struct Grade {
    string courseCode;  // should we change it to course?
    string grade;
    string semester;
};

struct Student {
    string id;  // should i do it as int?
    string name;
    string password;
    set<string> registeredCourses;  // should i do it as course?
    unordered_map<string, Grade> grades; // change it to course , grade or leave it ?
};

struct Admin {
    string username;
    string password;
};

class CourseRegistrationSystem {
private:
    unordered_map<string, Student> students;
    unordered_map<string, Admin> admins;
    unordered_map<string, Course> courses;
    string generateUniqueID() {
        string id;
        do {
            id = "2023";
            for (int i = 0; i < 6; ++i)
                id += char('0' + (rand() % 7));
        } while (students.count(id));  
        return id;
    }

public:
    // void signUpStudent(const string& id, const string& name, const string& password) {
    //     if (students.find(id) != students.end()) {
    //         cout << "Student already exists.\n";
    //         return;
    //     }
    //     string id = generateUniqueID();
    //     students[id] = { id, name, password };
    //     cout << "Student registered successfully. Assigned ID: " << id << "\n";
    // }
    void signUpStudent(const string& name, const string& password) {
        string id = generateUniqueID();
        students[id] = { id, name, password };
        cout << "Student registered successfully. Assigned ID: " << id << "\n\n";
    }

// sign up test
    void signUpAdmin(const string& username, const string& password) {
        if (admins.find(username) != admins.end()) {
            cout << "Admin already exists.\n";
            return;
        }
        admins[username] = { username, password };
        cout << "Admin registered successfully.\n";
    }

    bool loginStudent(const string& id, const string& password) {
        return students.find(id) != students.end() && students[id].password == password;
    }

    bool loginAdmin(const string& username, const string& password) {
        return admins.find(username) != admins.end() && admins[username].password == password;
    }

    void addCourse(const string& code, const string& title, int credits, const string& instructor) {
        courses[code] = { code, title, credits, instructor };
        cout << "Course added: " << code << endl;
    }

    void setCoursePrerequisites(const string& code, const vector<string>& prereqs) {
        if (courses.find(code) != courses.end()) {
            courses[code].prerequisites = prereqs;
            cout << "Prerequisites updated for " << code << endl;
        }
    }

    void assignGrade(const string& studentId, const string& courseCode, const string& grade, const string& semester) {
        if (students.find(studentId) != students.end()) {
            students[studentId].grades[courseCode] = { courseCode, grade, semester };
            cout << "Grade assigned.\n";
        }
    }

    void registerCourse(const string& studentId, const string& courseCode) {
        if (students.find(studentId) == students.end() || courses.find(courseCode) == courses.end()) {
            cout << "Student or Course not found.\n";
            return;
        }

        bool prerequisitesMet = true;
        for (const string& pre : courses[courseCode].prerequisites) {
            // Check if the student has registered for the prerequisite
            if (students[studentId].grades.find(pre) == students[studentId].grades.end()) {
                cout << "You have not completed prerequisite: " << pre << endl;
                prerequisitesMet = false;
            }
            else {
                // Check if the student has passed the prerequisite (they should have a grade)
                const Grade& grade = students[studentId].grades.at(pre);
                if (grade.grade == "") {  // This implies the student has registered but not completed
                    cout << "You have registered for " << pre << " but have not completed it yet.\n";
                    prerequisitesMet = false;
                }
            }
        }

        if (prerequisitesMet) {
            students[studentId].registeredCourses.insert(courseCode);
            cout << "Registered in course: " << courseCode << endl;
        }
        else {
            cout << "You cannot register for " << courseCode << " due to missing or incomplete prerequisites.\n";
        }
    }

    double getGradePoint(const string& grade) {
        if (grade == "A"||grade=="a") return 4.0;
        if (grade == "B" || grade == "b") return 3.0;
        if (grade == "C" || grade == "c") return 2.0;
        if (grade == "D" || grade == "d") return 1.0;
        if (grade == "F" || grade == "f") return 0.0;
        return -1.0; // Invalid or missing grade
    }

    void viewGrades(const string& studentId) {
        const auto& s = students[studentId];
        cout << "\nGrades for " << s.name << ":\n";
        for (const auto& pair : s.grades) {
            const Grade& g = pair.second;
            cout << g.courseCode << ": " << g.grade << " (" << g.semester << ")\n";
        }
    }

    void checkPrerequisites(const string& code) {
        if (courses.find(code) != courses.end()) {
            const auto& prereqs = courses[code].prerequisites;
            if (prereqs.empty()) {
                cout << "No prerequisites for " << code << ".\n";
            }
            else {
                cout << "Prerequisites for " << code << ": ";
                for (const auto& pre : prereqs) {
                    cout << pre << " ";
                }
                cout << endl;
            }
        }
        else {
            cout << "Course not found.\n";
        }
    }

    void generateReport(const string& studentId) {
        if (students.find(studentId) == students.end()) {
            cout << "Student not found.\n";
            return;
        }

        const auto& s = students[studentId];
        cout << "\nReport for " << s.name << " (" << studentId << "):\n";

        cout << "Registered Courses:\n";
        for (const string& course : s.registeredCourses) {
            cout << "- " << course << "\n";
        }

        cout << "Grades:\n";
        double totalGradePoints = 0;
        int totalCredits = 0;

        for (const auto& pair : s.grades) {
            const Grade& g = pair.second;
            cout << g.courseCode << ": " << g.grade << " (" << g.semester << ")\n";

            // Lookup course for credit info
            auto courseIt = courses.find(g.courseCode);
            if (courseIt != courses.end()) {
                double gradePoint = getGradePoint(g.grade);
                if (gradePoint >= 0) {
                    totalGradePoints += gradePoint * courseIt->second.creditHours;
                    totalCredits += courseIt->second.creditHours;
                }
            }
        }

        if (totalCredits > 0) {
            double gpa = totalGradePoints / totalCredits;
            cout << fixed << setprecision(2);
            cout << "\nOverall GPA: " << gpa << "\n";
        }
        else {
            cout << "\nNo GPA available (no graded courses with credits).\n";
        }
    }


    unordered_map<string, Course>& getCourses() {
        return courses;
    }
     
    // Save data to a file
    void saveData() {
        // Open files for writing
        ofstream studentsFile("students.txt");
        if (!studentsFile) {
            cout << "Error opening students file for saving data.\n";
            return;
        }

        ofstream adminsFile("admins.txt");
        if (!adminsFile) {
            cout << "Error opening admins file for saving data.\n";
            return;
        }

        ofstream coursesFile("courses.txt");
        if (!coursesFile) {
            cout << "Error opening courses file for saving data.\n";
            return;
        }

        // Save students
        for (const auto& studentPair : students) {
            const Student& s = studentPair.second;
            studentsFile << s.id << "," << s.name << "," << s.password << "\n";
            for (const string& courseCode : s.registeredCourses) {
                studentsFile << "  " << courseCode << "\n";
            }
            for (const auto& gradePair : s.grades) {
                const Grade& g = gradePair.second;
                studentsFile << "  " << g.courseCode << "," << g.grade << "," << g.semester << "\n";
            }
        }

        // Save admins
        for (const auto& adminPair : admins) {
            const Admin& a = adminPair.second;
            adminsFile << a.username << "," << a.password << "\n";
        }

        // Save courses
        for (const auto& coursePair : courses) {
            const Course& c = coursePair.second;
            coursesFile << c.code << "," << c.title << "," << c.creditHours << "," << c.instructor << "\n";
            for (const string& pre : c.prerequisites) {
                coursesFile << "  " << pre << "\n";
            }
        }
        

        studentsFile.close();
        adminsFile.close();
        coursesFile.close();

        cout << "Data saved successfully.\n";


    }

    // Load data from files
    void loadData() {
        // === Load Students ===
        ifstream studentFile("students.txt");
        if (studentFile) {
            string line;
            Student s;

            while (getline(studentFile, line)) {
                if (line.empty()) continue;

                if (line[0] != ' ') {
                    if (!s.id.empty()) students[s.id] = s;
                    s = {};

                    auto p1 = line.find(',');
                    auto p2 = line.find(',', p1 + 1);
                    if (p1 == string::npos || p2 == string::npos) continue;

                    s.id = line.substr(0, p1);
                    s.name = line.substr(p1 + 1, p2 - p1 - 1);
                    s.password = line.substr(p2 + 1);
                }
                else {
                    string data = line.substr(2);
                    size_t c1 = data.find(',');
                    size_t c2 = data.find(',', c1 + 1);

                    if (c1 != string::npos && c2 != string::npos) {
                        string code = data.substr(0, c1);
                        string grade = data.substr(c1 + 1, c2 - c1 - 1);
                        string semester = data.substr(c2 + 1);
                        s.grades[code] = { code, grade, semester };
                    }
                    else {
                        s.registeredCourses.insert(data);
                    }
                }
            }
            if (!s.id.empty()) students[s.id] = s;
            studentFile.close();
        }

        // === Load Admins ===
        ifstream adminFile("admins.txt");
        if (adminFile) {
            string line;
            while (getline(adminFile, line)) {
                size_t p = line.find(',');
                if (p != string::npos) {
                    string user = line.substr(0, p);
                    string pass = line.substr(p + 1);
                    admins[user] = { user, pass };
                }
            }
            adminFile.close();
        }

        // === Load Courses ===
        ifstream courseFile("courses.txt");
        if (courseFile) {
            string line;
            Course c;

            while (getline(courseFile, line)) {
                if (line.empty()) continue;

                if (line[0] != ' ') {
                    if (!c.code.empty()) courses[c.code] = c;
                    c = {};

                    auto p1 = line.find(',');
                    auto p2 = line.find(',', p1 + 1);
                    auto p3 = line.find(',', p2 + 1);

                    if (p1 == string::npos || p2 == string::npos || p3 == string::npos)
                        continue;

                    c.code = line.substr(0, p1);
                    c.title = line.substr(p1 + 1, p2 - p1 - 1);
                    c.creditHours = stoi(line.substr(p2 + 1, p3 - p2 - 1));
                    c.instructor = line.substr(p3 + 1);
                }
                else {
                    c.prerequisites.push_back(line.substr(2));
                }
            }
            if (!c.code.empty()) courses[c.code] = c;
            courseFile.close();
        }

        cout << "Data loaded successfully.\n";
    }

};

void studentMenu(CourseRegistrationSystem& system, const string& studentId) {
    int choice;
    while (true) {
        cout << "\nStudent Menu:\n";
        cout << "1. Register for a course\n";
        cout << "2. View grades\n";
        cout << "3. View report\n";
        cout << "4. Check course prerequisites\n";
        cout << "5. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
        {
            const auto& courses = system.getCourses();
            if (courses.empty()) {
                cout << "No courses available for registration.\n";
                break;
            }

            cout << "\nAvailable Courses:\n";
            for (const auto& pair : courses) {
                const Course& c = pair.second;
                cout << c.code << " - " << c.title
                    << " (" << c.creditHours << " credits)"
                    << " | Instructor: " << c.instructor << "\n";
            }

            string courseCode;
            cout << "Enter course code to register: ";
            cin >> courseCode;
            system.registerCourse(studentId, courseCode);
            break;
        }
        case 2:
            system.viewGrades(studentId);
            break;
        case 3:
            system.generateReport(studentId);
            break;
        case 4:
        {
            string code;
            cout << "Enter course code to check prerequisites: ";
            cin >> code;
            system.checkPrerequisites(code);
            break;
        }
        case 5:
            return; // Logout and return to the main menu
        default:
            cout << "Invalid choice, please try again.\n";
        }
    }
}


void adminMenu(CourseRegistrationSystem& system) {
    int choice;
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add a course\n";
        cout << "2. Set prerequisites for a course\n";
        cout << "3. Assign grade\n";
        cout << "4. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
        {
            string code, title, instructor;
            int credits;
            cout << "Enter course code: ";
            cin >> code;
            cout << "Enter course title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter course credits: ";
            cin >> credits;
            cout << "Enter instructor name: ";
            cin.ignore();
            getline(cin, instructor);
            system.addCourse(code, title, credits, instructor);
            break;
        }
        case 2:
        {
            const auto& courses = system.getCourses();
            if (courses.empty()) {
                cout << "No courses available .\n";
                break;
            }

            cout << "\nAvailable Courses:\n";
            for (const auto& pair : courses) {
                const Course& c = pair.second;
                cout << c.code << " - " << c.title
                    << " (" << c.creditHours << " credits)"
                    << " | Instructor: " << c.instructor << "\n";
            }
            string code;
            cout << "Enter course code: ";
            cin >> code;
            vector<string> prereqs;
            string prereq;
            cout << "Enter prerequisites (enter 'done' to finish):\n";
            while (true) {
                cin >> prereq;
                if (prereq == "done") break;
                prereqs.push_back(prereq);
            }
            system.setCoursePrerequisites(code, prereqs);
            break;
        }
        case 3:
        {
            string studentId, courseCode, grade, semester;
            cout << "Enter student ID: ";
            cin >> studentId;
            cout << "Enter course code: ";
            cin >> courseCode;
            cout << "Enter grade: ";
            cin >> grade;
            cout << "Enter semester: ";
            cin >> semester;
            system.assignGrade(studentId, courseCode, grade, semester);
            break;
        }
        case 4:
            return; // Logout and return to the main menu
        default:
            cout << "Invalid choice, please try again.\n";
        }
    }
}

int main() {
    CourseRegistrationSystem system;

    // Load data at the start of the program
    system.loadData();

    while (true) {
        int authChoice;
        cout << "\n1. Sign Up\n2. Sign In\n0. Exit\nChoose option: ";
        cin >> authChoice;
     
        
        if (authChoice == 0) {
            // Save data before exiting
            system.saveData();
            break;
        }

  

        if (authChoice == 1) {
            int userType;
            cout << "1. Student\n2. Admin\nChoose type: ";
            cin >> userType;
            if (userType == 1) {
            string username, password;
            cout << "Full name: ";  cin.ignore();  getline(cin, username);
            cout << "Password: ";   cin  >> password;
            system.signUpStudent(username, password);
            }
            else {
                string username, password;
                cout<<"Enter Admin User : ";
                cin>>username;
                cout<<"Enter Admin Passowrd: ";
                cin>>password;

                system.signUpAdmin(username, password);
            }
        }

        else if (authChoice == 2) {
            int userType;
            cout << "1. Student\n2. Admin\nChoose type: ";
            cin >> userType;
            string username, password;
            cout<<"Enter id : ";
            cin>>username;
            cout<<"Enter password : ";
            cin>>password;

            bool loggedIn = false;
            if (userType == 1) {
                loggedIn = system.loginStudent(username, password);
                if (loggedIn) {
                    cout << "Student logged in successfully.\n";
                    studentMenu(system, username); // Redirect to student menu
                }
                else {
                    cout << "Invalid credentials.\n";
                }
            }
            else if (userType == 2) {
                loggedIn = system.loginAdmin(username, password);
                if (loggedIn) {
                    cout << "Admin logged in successfully.\n";
                    adminMenu(system); // Redirect to admin menu
                }
                else {
                    cout << "Invalid credentials.\n";
                }
            }
        }
    }

    return 0;
}
