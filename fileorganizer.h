#ifndef FILEORGANIZER_H
#define FILEORGANIZER_H

#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QString>

struct PreviewItem {
    QString fileName;
    QString folder;
    QString oldPath;
    QString newPath;
    QString finalName;
    bool renamed = false;
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

class Organizer {
private:
    Rule rule;

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

public:
    QList<PreviewItem> preview(const QString &path, QString *errorMessage = nullptr) const {
        QList<PreviewItem> items;
        const QDir sourceDir(path);
        if (!sourceDir.exists()) {
            if (errorMessage) *errorMessage = "The selected folder is not available.";
            return items;
        }

        const QFileInfoList files = sourceDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        for (const QFileInfo &info : files) {
            const QString filename = info.fileName();
            const QString ext = info.suffix().isEmpty() ? QString() : QString(".") + info.suffix();
            const QString folder = rule.getFolder(ext, filename);
            const QString newFolderPath = QDir(path).filePath(folder);
            const QString newPath = getDuplicateName(newFolderPath, filename);

            PreviewItem item;
            item.fileName = filename;
            item.folder = folder;
            item.oldPath = info.absoluteFilePath();
            item.newPath = newPath;
            item.finalName = QFileInfo(newPath).fileName();
            item.renamed = (item.finalName != filename);
            items.append(item);
        }
        return items;
    }
};

#endif // FILEORGANIZER_H
