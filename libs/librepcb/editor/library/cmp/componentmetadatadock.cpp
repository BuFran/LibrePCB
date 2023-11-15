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
#include "componentmetadatadock.h"

#include "../cat/categorylisteditorwidget.h"
#include "../cmd/cmdcomponentedit.h"
#include "ui_componentmetadatadock.h"

#include <librepcb/core/library/cmp/component.h>

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

ComponentMetadataDock::ComponentMetadataDock(Workspace& workspace) noexcept
  : QDockWidget(nullptr), mUi(new Ui::ComponentMetadataDock) {
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
          &ComponentMetadataDock::modified);
  connect(mUi->edtDescription, &PlainTextEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
  connect(mUi->edtKeywords, &QLineEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
  connect(mUi->edtAuthor, &QLineEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
  connect(mUi->edtVersion, &QLineEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &ComponentMetadataDock::modified);
  connect(mCategoriesEditorWidget.data(), &CategoryListEditorWidget::edited,
          this, &ComponentMetadataDock::modified);
  connect(mUi->cbxSchematicOnly, &QCheckBox::toggled, this,
          &ComponentMetadataDock::modified);
  connect(mUi->cbxSchematicOnly, &QCheckBox::clicked, this,
          &ComponentMetadataDock::modified);
  connect(mUi->edtPrefix, &QLineEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
  connect(mUi->edtDefaultValue, &PlainTextEdit::editingFinished, this,
          &ComponentMetadataDock::modified);
}

ComponentMetadataDock::~ComponentMetadataDock() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/
void ComponentMetadataDock::updateData() noexcept {
  Q_ASSERT(mComponent);

  mUi->edtName->setText(*mComponent->getNames().getDefaultValue());
  mUi->edtDescription->setPlainText(
      mComponent->getDescriptions().getDefaultValue());
  mUi->edtKeywords->setText(mComponent->getKeywords().getDefaultValue());
  mUi->edtAuthor->setText(mComponent->getAuthor());
  mUi->edtVersion->setText(mComponent->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mComponent->isDeprecated());
  mUi->lstMessages->setApprovals(mComponent->getMessageApprovals());
  mCategoriesEditorWidget->setUuids(mComponent->getCategories());
  mUi->cbxSchematicOnly->setChecked(mComponent->isSchematicOnly());
  mUi->edtPrefix->setText(*mComponent->getPrefixes().getDefaultValue());
  mUi->edtDefaultValue->setPlainText(mComponent->getDefaultValue());
}

UndoCommand* ComponentMetadataDock::commitData() {
  Q_ASSERT(mComponent);

  QScopedPointer<CmdComponentEdit> cmd(new CmdComponentEdit(*mComponent));
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
  cmd->setIsSchematicOnly(mUi->cbxSchematicOnly->isChecked());
  try {
    // throws on invalid prefix
    cmd->setPrefix("", ComponentPrefix(mUi->edtPrefix->text().trimmed()));
  } catch (const Exception& e) {
  }
  cmd->setDefaultValue(mUi->edtDefaultValue->toPlainText().trimmed());

  return cmd.take();
}

void ComponentMetadataDock::connectItem(
    const std::shared_ptr<Component>& component,
    EditorWidgetBase::Context* context, IF_RuleCheckHandler* handler) noexcept {
  mComponent = component;

  mUi->edtName->setReadOnly(context->readOnly);
  mUi->edtDescription->setReadOnly(context->readOnly);
  mUi->edtKeywords->setReadOnly(context->readOnly);
  mUi->edtAuthor->setReadOnly(context->readOnly);
  mUi->edtVersion->setReadOnly(context->readOnly);
  mUi->cbxDeprecated->setCheckable(!context->readOnly);
  mUi->lstMessages->setReadOnly(context->readOnly);
  mCategoriesEditorWidget->setReadOnly(context->readOnly);

  mUi->cbxSchematicOnly->setCheckable(!context->readOnly);
  mUi->edtPrefix->setReadOnly(context->readOnly);
  mUi->edtDefaultValue->setReadOnly(context->readOnly);

  updateData();

  show();

  mUi->lstMessages->setHandler(handler);
}

void ComponentMetadataDock::disconnectItem() noexcept {
  mComponent = nullptr;
  hide();
  mUi->lstMessages->setHandler(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

bool ComponentMetadataDock::isSchematicOnly() const noexcept {
  return mUi->cbxSchematicOnly->isChecked();
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentMetadataDock::setMessages(
    const RuleCheckMessageList& messages) noexcept {
  mUi->lstMessages->setMessages(messages);
}

void ComponentMetadataDock::setName(QString name) noexcept {
  mUi->edtName->setText(name);
  emit modified();
}

void ComponentMetadataDock::setAuthor(QString author) noexcept {
  mUi->edtAuthor->setText(author);
  emit modified();
}

void ComponentMetadataDock::addCategory() noexcept {
  mCategoriesEditorWidget->openAddCategoryDialog();
}

void ComponentMetadataDock::setDefaultValue(QString value) noexcept {
  mUi->edtDefaultValue->setPlainText(value);
  emit modified();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
