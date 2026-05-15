#ifndef FILEORGANIZER_H
#define FILEORGANIZER_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>

struct PreviewItem {
    QString fileName;
    QString folder;
    QString oldPath;
    QString newPath;
    QString finalName;
    bool renamed = false;
};

struct StatsReport {
    int totalScanned = 0;
    int totalMoved = 0;
    int totalFailed = 0;
    int imageCount = 0;
    int documentCount = 0;
    int audioCount = 0;
    int videoCount = 0;
    int archiveCount = 0;
    int codeCount = 0;
    int projectCount = 0;
    int otherCount = 0;
};

class Rule {
public:
    QString getFolder(const QString &ext, const QString &filename) const {
        const QString lowerExt = ext.toLower();
        const QString lowerName = filename.toLower();
        if (lowerExt == ".jpg" || lowerExt == ".jpeg") return "Images/JPG";
        if (lowerExt == ".png") return "Images/PNG";
        if (lowerExt == ".gif") return "Images/GIF";
        if (lowerExt == ".bmp") return "Images/BMP";
        if (lowerExt == ".mp3") return "Audio/MP3";
        if (lowerExt == ".wav") return "Audio/WAV";
        if (lowerExt == ".mp4") return "Videos/MP4";
        if (lowerExt == ".mkv") return "Videos/MKV";
        if (lowerExt == ".avi") return "Videos/AVI";
        if (lowerExt == ".pdf") return "Documents/PDFs";
        if (lowerExt == ".docx" || lowerExt == ".doc") return "Documents/Word";
        if (lowerExt == ".txt") return "Documents/Text";
        if (lowerExt == ".xlsx" || lowerExt == ".xls") return "Documents/Excel";
        if (lowerExt == ".pptx" || lowerExt == ".ppt") return "Documents/PowerPoint";
        if (lowerExt == ".zip") return "Archives/ZIP";
        if (lowerExt == ".rar") return "Archives/RAR";
        if (lowerExt == ".cpp") return "Code/C++";
        if (lowerExt == ".py") return "Code/Python";
        if (lowerExt == ".java") return "Code/Java";
        if (lowerExt == ".html") return "Code/HTML";
        if (lowerExt == ".css") return "Code/CSS";
        if (lowerExt == ".js") return "Code/JavaScript";
        if (lowerName.contains("project")) return "Projects";
        return "Others";
    }
};

class Logger {
private:
    QString logsDirectory;
    QString logFileName;
public:
    bool initializeSessionStorage(QString *errorMessage = nullptr) {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        if (desktopPath.isEmpty()) desktopPath = QDir::homePath();
        QDir desktopDir(desktopPath);
        logsDirectory = desktopDir.filePath("logs");
        if (!desktopDir.exists() || (!QDir(logsDirectory).exists() && !desktopDir.mkpath("logs"))) {
            if (errorMessage) *errorMessage = "Desktop logs folder could not be created.";
            return false;
        }
        logFileName = QDir(logsDirectory).filePath("log1.txt");
        QFile file(logFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            if (errorMessage) *errorMessage = "Session log file could not be created.";
            return false;
        }
        QTextStream out(&file);
        out << "===== FileForge Session =====\n\n";
        file.close();
        return true;
    }

    void logMove(const QString &filename, const QString &oldPath, const QString &newPath, const QString &folder) {
        if (logFileName.isEmpty()) return;
        QFile file(logFileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream out(&file);
            out << "Moved File: " << filename << "\n";
            out << "From: " << oldPath << "\n";
            out << "To: " << newPath << "\n";
            out << "Category: " << folder << "\n";
            out << "-----------------------------\n";
        }
    }

    void logError(const QString &filename, const QString &reason) {
        if (logFileName.isEmpty()) return;
        QFile file(logFileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream out(&file);
            out << "Error File: " << filename << "\nReason: " << reason << "\n-----------------------------\n";
        }
    }

    QString readLogText() const {
        QFile file(logFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "Could not open the session log file.";
        QTextStream in(&file);
        return in.readAll();
    }

    QString getLogFileName() const { return logFileName; }
    QString getLogsDirectory() const { return logsDirectory; }
    bool isReady() const { return !logFileName.isEmpty(); }
};

class Organizer {
private:
    Rule rule;
    Logger *logger = nullptr;
    StatsReport lastStats;

    void resetStats() { lastStats = StatsReport(); }
    void updateStats(const QString &folder) {
        if (folder.startsWith("Images")) lastStats.imageCount++;
        else if (folder.startsWith("Documents")) lastStats.documentCount++;
        else if (folder.startsWith("Audio")) lastStats.audioCount++;
        else if (folder.startsWith("Videos")) lastStats.videoCount++;
        else if (folder.startsWith("Archives")) lastStats.archiveCount++;
        else if (folder.startsWith("Code")) lastStats.codeCount++;
        else if (folder.startsWith("Projects")) lastStats.projectCount++;
        else lastStats.otherCount++;
    }

    QString getDuplicateName(const QString &folderPath, const QString &filename) const {
        const QString firstPath = QDir(folderPath).filePath(filename);
        if (!QFileInfo::exists(firstPath)) return firstPath;
        int count = 1;
        while (true) {
            const QString tempPath = QDir(folderPath).filePath(QString("copy%1_%2").arg(count).arg(filename));
            if (!QFileInfo::exists(tempPath)) return tempPath;
            count++;
        }
    }

    QList<QFileInfo> collectSourceFiles(const QString &path, QString *errorMessage = nullptr) const {
        QList<QFileInfo> files;
        const QDir sourceDir(path);
        if (!sourceDir.exists()) {
            if (errorMessage) *errorMessage = "The selected folder is not available.";
            return files;
        }
        const QFileInfoList infoList = sourceDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        for (const QFileInfo &info : infoList) files.append(info);
        return files;
    }

public:
    explicit Organizer(Logger *logRef = nullptr) : logger(logRef) {}
    void setLogger(Logger *logRef) { logger = logRef; }

    QList<PreviewItem> preview(const QString &path, QString *errorMessage = nullptr) const {
        QList<PreviewItem> items;
        const QList<QFileInfo> files = collectSourceFiles(path, errorMessage);
        for (const QFileInfo &info : files) {
            const QString filename = info.fileName();
            const QString ext = info.suffix().isEmpty() ? QString() : QString(".") + info.suffix();
            const QString folder = rule.getFolder(ext, filename);
            const QString newPath = getDuplicateName(QDir(path).filePath(folder), filename);
            PreviewItem item{filename, folder, info.absoluteFilePath(), newPath, QFileInfo(newPath).fileName(), QFileInfo(newPath).fileName() != filename};
            items.append(item);
        }
        return items;
    }

    QStringList organize(const QString &path) {
        QStringList messages;
        resetStats();
        QString scanError;
        const QList<QFileInfo> files = collectSourceFiles(path, &scanError);
        if (!scanError.isEmpty()) { messages << scanError; return messages; }

        for (const QFileInfo &info : files) {
            lastStats.totalScanned++;
            const QString filename = info.fileName();
            const QString ext = info.suffix().isEmpty() ? QString() : QString(".") + info.suffix();
            const QString folder = rule.getFolder(ext, filename);
            const QString newFolderPath = QDir(path).filePath(folder);
            QDir dir;
            if (!dir.mkpath(newFolderPath)) {
                messages << QString("Error moving file: %1").arg(filename);
                if (logger) logger->logError(filename, "Destination folder could not be created.");
                lastStats.totalFailed++;
                continue;
            }
            const QString oldPath = info.absoluteFilePath();
            const QString newPath = getDuplicateName(newFolderPath, filename);
            QFile file(oldPath);
            if (!file.rename(newPath)) {
                messages << QString("Error moving file: %1").arg(filename);
                if (logger) logger->logError(filename, file.errorString());
                lastStats.totalFailed++;
                continue;
            }
            if (logger) logger->logMove(filename, oldPath, newPath, folder);
            QString message = QString("Moved: %1 -> %2").arg(filename, folder);
            const QString finalName = QFileInfo(newPath).fileName();
            if (finalName != filename) message += QString(" (saved as %1)").arg(finalName);
            messages << message;
            lastStats.totalMoved++;
            updateStats(folder);
        }
        return messages;
    }

    StatsReport getStats() const { return lastStats; }
};

#endif // FILEORGANIZER_H
