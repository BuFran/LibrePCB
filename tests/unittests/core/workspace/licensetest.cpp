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
#include <librepcb/core/fileio/fileutils.h>
#include <librepcb/core/workspace/license.h>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace tests {

/*******************************************************************************
 *  Test Class
 ******************************************************************************/

class LicenseTest : public ::testing::Test, public TestConv {
public:
  LicenseTest();
  ~LicenseTest() override;

  FilePath root{FilePath::getRandomTempPath()};

  FilePath ValidLicense{root.getPathTo("ValidLicense")};
  FilePath MissingMeta{root.getPathTo("MissingMeta")};

  FilePath MissingNameKey{root.getPathTo("NoName")};
  FilePath MissingLinkKey{root.getPathTo("NoLink")};

private:  // constants to be used
  const QString LicenseContent = "Test text\n";

  const QString ValidMeta =
      "[LibrePCBLicense]\n"
      "Name = First License\n"
      "Link = https://cern-ohl.web.cern.ch/\n";

  const QString NoLinkMeta =
      "[LibrePCBLicense]\n"
      "Name = First License\n";

  const QString NoNameMeta =
      "[LibrePCBLicense]\n"
      "Link = https://cern-ohl.web.cern.ch/\n";

  const QString AdditionalFlag = "Additional = true\n";

  static void buildContent(const FilePath& base, const QString& file,
                           const QString& content) {
    FileUtils::makePath(base);
    FileUtils::writeFile(base.getPathTo(file), content.toUtf8());
  }
};

LicenseTest::LicenseTest() {
  // create ValidLicense
  buildContent(ValidLicense, ".license", ValidMeta + AdditionalFlag);
  buildContent(ValidLicense, "LICENSE.txt", LicenseContent);

  // create MissingMeta
  buildContent(MissingMeta, "LICENSE.txt", LicenseContent);

  // create MissingName
  buildContent(MissingNameKey, ".license", NoNameMeta);
  buildContent(MissingNameKey, "LICENSE.txt", LicenseContent);

  // create MissingLink
  buildContent(MissingLinkKey, ".license", NoLinkMeta);
  buildContent(MissingLinkKey, "LICENSE.txt", LicenseContent);
}

LicenseTest::~LicenseTest() {
  FileUtils::removeDirRecursively(root);
}

/*******************************************************************************
 *  Test Methods
 ******************************************************************************/

TEST_F(LicenseTest, testEmpty) {
  License lic{};

  EXPECT_FALSE(lic.isSet());
  EXPECT_FALSE(lic.getPath().isValid());
  EXPECT_EQ("None", lic.getName());
  EXPECT_EQ("", lic.getPath().toNative());
  EXPECT_EQ("", lic.getLink());
  EXPECT_FALSE(lic.getAdditional());
  EXPECT_EQ("", str(lic.getFiles()));
}

TEST_F(LicenseTest, testDetectValid) {

  EXPECT_EQ("", str(License::detect(ValidLicense)));
}

TEST_F(LicenseTest, testDetectMissingMeta) {
  auto res = License::detect(MissingMeta);

  EXPECT_EQ("Missing metainfo file", str(res));
}

TEST_F(LicenseTest, testDetectMissingRequiredKey) {

  EXPECT_EQ("Required key LibrePCBLicense/Name missing in the metainfo",
            str(License::detect(MissingNameKey)));

  EXPECT_EQ("Required key LibrePCBLicense/Link missing in the metainfo",
            str(License::detect(MissingLinkKey)));
}

TEST_F(LicenseTest, testSimple) {
  License lic{ValidLicense};

  EXPECT_TRUE(lic.isSet());
  EXPECT_TRUE(lic.getPath().isValid());
  EXPECT_EQ("ValidLicense", str(lic.getPath(), root));
  EXPECT_EQ("First License", lic.getName());
  EXPECT_EQ("https://cern-ohl.web.cern.ch/", lic.getLink());
  EXPECT_TRUE(lic.getAdditional());
  EXPECT_EQ("LICENSE.txt", str(lic.getFiles(), lic.getPath()));
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace tests
}  // namespace librepcb
