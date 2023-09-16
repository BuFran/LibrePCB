/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_GUI_EDITORAPPLICATION_H
#define LIBREPCB_GUI_EDITORAPPLICATION_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>

#include <memory>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class FilePath;
class Workspace;

namespace gui {

class EditorWindow;
class ObjectListModel;

/*******************************************************************************
 *  Class EditorApplication
 ******************************************************************************/

/**
 * @brief Top level class of the graphical editor application
 */
class EditorApplication : public QObject {
  Q_OBJECT

public:
  // Constructors / Destructor
  EditorApplication(Workspace& ws, QObject* parent = nullptr);
  EditorApplication(const EditorApplication& other) noexcept = delete;
  virtual ~EditorApplication() noexcept;

  // Properties
  Q_PROPERTY(QString wsPath READ getWorkspacePath NOTIFY workspaceChanged)
  Q_PROPERTY(QAbstractItemModel* openedProjects READ getOpenedProjects CONSTANT)

  // Getters
  QString getWorkspacePath() const noexcept;
  QAbstractItemModel* getOpenedProjects() noexcept;

public slots:  // GUI Handlers
  void createProject() noexcept;
  void openProject() noexcept;

signals:
  void workspaceChanged();

protected:  // Inherited Methods
  virtual bool eventFilter(QObject* watched, QEvent* event) noexcept override;

private:  // Methods
  void openProjectsPassedByCommandLine() noexcept;
  void openProjectPassedByOs(const QString& file, bool silent = false) noexcept;
  void openProject(const FilePath& fp) noexcept;

private:  // Data
  Workspace& mWorkspace;
  QVector<std::shared_ptr<EditorWindow>> mWindows;
  QScopedPointer<ObjectListModel> mOpenedProjects;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace gui
}  // namespace librepcb

#endif
