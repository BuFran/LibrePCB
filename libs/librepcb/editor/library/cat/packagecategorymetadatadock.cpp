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
#include "packagecategorymetadatadock.h"

#include "../cat/categorylisteditorwidget.h"
#include "../cmd/cmdlibrarycategoryedit.h"
#include "categorychooserdialog.h"
#include "categorytreelabeltextbuilder.h"
#include "ui_packagecategorymetadatadock.h"

#include <librepcb/core/library/cat/packagecategory.h>
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

PackageCategoryMetadataDock::PackageCategoryMetadataDock(
    Workspace& workspace) noexcept
  : QDockWidget(nullptr),
    mUi(new Ui::PackageCategoryMetadataDock),
    mWorkspace(workspace) {
  mUi->setupUi(this);

  connect(mUi->btnChooseParentCategory, &QToolButton::clicked, this,
          &PackageCategoryMetadataDock::btnChooseParentCategoryClicked);
  connect(mUi->btnResetParentCategory, &QToolButton::clicked, this,
          &PackageCategoryMetadataDock::btnResetParentCategoryClicked);

  connect(mUi->edtName, &QLineEdit::editingFinished, this,
          &PackageCategoryMetadataDock::modified);
  connect(mUi->edtDescription, &PlainTextEdit::editingFinished, this,
          &PackageCategoryMetadataDock::modified);
  connect(mUi->edtKeywords, &QLineEdit::editingFinished, this,
          &PackageCategoryMetadataDock::modified);
  connect(mUi->edtAuthor, &QLineEdit::editingFinished, this,
          &PackageCategoryMetadataDock::modified);
  connect(mUi->edtVersion, &QLineEdit::editingFinished, this,
          &PackageCategoryMetadataDock::modified);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &PackageCategoryMetadataDock::modified);
}

PackageCategoryMetadataDock::~PackageCategoryMetadataDock() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void PackageCategoryMetadataDock::updateDisplay() noexcept {
  Q_ASSERT(mCategory);
  mUi->edtName->setText(*mCategory->getNames().getDefaultValue());
  mUi->edtDescription->setPlainText(
      mCategory->getDescriptions().getDefaultValue());
  mUi->edtKeywords->setText(mCategory->getKeywords().getDefaultValue());
  mUi->edtAuthor->setText(mCategory->getAuthor());
  mUi->edtVersion->setText(mCategory->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mCategory->isDeprecated());
  mUi->lstMessages->setApprovals(mCategory->getMessageApprovals());
  mParentUuid = mCategory->getParentUuid();
  updateCategoryLabel();
}

UndoCommand* PackageCategoryMetadataDock::commitChanges() {
  Q_ASSERT(mCategory);

  QScopedPointer<CmdLibraryCategoryEdit> cmd(
      new CmdLibraryCategoryEdit(*mCategory));
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
  cmd->setParentUuid(mParentUuid);

  return cmd.take();
}

void PackageCategoryMetadataDock::connectItem(
    const std::shared_ptr<PackageCategory>& category,
    EditorWidgetBase::Context* context, IF_RuleCheckHandler* handler) noexcept {
  Q_ASSERT(category);
  Q_ASSERT(context);
  Q_ASSERT(handler);

  mCategory = category;

  mUi->lstMessages->setReadOnly(context->readOnly);
  mUi->edtName->setReadOnly(context->readOnly);
  mUi->edtDescription->setReadOnly(context->readOnly);
  mUi->edtKeywords->setReadOnly(context->readOnly);
  mUi->edtAuthor->setReadOnly(context->readOnly);
  mUi->edtVersion->setReadOnly(context->readOnly);
  mUi->cbxDeprecated->setCheckable(!context->readOnly);
  mUi->btnChooseParentCategory->setEnabled(!context->readOnly);
  mUi->btnResetParentCategory->setEnabled(!context->readOnly);

  updateDisplay();
  show();
  mUi->lstMessages->setHandler(handler);
}

void PackageCategoryMetadataDock::disconnectItem() noexcept {
  mCategory = nullptr;
  hide();
  mUi->lstMessages->setHandler(nullptr);
}

void PackageCategoryMetadataDock::btnChooseParentCategoryClicked() noexcept {
  CategoryChooserDialog dialog(mWorkspace,
                               CategoryChooserDialog::Filter::CmpCat);
  if (dialog.exec()) {
    mParentUuid = dialog.getSelectedCategoryUuid();
    emit modified();
  }
}

void PackageCategoryMetadataDock::btnResetParentCategoryClicked() noexcept {
  mParentUuid = tl::nullopt;
  emit modified();
}

void PackageCategoryMetadataDock::updateCategoryLabel() noexcept {
  const WorkspaceLibraryDb& db = mWorkspace.getLibraryDb();
  PackageCategoryTreeLabelTextBuilder textBuilder(
      db, mWorkspace.getSettings().libraryLocaleOrder.get(), true,
      *mUi->lblParentCategories);
  // TODO getLibLocaleOrder expanded from editorwidgetbase
  textBuilder.updateText(mParentUuid);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void PackageCategoryMetadataDock::setMessages(
    const RuleCheckMessageList& messages) noexcept {
  mUi->lstMessages->setMessages(messages);
}

void PackageCategoryMetadataDock::setName(QString name) noexcept {
  mUi->edtName->setText(name);
  emit modified();
}

void PackageCategoryMetadataDock::setAuthor(QString author) noexcept {
  mUi->edtAuthor->setText(author);
  emit modified();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
