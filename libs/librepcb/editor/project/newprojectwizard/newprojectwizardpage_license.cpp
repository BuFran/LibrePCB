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

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "newprojectwizardpage_license.h"

#include "../../dialogs/filedialog.h"
#include "../../editorcommandset.h"
#include "../../workspace/desktopservices.h"
#include "ui_newprojectwizardpage_license.h"

#include <librepcb/core/application.h>
#include <librepcb/core/workspace/licensedb.h>
#include <librepcb/core/workspace/workspace.h>
#include <librepcb/core/workspace/workspacesettings.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

NewProjectWizardPage_License::NewProjectWizardPage_License(
    const Workspace& ws, QWidget* parent) noexcept
  : QWizardPage(parent),
    mWorkspace(ws),
    mUi(new Ui::NewProjectWizardPage_License) {
  mUi->setupUi(this);
  setPixmap(QWizard::LogoPixmap, QPixmap(":/img/actions/plus_2.png"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/img/wizards/watermark.jpg"));

  // signal/slot connections
  connect(mUi->cbxAddLicense, &QGroupBox::toggled, this,
          &NewProjectWizardPage_License::completeChanged);
  connect(mUi->cbxLicense, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &NewProjectWizardPage_License::licenseChanged);
  connect(mUi->lblLicenseLink, &QLabel::linkActivated, this,
          &NewProjectWizardPage_License::openLink);
  connect(mUi->lstFiles, &QListWidget::itemChanged, this,
          &NewProjectWizardPage_License::completeChanged);


  // insert values
  for (const auto& lic : ws.getLicenses().all())
    mUi->cbxLicense->addItem(lic.getName(), QVariant::fromValue(lic));

  mUi->cbxLicense->setCurrentIndex(0);  // no license
}

NewProjectWizardPage_License::~NewProjectWizardPage_License() noexcept {
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

License NewProjectWizardPage_License::getProjectLicense() const noexcept {
  if (!mUi->cbxAddLicense->isChecked())
    return License{};
  return mUi->cbxLicense->currentData(Qt::UserRole).value<License>();
}

QList<QString> NewProjectWizardPage_License::getFiles() const noexcept
{
  QList<QString> result;
  if (!mUi->cbxAddLicense->isChecked())
    return result;

  for(int i = 0; i < mUi->lstFiles->count(); ++i) {
    auto item = mUi->lstFiles->item(i);
    if (item->checkState() == Qt::Checked) {
      result.append(item->text());
    }
  }

  return result;
}

/*******************************************************************************
 *  GUI Action Handlers
 ******************************************************************************/

void NewProjectWizardPage_License::licenseChanged(int index) noexcept {
  Q_UNUSED(index)

  const QString SButton =
      tr("<a href=\"%1\"><img src=\":/img/places/www.png\" width=\"25\" "
         "height=\"25\"/></a>");

  license = getProjectLicense();

  auto link = license.getLink();

  mUi->lblLicenseLink->setText(SButton.arg(link));
  mUi->lblLicenseLink->setToolTip(link);
  mUi->lblLicenseLink->setVisible(!link.isEmpty());

  mUi->lstFiles->clear();
  for (const auto& p : license.getFiles()) {
    auto item = new QListWidgetItem(p.toRelative(license.getPath()), mUi->lstFiles);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
  }

  emit completeChanged();
}

void NewProjectWizardPage_License::openLink(const QString& url) noexcept {
  DesktopServices ds(mWorkspace.getSettings(), this);
  ds.openWebUrl(QUrl(url));
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

bool NewProjectWizardPage_License::isComplete() const noexcept {
  // check base class
  if (!QWizardPage::isComplete()) {
    return false;
  }

  return !mUi->cbxAddLicense->isChecked() || !getFiles().isEmpty();
}

bool NewProjectWizardPage_License::validatePage() noexcept {
  // check base class
  if (!QWizardPage::validatePage()) {
    return false;
  }

  // are we licensing ?
  if (!mUi->cbxAddLicense->isChecked()) {
    return true;
  }

  // we need exactly one file with LICENSE filename (either pdf o txt)
  int count = 0;
  for (const auto &f : getFiles()) {
    if (f.contains("LICENSE", Qt::CaseSensitive))
      count++;
  }

  if (count != 1) {
    QMessageBox::critical(
        this, tr("Invalid license"),
        tr("There should be single file with name LICENSE selected"));
    return false;
  }

  return true;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
