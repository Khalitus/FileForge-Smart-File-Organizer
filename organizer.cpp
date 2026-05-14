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

    void logUndo(string filename, string currentPath, string restoredPath) {
        ofstream file(logFileName, ios::app);

        if (file.is_open()) {
            file << "Undo File: " << filename << "\n";
            file << "From: " << currentPath << "\n";
            file << "To: " << restoredPath << "\n";
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
    string undoFileName;

    int totalScanned;
    int totalMoved;
    int totalFailed;

    int imageCount;
    int documentCount;
    int audioCount;
    int videoCount;
    int archiveCount;
    int codeCount;
    int projectCount;
    int otherCount;

public:
    Organizer(Logger& logRef) : logger(logRef) {
        undoFileName = "undo_data.txt";
        resetStats();
    }

    void resetStats() {
        totalScanned = 0;
        totalMoved = 0;
        totalFailed = 0;

        imageCount = 0;
        documentCount = 0;
        audioCount = 0;
        videoCount = 0;
        archiveCount = 0;
        codeCount = 0;
        projectCount = 0;
        otherCount = 0;
    }

    void updateStats(string folder) {
        if (folder.find("Images") == 0) imageCount++;
        else if (folder.find("Documents") == 0) documentCount++;
        else if (folder.find("Audio") == 0) audioCount++;
        else if (folder.find("Videos") == 0) videoCount++;
        else if (folder.find("Archives") == 0) archiveCount++;
        else if (folder.find("Code") == 0) codeCount++;
        else if (folder.find("Projects") == 0) projectCount++;
        else otherCount++;
    }

    string getDuplicateName(string folderPath, string filename) {
        string newPath = folderPath + "/" + filename;

        if (!fs::exists(newPath)) {
            return newPath;
        }

        int count = 1;
        while (true) {
            string tempName = folderPath + "/copy" + to_string(count) + "_" + filename;
            if (!fs::exists(tempName)) {
                return tempName;
            }
            count++;
        }
    }

    string getUndoRestoreName(string oldPath) {
        if (!fs::exists(oldPath)) {
            return oldPath;
        }

        fs::path originalPath(oldPath);
        string folderPath = originalPath.parent_path().string();
        string filename = originalPath.filename().string();

        int count = 1;
        while (true) {
            string tempPath = folderPath + "/restored" + to_string(count) + "_" + filename;
            if (!fs::exists(tempPath)) {
                return tempPath;
            }
            count++;
        }
    }

    void clearUndoFile() {
        ofstream file(undoFileName, ios::out);
        file.close();
    }

    void saveUndoInfo(string oldPath, string newPath) {
        ofstream file(undoFileName, ios::app);
        if (file.is_open()) {
            file << oldPath << "|" << newPath << "\n";
            file.close();
        }
    }

    void viewFiles(string path) {
        cout << "\nFiles and folders in directory:\n";

        try {
            for (auto& entry : fs::directory_iterator(path)) {
                if (fs::is_regular_file(entry)) {
                    cout << "[FILE]   " << entry.path().filename().string() << endl;
                } else if (fs::is_directory(entry)) {
                    cout << "[FOLDER] " << entry.path().filename().string() << endl;
                }
            }
        }
        catch (...) {
            cout << "Could not open directory.\n";
        }
    }

    void preview(string path) {
        bool found = false;
        cout << "\n===== Preview Before Organizing =====\n";

        try {
            for (auto& entry : fs::directory_iterator(path)) {
                if (fs::is_regular_file(entry)) {
                    found = true;

                    string ext = entry.path().extension().string();
                    string filename = entry.path().filename().string();
                    string folder = rule.getFolder(ext, filename);

                    string newFolderPath = path + "/" + folder;
                    string newPath = getDuplicateName(newFolderPath, filename);
                    string newName = fs::path(newPath).filename().string();

                    cout << filename << " -> " << folder;
                    if (newName != filename) {
                        cout << " (will be saved as " << newName << ")";
                    }
                    cout << endl;
                }
            }

            if (!found) {
                cout << "No regular files found.\n";
            }
        }
        catch (...) {
            cout << "Could not preview files.\n";
        }
    }

    void organize(string path) {
        resetStats();
        clearUndoFile();

        try {
            for (auto& entry : fs::directory_iterator(path)) {
                if (fs::is_regular_file(entry)) {
                    totalScanned++;

                    string ext = entry.path().extension().string();
                    string filename = entry.path().filename().string();
                    string folder = rule.getFolder(ext, filename);

                    string newFolderPath = path + "/" + folder;
                    fs::create_directories(newFolderPath);

                    string newPath = getDuplicateName(newFolderPath, filename);

                    try {
                        string oldPath = entry.path().string();
                        fs::rename(entry.path(), newPath);

                        cout << "Moved: " << filename << " -> " << folder;

                        string newName = fs::path(newPath).filename().string();
                        if (newName != filename) {
                            cout << " (saved as " << newName << ")";
                        }

                        cout << endl;

                        logger.logMove(filename, oldPath, newPath, folder);
                        saveUndoInfo(oldPath, newPath);

                        totalMoved++;
                        updateStats(folder);
                    }
                    catch (...) {
                        cout << "Error moving file: " << filename << endl;
                        logger.logError(filename);
                        totalFailed++;
                    }
                }
            }

            cout << "\nFiles organized successfully!\n";
            showReport();
        }
        catch (...) {
            cout << "Could not access directory.\n";
        }
    }

    void undoLastOrganization() {
        ifstream file(undoFileName);
        string line;

        string oldPaths[500];
        string newPaths[500];
        int count = 0;

        if (!file.is_open()) {
            cout << "No undo information found.\n";
            return;
        }

        while (getline(file, line)) {
            if (line.length() > 0 && count < 500) {
                int pos = line.find('|');
                if (pos != string::npos) {
                    oldPaths[count] = line.substr(0, pos);
                    newPaths[count] = line.substr(pos + 1);
                    count++;
                }
            }
        }
        file.close();

        if (count == 0) {
            cout << "No files available to undo.\n";
            return;
        }

        cout << "\n===== Undo Last Organization =====\n";

        for (int i = count - 1; i >= 0; i--) {
            try {
                if (fs::exists(newPaths[i])) {
                    fs::path oldPath(oldPaths[i]);
                    fs::create_directories(oldPath.parent_path());

                    string restoredPath = getUndoRestoreName(oldPaths[i]);
                    fs::rename(newPaths[i], restoredPath);

                    cout << "Restored: " << fs::path(newPaths[i]).filename().string();

                    if (restoredPath != oldPaths[i]) {
                        cout << " (saved as " << fs::path(restoredPath).filename().string() << ")";
                    }

                    cout << endl;
                    logger.logUndo(fs::path(newPaths[i]).filename().string(), newPaths[i], restoredPath);
                }
            }
            catch (...) {
                cout << "Could not restore file: " << fs::path(newPaths[i]).filename().string() << endl;
            }
        }

        clearUndoFile();
        cout << "Undo completed successfully!\n";
    }

    void showReport() {
        cout << "\n===== Statistics Report =====\n";
        cout << "Total files scanned: " << totalScanned << endl;
        cout << "Total files moved: " << totalMoved << endl;
        cout << "Total failed moves: " << totalFailed << endl;

        cout << "\nCategory-wise summary:\n";
        cout << "Images: " << imageCount << endl;
        cout << "Documents: " << documentCount << endl;
        cout << "Audio: " << audioCount << endl;
        cout << "Videos: " << videoCount << endl;
        cout << "Archives: " << archiveCount << endl;
        cout << "Code: " << codeCount << endl;
        cout << "Projects: " << projectCount << endl;
        cout << "Others: " << otherCount << endl;
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
        cout << "3. Undo Last Organization\n";
        cout << "4. View Log\n";
        cout << "5. Exit\n";
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
                    organizer.preview(path);
                    if (askYesNo("\nDo you want to organize these files now? (Y/N): ")) {
                        organizer.organize(path);
                    } else {
                        cout << "Organization cancelled.\n";
                    }
                    break;

                case 3:
                    if (askYesNo("Do you want to undo the last organization? (Y/N): ")) {
                        organizer.undoLastOrganization();
                    } else {
                        cout << "Undo cancelled.\n";
                    }
                    break;

                case 4:
                    logger.viewLog();
                    break;

                case 5:
                    cout << "Exiting program...\n";
                    break;

                default:
                    cout << "Invalid choice.\n";
            }

        } while (choice != 5);
    }
};

int main() {
    Menu menu;
    menu.run();
    return 0;
}
