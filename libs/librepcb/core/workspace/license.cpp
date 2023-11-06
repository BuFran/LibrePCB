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
#include "license.h"

#include "../fileio/fileutils.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

static const char* const LicenseMeta = ".license";
static const char* const KeyLicenseName = "LibrePCBLicense/Name";
static const char* const KeyLicenseUrl = "LibrePCBLicense/Link";
static const char* const KeyLicenseAdditional = "LibrePCBLicense/Additional";

static const QList<QString> RequiredKeys{KeyLicenseName, KeyLicenseUrl};

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

License::License(const librepcb::FilePath& path) noexcept : mPath(path) {
  if (!mPath.isValid()) {
    mName = tr("None");
    return;
  }

  QSettings file(path.getPathTo(LicenseMeta).toNative(), QSettings::IniFormat);

  mName = file.value(KeyLicenseName, path.getBasename()).toString();
  mLink = file.value(KeyLicenseUrl, "").toString();
  mAdditional = file.value(KeyLicenseAdditional, false).toBool();
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

QList<FilePath> License::getFiles() const {
  if (!mPath.isValid() || !mPath.isExistingDir()) {
    return {};
  }
  return FileUtils::getFilesInDirectory(getPath(), QStringList(), false, true);
}

/*******************************************************************************
 *  Static Methods
 ******************************************************************************/

QList<QString> License::detect(const FilePath& path) noexcept {
  auto fn = path.getPathTo(LicenseMeta);
  if (!path.isValid() || !path.isExistingDir() || !fn.isExistingFile()) {
    return {"Missing metainfo file"};
  }

  QList<QString> result;
  QSettings file(fn.toNative(), QSettings::IniFormat);

  foreach (const QString& k, RequiredKeys) {
    if (!file.contains(k)) {
      result.append(QString("Required key %1 missing in the metainfo").arg(k));
    }
  }

  return result;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
