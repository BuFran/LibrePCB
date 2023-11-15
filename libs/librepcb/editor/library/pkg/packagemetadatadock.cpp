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
#include "packagemetadatadock.h"

#include "../cat/categorylisteditorwidget.h"
#include "../cmd/cmdpackageedit.h"
#include "ui_packagemetadatadock.h"

#include <librepcb/core/library/pkg/package.h>

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

PackageMetadataDock::PackageMetadataDock(Workspace& workspace) noexcept
  : QDockWidget(nullptr), mUi(new Ui::PackageMetadataDock) {
  mUi->setupUi(this);

  // Insert category list editor widget.
  mCategoriesEditorWidget.reset(new CategoryListEditorWidget(
      workspace, CategoryListEditorWidget::Categories::Package, this));

  mCategoriesEditorWidget->setRequiresMinimumOneEntry(true);
  int row;
  QFormLayout::ItemRole role;
  mUi->formLayout->getWidgetPosition(mUi->lblCategories, &row, &role);
  mUi->formLayout->setWidget(row, QFormLayout::FieldRole,
                             mCategoriesEditorWidget.data());

  // List mount types.
  mUi->cbxAssemblyType->addItem(
      tr("THT (all leads)"), QVariant::fromValue(Package::AssemblyType::Tht));
  mUi->cbxAssemblyType->addItem(
      tr("SMT (all leads)"), QVariant::fromValue(Package::AssemblyType::Smt));
  mUi->cbxAssemblyType->addItem(
      tr("THT+SMT (mixed leads)"),
      QVariant::fromValue(Package::AssemblyType::Mixed));
  mUi->cbxAssemblyType->addItem(
      tr("Other (included in BOM/PnP)"),
      QVariant::fromValue(Package::AssemblyType::Other));
  mUi->cbxAssemblyType->addItem(
      tr("None (excluded from BOM/PnP)"),
      QVariant::fromValue(Package::AssemblyType::None));
  mUi->cbxAssemblyType->addItem(
      tr("Auto-detect (not recommended)"),
      QVariant::fromValue(Package::AssemblyType::Auto));

  // Handle changes of metadata.
  connect(mUi->edtName, &QLineEdit::editingFinished, this,
          &PackageMetadataDock::modified);
  connect(mUi->edtDescription, &PlainTextEdit::editingFinished, this,
          &PackageMetadataDock::modified);
  connect(mUi->edtKeywords, &QLineEdit::editingFinished, this,
          &PackageMetadataDock::modified);
  connect(mUi->edtAuthor, &QLineEdit::editingFinished, this,
          &PackageMetadataDock::modified);
  connect(mUi->edtVersion, &QLineEdit::editingFinished, this,
          &PackageMetadataDock::modified);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &PackageMetadataDock::modified);
  connect(mCategoriesEditorWidget.data(), &CategoryListEditorWidget::edited,
          this, &PackageMetadataDock::modified);
  connect(
      mUi->cbxAssemblyType,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &PackageMetadataDock::modified);
}

PackageMetadataDock::~PackageMetadataDock() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/
void PackageMetadataDock::updateDisplay() noexcept {
  Q_ASSERT(mPackage);

  mUi->edtName->setText(*mPackage->getNames().getDefaultValue());
  mUi->edtDescription->setPlainText(
      mPackage->getDescriptions().getDefaultValue());
  mUi->edtKeywords->setText(mPackage->getKeywords().getDefaultValue());
  mUi->edtAuthor->setText(mPackage->getAuthor());
  mUi->edtVersion->setText(mPackage->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mPackage->isDeprecated());
  mUi->cbxAssemblyType->setCurrentIndex(mUi->cbxAssemblyType->findData(
      QVariant::fromValue(mPackage->getAssemblyType(false))));
  mUi->lstMessages->setApprovals(mPackage->getMessageApprovals());
  mCategoriesEditorWidget->setUuids(mPackage->getCategories());
}

UndoCommand* PackageMetadataDock::commitChanges() {
  Q_ASSERT(mPackage);

  QScopedPointer<CmdPackageEdit> cmd(new CmdPackageEdit(*mPackage));
  try {
    // throws on invalid name
    cmd->setName("", ElementName(mUi->edtName->text().trimmed()));
  } catch (const Exception& e) {
  }
  cmd->setDescription("", mUi->edtDescription->toPlainText().trimmed());
  cmd->setKeywords("", mUi->edtKeywords->text().trimmed());
  try {
    // throws on invalid version
    cmd->setVersion(Version::fromString(mUi->edtVersion->text().trimmed()));
  } catch (const Exception& e) {
  }
  cmd->setAuthor(mUi->edtAuthor->text().trimmed());
  cmd->setDeprecated(mUi->cbxDeprecated->isChecked());
  const QVariant asblyType = mUi->cbxAssemblyType->currentData();
  if (asblyType.isValid() && asblyType.canConvert<Package::AssemblyType>()) {
    cmd->setAssemblyType(asblyType.value<Package::AssemblyType>());
  }
  cmd->setCategories(mCategoriesEditorWidget->getUuids());

  return cmd.take();
}

void PackageMetadataDock::connectItem(const std::shared_ptr<Package>& package,
                                      EditorWidgetBase::Context* context,
                                      IF_RuleCheckHandler* handler) noexcept {
  Q_ASSERT(package);
  Q_ASSERT(context);
  Q_ASSERT(handler);

  mPackage = package;

  mUi->lstMessages->setReadOnly(context->readOnly);
  mUi->edtName->setReadOnly(context->readOnly);
  mUi->edtDescription->setReadOnly(context->readOnly);
  mUi->edtKeywords->setReadOnly(context->readOnly);
  mUi->edtAuthor->setReadOnly(context->readOnly);
  mUi->edtVersion->setReadOnly(context->readOnly);
  mUi->cbxDeprecated->setCheckable(!context->readOnly);
  mUi->cbxAssemblyType->setEnabled(!context->readOnly);

  mUi->lstMessages->setReadOnly(context->readOnly);
  mCategoriesEditorWidget->setReadOnly(context->readOnly);

  updateDisplay();

  show();
  mUi->lstMessages->setHandler(handler);
}

void PackageMetadataDock::disconnectItem() noexcept {
  mPackage = nullptr;
  hide();
  mUi->lstMessages->setHandler(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void PackageMetadataDock::setMessages(
    const RuleCheckMessageList& messages) noexcept {
  mUi->lstMessages->setMessages(messages);
}

void PackageMetadataDock::setName(QString name) noexcept {
  mUi->edtName->setText(name);
  emit modified();
}

void PackageMetadataDock::setAuthor(QString author) noexcept {
  mUi->edtAuthor->setText(author);
  emit modified();
}

void PackageMetadataDock::addCategory() noexcept {
  mCategoriesEditorWidget->openAddCategoryDialog();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
