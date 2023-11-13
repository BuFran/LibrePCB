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
#include "testconv.h"

#include <gtest/gtest.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace tests {

/*******************************************************************************
 *  Static Methods
 ******************************************************************************/

std::string TestConv::str(const QString& string) noexcept {
  return string.toStdString();
}

std::string TestConv::str(const FilePath& path, const FilePath& root) noexcept {
  if (root.isValid())
    return path.toRelative(root).toStdString();
  else
    return path.toStr().toStdString();
}

std::string TestConv::str(const QList<QString>& strs, bool sort) noexcept {
  QStringList result{strs};
  if (sort) {
    result.sort();
  }
  return result.join("\n").toStdString();
}

std::string TestConv::str(const QList<FilePath>& paths, const FilePath& root,
                          bool sort) noexcept {
  QStringList result;
  foreach (const auto& fp, paths) {
    if (root.isValid())
      result.append(fp.toRelative(root));
    else
      result.append(fp.toStr());
  }
  if (sort) {
    result.sort();
  }
  return result.join("\n").toStdString();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace tests
}  // namespace librepcb
