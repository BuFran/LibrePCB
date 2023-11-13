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

#ifndef LIBREPCB_CORE_LICENSE_H
#define LIBREPCB_CORE_LICENSE_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../fileio/filepath.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class MathParser
 ******************************************************************************/

/**
 * @brief License information parser
 *
 * This class holds all information needed for licensing of new project
 */
class License final {
  Q_DECLARE_TR_FUNCTIONS(License)

public:
  // Constructors / Destructor
  explicit License(const FilePath& path = FilePath()) noexcept;

  // Setters
  // Getters
  /**
   * @brief is this license not empty ?
   */
  bool isSet() const { return mPath.isValid(); }
  /**
   * @brief Get the displayed name of the license
   */
  QString getName() const { return mName; }
  /**
   * @brief Get the path of license folder
   */
  FilePath getPath() const { return mPath; };
  /**
   * @brief Get the link (url) to show to the user
   */
  QString getLink() const { return mLink; }

  /**
   * @brief Get the additional steps user need to run
   */
  bool getAdditional() const { return mAdditional; }

  /**
   * @brief Get all files connected to this license
   *
   * @return list of files to be copied to project
   */
  QList<FilePath> getFiles() const;

  /**
   * @brief Returns all issues with the license
   *
   * @param path the path to test license
   */
  static QList<QString> detect(const FilePath& path) noexcept;

private:
  FilePath mPath;  ///< License root directory
  QString mName{};  ///< license displayed name
  QString mLink{};  ///< license link
  bool mAdditional{false};
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

Q_DECLARE_METATYPE(librepcb::License)

#endif
