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
#include "devicemetadatadock.h"

#include "../cat/categorylisteditorwidget.h"
#include "../cmd/cmdlibraryelementedit.h"
#include "ui_devicemetadatadock.h"

#include <librepcb/core/library/dev/device.h>

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

DeviceMetadataDock::DeviceMetadataDock(Workspace& workspace) noexcept
  : QDockWidget(nullptr), mUi(new Ui::DeviceMetadataDock) {
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
          &DeviceMetadataDock::modified);
  connect(mUi->edtDescription, &PlainTextEdit::editingFinished, this,
          &DeviceMetadataDock::modified);
  connect(mUi->edtKeywords, &QLineEdit::editingFinished, this,
          &DeviceMetadataDock::modified);
  connect(mUi->edtAuthor, &QLineEdit::editingFinished, this,
          &DeviceMetadataDock::modified);
  connect(mUi->edtVersion, &QLineEdit::editingFinished, this,
          &DeviceMetadataDock::modified);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &DeviceMetadataDock::modified);
  connect(mCategoriesEditorWidget.data(), &CategoryListEditorWidget::edited,
          this, &DeviceMetadataDock::modified);
}

DeviceMetadataDock::~DeviceMetadataDock() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/
void DeviceMetadataDock::updateDisplay() noexcept {
  Q_ASSERT(mDevice);

  mUi->edtName->setText(*mDevice->getNames().getDefaultValue());
  mUi->edtDescription->setPlainText(
      mDevice->getDescriptions().getDefaultValue());
  mUi->edtKeywords->setText(mDevice->getKeywords().getDefaultValue());
  mUi->edtAuthor->setText(mDevice->getAuthor());
  mUi->edtVersion->setText(mDevice->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mDevice->isDeprecated());
  mUi->lstMessages->setApprovals(mDevice->getMessageApprovals());
  mCategoriesEditorWidget->setUuids(mDevice->getCategories());
}

UndoCommand* DeviceMetadataDock::commitChanges() {
  Q_ASSERT(mDevice);

  QScopedPointer<CmdLibraryElementEdit> cmd(
      new CmdLibraryElementEdit(*mDevice, tr("Edit device metadata")));
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

void DeviceMetadataDock::connectItem(const std::shared_ptr<Device>& device,
                                     EditorWidgetBase::Context* context,
                                     IF_RuleCheckHandler* handler) noexcept {
  mDevice = device;

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

void DeviceMetadataDock::disconnectItem() noexcept {
  mDevice = nullptr;
  hide();
  mUi->lstMessages->setHandler(nullptr);
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void DeviceMetadataDock::setMessages(
    const RuleCheckMessageList& messages) noexcept {
  mUi->lstMessages->setMessages(messages);
}

void DeviceMetadataDock::setName(QString name) noexcept {
  mUi->edtName->setText(name);
  emit modified();
}

void DeviceMetadataDock::setAuthor(QString author) noexcept {
  mUi->edtAuthor->setText(author);
  emit modified();
}

void DeviceMetadataDock::addCategory() noexcept {
  mCategoriesEditorWidget->openAddCategoryDialog();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
