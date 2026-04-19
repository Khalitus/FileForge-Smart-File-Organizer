#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

// Rule class
class Rule {
private:
    string toLowerCase(string text) {
        for (int i = 0; i < text.length(); i++) {
            text[i] = tolower(text[i]);
        }
        return text;
    }

public:
    string getFolder(string ext, string filename) {
        ext = toLowerCase(ext);
        filename = toLowerCase(filename);

        // Images
        if (ext == ".jpg" || ext == ".jpeg") return "Images/JPG";
        if (ext == ".png") return "Images/PNG";
        if (ext == ".gif") return "Images/GIF";
        if (ext == ".bmp") return "Images/BMP";

        // Audio
        if (ext == ".mp3") return "Audio/MP3";
        if (ext == ".wav") return "Audio/WAV";

        // Videos
        if (ext == ".mp4") return "Videos/MP4";
        if (ext == ".mkv") return "Videos/MKV";
        if (ext == ".avi") return "Videos/AVI";

        // Documents
        if (ext == ".pdf") return "Documents/PDFs";
        if (ext == ".docx" || ext == ".doc") return "Documents/Word";
        if (ext == ".txt") return "Documents/Text";
        if (ext == ".xlsx" || ext == ".xls") return "Documents/Excel";
        if (ext == ".pptx" || ext == ".ppt") return "Documents/PowerPoint";

        // Archives
        if (ext == ".zip") return "Archives/ZIP";
        if (ext == ".rar") return "Archives/RAR";

        // Code
        if (ext == ".cpp") return "Code/C++";
        if (ext == ".py") return "Code/Python";
        if (ext == ".java") return "Code/Java";
        if (ext == ".html") return "Code/HTML";
        if (ext == ".css") return "Code/CSS";
        if (ext == ".js") return "Code/JavaScript";

        // Projects
        if (filename.find("project") != string::npos) return "Projects";

        return "Others";
    }
};

// Logger class
class Logger {
private:
    string logFileName;

public:
    string addTxtExtension(string name) {
        if (name.find(".txt") == string::npos) {
            name += ".txt";
        }
        return name;
    }

    bool fileExists(string name) {
        return fs::exists(name);
    }

    bool createLogFile(string name) {
        logFileName = name;
        ofstream file(logFileName, ios::out);

        if (!file.is_open()) {
            return false;
        }

        file << "===== File Organizer Log =====\n\n";
        file.close();
        return true;
    }

    void logMove(string filename, string oldPath, string newPath, string folder) {
        ofstream file(logFileName, ios::app);

        if (file.is_open()) {
            file << "Moved File: " << filename << "\n";
            file << "From: " << oldPath << "\n";
            file << "To: " << newPath << "\n";
            file << "Category: " << folder << "\n";
            file << "-----------------------------\n";
            file.close();
        }
    }

    void logError(string filename) {
        ofstream file(logFileName, ios::app);

        if (file.is_open()) {
            file << "Error moving file: " << filename << "\n";
            file << "-----------------------------\n";
            file.close();
        }
    }

    void viewLog() {
        ifstream file(logFileName);
        string line;

        if (file.is_open()) {
            cout << "\n--- Log File: " << logFileName << " ---\n";
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cout << "Log file not found.\n";
        }
    }

    string getLogFileName() {
        return logFileName;
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
                    string oldPath = entry.path().string();
                    fs::rename(entry.path(), newPath);
                    cout << "Moved: " << filename << " -> " << folder << endl;
                    logger.logMove(filename, oldPath, newPath, folder);
                }
                catch (...) {
                    cout << "Error moving file: " << filename << endl;
                    logger.logError(filename);
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

    bool askYesNo(string message) {
        char choice;
        cout << message;
        cin >> choice;
        cin.ignore();
        return (choice == 'Y' || choice == 'y');
    }

    void setFolderPath() {
        while (true) {
            cout << "Enter folder path: ";
            getline(cin, path);

            if (fs::exists(path) && fs::is_directory(path)) {
                break;
            } else {
                cout << "Invalid folder path. Try again.\n";
            }
        }
    }

    void setLogFile() {
        while (true) {
            string name;
            cout << "Enter log file name: ";
            getline(cin, name);

            name = logger.addTxtExtension(name);

            if (logger.fileExists(name)) {
                cout << "Log file already exists.\n";
                if (askYesNo("Do you want to overwrite it? (Y/N): ")) {
                    if (logger.createLogFile(name)) {
                        cout << "Log file created: " << name << endl;
                        break;
                    } else {
                        cout << "Could not create log file.\n";
                    }
                } else {
                    cout << "Please enter another file name.\n";
                }
            } else {
                if (logger.createLogFile(name)) {
                    cout << "Log file created: " << name << endl;
                    break;
                } else {
                    cout << "Could not create log file.\n";
                }
            }
        }
    }

    void displayMenu() {
        cout << "\n===== File Organizer Menu =====\n";
        cout << "Current Folder: " << path << endl;
        cout << "Current Log File: " << logger.getLogFileName() << endl;
        cout << "1. View Files\n";
        cout << "2. Organize Files\n";
        cout << "3. View Log\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
    }

    void run() {
        setFolderPath();
        setLogFile();

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
