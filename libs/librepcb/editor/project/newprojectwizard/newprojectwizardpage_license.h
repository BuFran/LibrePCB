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

#ifndef LIBREPCB_EDITOR_NEWPROJECTWIZARDPAGE_LICENSE_H
#define LIBREPCB_EDITOR_NEWPROJECTWIZARDPAGE_LICENSE_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <librepcb/core/fileio/filepath.h>
#include <librepcb/core/workspace/license.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class Workspace;

namespace editor {

namespace Ui {
class NewProjectWizardPage_License;
}

/*******************************************************************************
 *  Class NewProjectWizardPage_License
 ******************************************************************************/

/**
 * @brief The NewProjectWizardPage_License class
 */
class NewProjectWizardPage_License final : public QWizardPage {
  Q_OBJECT
  Q_DISABLE_COPY(NewProjectWizardPage_License)

public:
  // Constructors / Destructor
  explicit NewProjectWizardPage_License(const Workspace& ws,
                                        QWidget* parent = nullptr) noexcept;

  ~NewProjectWizardPage_License() noexcept;

  // Getters
  License getProjectLicense() const noexcept;
  QList<QString> getFiles() const noexcept;

private:  // GUI Action Handlers
  void licenseChanged(int index) noexcept;
  void openLink(const QString& url) noexcept;

private:  // Methods
  bool isComplete() const noexcept override;
  bool validatePage() noexcept override;

private:  // Data
  const Workspace& mWorkspace;
  QScopedPointer<Ui::NewProjectWizardPage_License> mUi;
  License license{};
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
