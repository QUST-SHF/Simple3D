//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "recentFilesQMenu.h"
#include <QAction>
#include <QFileInfo>
#include <QAction>
#include <algorithm>  // max, min


RecentFilesQMenu::RecentFilesQMenu(QWidget *parent, bool doShowDirPathInRecent, int maxNumFiles) :
    QMenu(parent), m_doShowDirPathInRecent(doShowDirPathInRecent), m_maxNumFiles(maxNumFiles), m_files()
{
    // Create blank, invisible actions to accomodate up to maximal allowed
    // number of recent files.
    insertMenuActions(m_maxNumFiles);
    // Populate actions with the recent files from current list.
    updateMenuActions();
    // Add a separator between the recent files and the "Clear Menu" action.
    addSeparator();

    // Add the "Clear Menu" action.
    QAction * act = new QAction(tr("Clear &Menu"), this);
    act->setObjectName(QStringLiteral("__qt_action_clear_menu_"));
    connect(act, SIGNAL(triggered()), this, SLOT(clearFiles()));
    addAction(act);
    // Make the full-directory-path name of the recent files not appear as tooltip.
    setToolTipsVisible(false);
}

// Destructor.
RecentFilesQMenu::~RecentFilesQMenu()
{
    clear(); // this will destroy the actions assigned to the menu
}


//=============================================================================
// The function "addRecentFile" adds a new file to the list of recent files.
//=============================================================================
void RecentFilesQMenu::addFile(const QString & fileName)
{
    // Remove from the list of recent files any files that no longer exist.
    revalidateFileList();
    // Remove any instances of the file to be addeed. There could be at most one.
    m_files.removeOne(fileName);
    // Prepend the file to be added.
    m_files.prepend(fileName);
    // Check if the maximal allowed length of the list is exceeded.
    while (m_files.size() > m_maxNumFiles)
        m_files.removeLast();

    // Populate the recent files menu actions according to the new file list.
    updateMenuActions();
}


//=============================================================================
// The function "openFile" emits a signal to open a file when its action is
// launched.
//=============================================================================
void RecentFilesQMenu::openFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
    {
        // Get the source file name from the action.
        QString sourceName = action->iconText();
        // Move the source to the top of the list.
        addFile(sourceName);
        // Send a signal specifying the desired source name.
        emit sourceSelected(sourceName);
    }
}


//=============================================================================
// The function "clearFiles" clears the recent files list.
//=============================================================================
void RecentFilesQMenu::clearFiles()
{
    m_files.clear();
    updateMenuActions();
}


//=============================================================================
// The function "insertMenuActions" inserts a specified number of blank,
// invisible actions to the menu at the beginning of its list of actions.
// INPUT: "int numActions" is the desired number of actions to be inserted.
//=============================================================================
void RecentFilesQMenu::insertMenuActions(int numActions)
{
    // Get the list of actions from the menu.
    const QList<QAction *> menuActions = actions();

    // Specify the action before the new actions are to be inserted. If no
    // actions are already in the menu, then we initialize with a nullptr which
    // leads to the new actions being added (rather inserted) to the list.
    QAction * nextAct = nullptr;
    if (menuActions.empty() == false)
        nextAct = menuActions[0];

    for (int i = 0; i < numActions; ++i)
    {
        // Add the necessary number of blank actions.
        QAction * act = new QAction(this);  // need to insert "this" into the QAction.
        act->setVisible(false);
        connect(act, SIGNAL(triggered()), this, SLOT(openFile()));
        // Insert the current action to the menu.
        insertAction(nextAct, act);
    }
}


//=============================================================================
// The function "removeMenuActions" removes a specified number of actions from
// the menu. The actions removed are from the beginning of the list of actions.
// INPUT: "int numActions" is the desired number of actions to be removed.
//=============================================================================
void RecentFilesQMenu::removeMenuActions(int numActions)
{
    // Get the list of actions from the menu.
    const QList<QAction *> menuActions = actions();

    // Find the number of actions actually to remove. Note that we always keep
    // the "Clear list" action and at least one recent file action.
    int numToRemove = std::min(numActions, menuActions.size() - 2);
    for (int i = 0; i < numToRemove; ++i)
    {
        // Remove the first action from the current list in the menu.
        removeAction(menuActions[i]);
    }
}


//=============================================================================
// The function "updateMenuActions" populates actions with the recent files
// from current list. Unused actions are made invisible. Note that the menu
// keeps the maximal needed number of actions at all times but makes visible
// only the ones that are set with a valid recent file.
//=============================================================================
void RecentFilesQMenu::updateMenuActions()
{
    // Get the list of actions from the menu.
    const QList<QAction *> menuActions = actions();

    // Set one action per recent file.
    int numRecentFiles = m_files.size();
    for (int i = 0; i < numRecentFiles; ++i)
    {
        menuActions[i]->setText(actionTextFromFileName(m_files[i]));
        menuActions[i]->setIconText(m_files[i]);
        menuActions[i]->setToolTip(m_files[i]);
        menuActions[i]->setVisible(true);
    }

    // Make the unused actions invisible.
    for (int i = numRecentFiles; i < m_maxNumFiles; ++i)
        menuActions[i]->setVisible(false);
}


//=============================================================================
// The function "revalidateFileList" removes from the list of recent files any
// files that no longer exist, and trims the list to the maxial allowed length,
// is necessary.
//=============================================================================
void RecentFilesQMenu::revalidateFileList()
{
    auto it = m_files.begin();
    while (it != m_files.end())
    {
        const QFileInfo fileInfo(*it);

        // If the file does not exist anymore, remove it from the list.
        if (fileInfo.exists() == false)
            // Delete the non-existing item and return iterator to the next.
            it = m_files.erase(it);
        else
            ++it;
    }

    // Remove any tail of the recent file list exceededing the allowed length.
    if (m_files.size() > m_maxNumFiles)
        m_files.erase(m_files.begin() + m_maxNumFiles, m_files.end());
}


//=============================================================================
// The function "resizeFileList" sets the maximal allowed number of recent
// files to be kept in the menu and resizes the menu accordingly.
// INPUT: "int maxNumFiles" is maximal allowed number of recent files.
//=============================================================================
void RecentFilesQMenu::resizeFileList(int maxNumFiles)
{
    // Always keep at least one recent file action.
    maxNumFiles = std::max(maxNumFiles, 1);

    if (m_maxNumFiles < maxNumFiles)
    {
        // Add blank, invisible actions to accomodate up to the new maximal
        // allowed number of recent files.
        insertMenuActions(maxNumFiles - m_maxNumFiles);
    }
    else
    {
        // Remove actions to reduce their number to the new maximal allowed
        // number of recent files.
        removeMenuActions(m_maxNumFiles - maxNumFiles);
    }
    m_maxNumFiles = maxNumFiles;

    // Remove any non-existing files and trim the list if necessary.
    revalidateFileList();
    // Update the menu actions.
    updateMenuActions();
}


//=============================================================================
// The function "setFileList" installs a new list of recent files.
// INPUT: "const QStringList & files" is a new list of recent files.
//=============================================================================
void RecentFilesQMenu::setFileList(const QStringList & files)
{
    m_files = files;

    // Remove any non-existing files and trim the list if necessary.
    revalidateFileList();
    // Update the menu actions.
    updateMenuActions();
}


//=============================================================================
// The function "setDoShowDirPathInRecent" possibly changes the way the recent
// files are shown in the recent files menu - with or without full directory
// path.
// INPUT: "bool doShowDirPathInRecent" indicates whether or not the recent
// files are to be shown with full directory path.
//=============================================================================
void RecentFilesQMenu::setDoShowDirPathInRecent(bool doShowDirPathInRecent)
{
    m_doShowDirPathInRecent = doShowDirPathInRecent;

    // Remove any non-existing files and trim the list if necessary.
    revalidateFileList();
    // Update the menu actions.
    updateMenuActions();
}


//=============================================================================
// The function "actionTextFromFileName" produce name for the recent files menu
// action corresponding to a given file according to the preferences.
// INPUT: "const QString & fileName" is a name of a recent file for whose
// action a name is needed.
// OUTPUT: The function returns the text for the action to be named.
//=============================================================================
QString RecentFilesQMenu::actionTextFromFileName(const QString & fileName) const
{
    if (m_doShowDirPathInRecent == true)
        return fileName;
    else
        // Extract the file name without directory path.
        return QFileInfo(fileName).fileName();
}
