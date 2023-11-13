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
#include "symbolmetadatadock.h"

#include "../cat/categorylisteditorwidget.h"
#include "../cmd/cmdlibraryelementedit.h"
#include "ui_symbolmetadatadock.h"

#include <librepcb/core/library/sym/symbol.h>

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

SymbolMetadataDock::SymbolMetadataDock(Workspace& workspace) noexcept
  : QDockWidget(nullptr), mUi(new Ui::SymbolMetadataDock) {
  mUi->setupUi(this);

  // Insert category list editor widget.
  mCategoriesEditorWidget.reset(new CategoryListEditorWidget(
      workspace, CategoryListEditorWidget::Categories::Component, this));

  mCategoriesEditorWidget->setRequiresMinimumOneEntry(true);
  int row;
  QFormLayout::ItemRole role;
  mUi->formLayout->getWidgetPosition(mUi->lblCategories, &row, &role);
  mUi->formLayout->setWidget(row, QFormLayout::FieldRole,
                             mCategoriesEditorWidget.data());

  connect(mUi->edtName, &QLineEdit::editingFinished, this,
          &SymbolMetadataDock::modified);
  connect(mUi->edtDescription, &PlainTextEdit::editingFinished, this,
          &SymbolMetadataDock::modified);
  connect(mUi->edtKeywords, &QLineEdit::editingFinished, this,
          &SymbolMetadataDock::modified);
  connect(mUi->edtAuthor, &QLineEdit::editingFinished, this,
          &SymbolMetadataDock::modified);
  connect(mUi->edtVersion, &QLineEdit::editingFinished, this,
          &SymbolMetadataDock::modified);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &SymbolMetadataDock::modified);
  connect(mCategoriesEditorWidget.data(), &CategoryListEditorWidget::edited,
          this, &SymbolMetadataDock::modified);
}

SymbolMetadataDock::~SymbolMetadataDock() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/
void SymbolMetadataDock::updateDisplay() noexcept {
  Q_ASSERT(mSymbol);

  mUi->edtName->setText(*mSymbol->getNames().getDefaultValue());
  mUi->edtDescription->setPlainText(
      mSymbol->getDescriptions().getDefaultValue());
  mUi->edtKeywords->setText(mSymbol->getKeywords().getDefaultValue());
  mUi->edtAuthor->setText(mSymbol->getAuthor());
  mUi->edtVersion->setText(mSymbol->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mSymbol->isDeprecated());
  mUi->lstMessages->setApprovals(mSymbol->getMessageApprovals());
  mCategoriesEditorWidget->setUuids(mSymbol->getCategories());
}

UndoCommand* SymbolMetadataDock::commitChanges() {
  Q_ASSERT(mSymbol);

  QScopedPointer<CmdLibraryElementEdit> cmd(
      new CmdLibraryElementEdit(*mSymbol, tr("Edit symbol metadata")));
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
  cmd->setCategories(mCategoriesEditorWidget->getUuids());

  return cmd.take();
}

void SymbolMetadataDock::connectItem(
    const std::shared_ptr<Symbol>& symbol, EditorWidgetBase::Context* context,
    librepcb::editor::IF_RuleCheckHandler* handler) noexcept {
  Q_ASSERT(symbol);
  Q_ASSERT(context);
  Q_ASSERT(handler);

  mSymbol = symbol;

  mUi->edtName->setReadOnly(context->readOnly);
  mUi->edtDescription->setReadOnly(context->readOnly);
  mUi->edtKeywords->setReadOnly(context->readOnly);
  mUi->edtAuthor->setReadOnly(context->readOnly);
  mUi->edtVersion->setReadOnly(context->readOnly);
  mUi->cbxDeprecated->setCheckable(!context->readOnly);
  mUi->lstMessages->setReadOnly(context->readOnly);
  mCategoriesEditorWidget->setReadOnly(context->readOnly);

  updateDisplay();

  show();

  mUi->lstMessages->setHandler(handler);
}

void SymbolMetadataDock::disconnectItem() noexcept {
  mSymbol = nullptr;

  hide();

  mUi->lstMessages->setHandler(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void SymbolMetadataDock::setMessages(
    const RuleCheckMessageList& messages) noexcept {
  mUi->lstMessages->setMessages(messages);
}

void SymbolMetadataDock::setName(QString name) noexcept {
  mUi->edtName->setText(name);
  emit modified();
}

void SymbolMetadataDock::setAuthor(QString author) noexcept {
  mUi->edtAuthor->setText(author);
  emit modified();
}

void SymbolMetadataDock::addCategory() noexcept {
  mCategoriesEditorWidget->openAddCategoryDialog();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
