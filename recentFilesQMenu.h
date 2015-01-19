//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef RECENT_FILES_Q_MENU_HEADER
#define RECENT_FILES_Q_MENU_HEADER

#include <QMenu>
#include <QString>
#include <QStringList>

class QActionGroup;

class RecentFilesQMenu : public QMenu
{
    Q_OBJECT

public:
    explicit RecentFilesQMenu(QWidget *parent = 0, bool doShowDirPathInRecent = false, int maxNumFiles = 15);
    ~RecentFilesQMenu();

    // Accessors.
    const QStringList & files() const { return m_files; }
    int maxNumFiles() const { return m_maxNumFiles; }

    void addFile(const QString & fileName);
    void insertMenuActions(int numActions);
    void removeMenuActions(int numActions);
    void updateMenuActions();
    void revalidateFileList();
    void resizeFileList(int maxNumFiles);
    void setFileList(const QStringList & files);
    void setDoShowDirPathInRecent(bool doShowDirPathInRecent);
    QString actionTextFromFileName(const QString & fileName) const;

signals:
    void sourceSelected(const QString & sourceName);

public slots:
    void openFile();
    void clearFiles();

private:
    bool m_doShowDirPathInRecent;  // indicates if the recent files are to show in the menu with directory path
    int m_maxNumFiles;             // max number of recent files to be shownere
    QStringList m_files;           // list of recent files, the most recent first
};

#endif // RECENT_FILES_Q_MENU_HEADER
