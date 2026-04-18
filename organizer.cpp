#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

// Rule class
class Rule {
public:
    string getFolder(string ext, string filename) {
        if (ext == ".jpg") return "Images";
        if (ext == ".pdf") return "Documents";
        if (ext == ".mp3") return "Audio";
        if (ext == ".cpp") return "Code";
        return "Others";
    }
};

// Logger class
class Logger {
public:
    void log(string message) {
        ofstream file("log.txt", ios::app);
        if (file.is_open()) {
            file << message << endl;
            file.close();
        }
    }

    void viewLog() {
        ifstream file("log.txt");
        string line;

        if (file.is_open()) {
            cout << "\n--- Log File ---\n";
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cout << "Log file not found.\n";
        }
    }
};

// Organizer class
class Organizer {
private:
    Rule rule;
    Logger& logger;

public:
    Organizer(Logger& logRef) : logger(logRef) {}

    void viewFiles(string path) {
        cout << "\nFiles in directory:\n";
        for (auto& entry : fs::directory_iterator(path)) {
            cout << entry.path().filename().string() << endl;
        }
    }

    void organize(string path) {
        for (auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry)) {
                string ext = entry.path().extension().string();
                string filename = entry.path().filename().string();
                string folder = rule.getFolder(ext, filename);

                string newFolderPath = path + "/" + folder;
                fs::create_directories(newFolderPath);

                string newPath = newFolderPath + "/" + filename;

                try {
                    fs::rename(entry.path(), newPath);
                    string msg = "Moved: " + filename + " -> " + folder;
                    cout << msg << endl;
                    logger.log(msg);
                }
                catch (...) {
                    cout << "Error moving file: " << filename << endl;
                }
            }
        }
    }
};

// Menu class
class Menu {
private:
    Logger logger;
    Organizer organizer;
    string path;

public:
    Menu() : organizer(logger) {}

    void setFolderPath() {
        cout << "Enter folder path: ";
        getline(cin, path);
    }

    void displayMenu() {
        cout << "\n===== File Organizer Menu =====\n";
        cout << "1. View Files\n";
        cout << "2. Organize Files\n";
        cout << "3. View Log\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
    }

    void run() {
        setFolderPath();

        int choice;
        do {
            displayMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    organizer.viewFiles(path);
                    break;
                case 2:
                    organizer.organize(path);
                    break;
                case 3:
                    logger.viewLog();
                    break;
                case 4:
                    cout << "Exiting program...\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
            }
        } while (choice != 4);
    }
};

int main() {
    Menu menu;
    menu.run();
    return 0;
}
