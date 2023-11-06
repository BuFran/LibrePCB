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

#ifndef UNITTESTS_TESTCONV_H
#define UNITTESTS_TESTCONV_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <librepcb/core/fileio/filepath.h>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace tests {

/*******************************************************************************
 *  TestConv Class
 ******************************************************************************/

/**
 * @brief The TestConv class provides conversion helper methods useful in tests
 *
 * It can be used standalone (calling `TestConv::str`) or be a parent of test
 * via inheritance to simplify the writing to use shorter variant `str`
 */
struct TestConv {
  /**
   * @brief convert QString to gtest readable representation
   *
   * This function converts item to string that is able to compare via
   * ASSERT_EQ macro, and when fail, the output is readable to the user
   *
   * @param string Qt string to convert
   * @return converted string
   */
  static std::string str(const QString& string) noexcept;

  /**
   * @brief convert file path to gtest readable representation
   *
   * This function converts item to string that is able to compare via
   * ASSERT_EQ macro, and when fail, the output is readable to the user
   *
   * @param path path to convert
   * @param root the root that will be stripped from output
   * @return converted string
   */
  static std::string str(const FilePath& path,
                         const FilePath& root = FilePath()) noexcept;

  /**
   * @brief convert list of strings to gtest readable representation
   *
   * This function converts list of items to multi-line string that is able
   * to compare via ASSERT_EQ macros, and when fails, the output is readable
   * to the user
   *
   * @param path list of items to convert
   * @param sort set true if the list should be sorted before concatenation
   * @return converted string
   */
  static std::string str(const QList<QString>& strs, bool sort = true) noexcept;

  /**
   * @brief convert list of paths to gtest readable representation
   *
   * This function converts list of items to multi-line string that is able
   * to compare via ASSERT_EQ macros, and when fails, the output is readable
   * to the user
   *
   * @param path list of paths to convert
   * @param root the root of every path that will be stripped from output
   * @param sort set true if the list should be sorted before concatenation
   * @return converted string
   */
  static std::string str(const QList<FilePath>& paths,
                         const FilePath& root = FilePath(),
                         bool sort = true) noexcept;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace tests
}  // namespace librepcb

#endif
