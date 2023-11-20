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
#include "componenteditorwidget.h"

#include "../../widgets/signalrolecombobox.h"
#include "../cmd/cmdcomponentedit.h"
#include "../cmd/cmdcomponentsignaledit.h"
#include "../cmd/cmdcomponentsymbolvariantedit.h"
#include "componentmetadatadock.h"
#include "componentsymbolvarianteditdialog.h"
#include "ui_componenteditorwidget.h"

#include <librepcb/core/library/cmp/component.h>
#include <librepcb/core/library/cmp/componentcheckmessages.h>
#include <librepcb/core/library/librarybaseelementcheckmessages.h>
#include <librepcb/core/library/libraryelementcheckmessages.h>

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

ComponentEditorWidget::ComponentEditorWidget(const Context& context,
                                             const FilePath& fp,
                                             QWidget* parent)
  : EditorWidgetBase(context, fp, parent), mUi(new Ui::ComponentEditorWidget) {
  mUi->setupUi(this);

  mUi->signalEditorWidget->setReadOnly(mContext.readOnly);
  mUi->symbolVariantsEditorWidget->setReadOnly(mContext.readOnly);
  mUi->attributesEditorWidget->setReadOnly(mContext.readOnly);
  setupErrorNotificationWidget(*mUi->errorNotificationWidget);
  setWindowIcon(QIcon(":/img/library/component.png"));

  // Load element.
  mComponent.reset(Component::open(std::unique_ptr<TransactionalDirectory>(
                                       new TransactionalDirectory(mFileSystem)))
                       .release());  // can throw
  mUi->signalEditorWidget->setReferences(mUndoStack.data(),
                                         &mComponent->getSignals());
  mUi->symbolVariantsEditorWidget->setReferences(
      mUndoStack.data(), &mComponent->getSymbolVariants(), this);

  // Load attribute editor.
  mUi->attributesEditorWidget->setReferences(mUndoStack.data(),
                                             &mComponent->getAttributes());

  // Show "interface broken" warning when related properties are modified.
  memorizeComponentInterface();
  setupInterfaceBrokenWarningWidget(*mUi->interfaceBrokenWarningWidget);

  // Reload metadata on undo stack state changes.
  connect(mUndoStack.data(), &UndoStack::stateModified, this,
          &ComponentEditorWidget::updateMetadata);
}

ComponentEditorWidget::~ComponentEditorWidget() noexcept {
  mUi->attributesEditorWidget->setReferences(nullptr, nullptr);
  mUi->signalEditorWidget->setReferences(nullptr, nullptr);
  mUi->symbolVariantsEditorWidget->setReferences(nullptr, nullptr, nullptr);
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

QSet<EditorWidgetBase::Feature> ComponentEditorWidget::getAvailableFeatures()
    const noexcept {
  return {
      EditorWidgetBase::Feature::Close,
  };
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentEditorWidget::connectEditor(
    UndoStackActionGroup& undoStackActionGroup,
    ExclusiveActionGroup& toolsActionGroup, QToolBar& commandToolBar,
    StatusBar& statusBar) noexcept {
  EditorWidgetBase::connectEditor(undoStackActionGroup, toolsActionGroup,
                                  commandToolBar, statusBar);
  auto dock = mContext.dockProvider.getDockComponentMetadata();
  dock->connectItem(mComponent, &mContext, this);

  updateMetadata();
  undoStackStateModified();
  connect(dock.get(), &ComponentMetadataDock::modified, this,
          &ComponentEditorWidget::commitMetadata);
}

void ComponentEditorWidget::disconnectEditor() noexcept {
  auto dock = mContext.dockProvider.getDockComponentMetadata();
  disconnect(dock.get(), &ComponentMetadataDock::modified, this,
             &ComponentEditorWidget::commitMetadata);

  dock->disconnectItem();

  EditorWidgetBase::disconnectEditor();
}

/*******************************************************************************
 *  Public Slots
 ******************************************************************************/

bool ComponentEditorWidget::save() noexcept {
  // Remove obsolete message approvals (bypassing the undo stack).
  mComponent->setMessageApprovals(mComponent->getMessageApprovals() -
                                  mDisappearedApprovals);

  // Commit metadata.
  QString errorMsg = commitMetadata();
  if (!errorMsg.isEmpty()) {
    QMessageBox::critical(this, tr("Invalid metadata"), errorMsg);
    return false;
  }

  // Save element.
  try {
    mComponent->save();  // can throw
    mFileSystem->save();  // can throw
    memorizeComponentInterface();
    return EditorWidgetBase::save();
  } catch (const Exception& e) {
    QMessageBox::critical(this, tr("Save failed"), e.getMsg());
    return false;
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void ComponentEditorWidget::updateMetadata() noexcept {
  setWindowTitle(*mComponent->getNames().getDefaultValue());
  mContext.dockProvider.getDockComponentMetadata()->updateData();
}

QString ComponentEditorWidget::commitMetadata() noexcept {
  try {
    // Commit all changes.
    mUndoStack->execCmd(mContext.dockProvider.getDockComponentMetadata()
                            ->commitData());  // can throw

    // Reload metadata into widgets to discard invalid input.
    updateMetadata();
  } catch (const Exception& e) {
    return e.getMsg();
  }
  return QString();
}

bool ComponentEditorWidget::openComponentSymbolVariantEditor(
    std::shared_ptr<ComponentSymbolVariant> variant) noexcept {
  ComponentSymbolVariantEditDialog dialog(mContext.workspace, mComponent,
                                          variant);
  dialog.setReadOnly(mContext.readOnly);
  return (dialog.exec() == QDialog::Accepted);
}

void ComponentEditorWidget::memorizeComponentInterface() noexcept {
  mOriginalIsSchematicOnly = mComponent->isSchematicOnly();
  mOriginalSignalUuids = mComponent->getSignals().getUuidSet();
  mOriginalSymbolVariants = mComponent->getSymbolVariants();
}

bool ComponentEditorWidget::isInterfaceBroken() const noexcept {
  if (mContext.dockProvider.getDockComponentMetadata()->isSchematicOnly() !=
      mOriginalIsSchematicOnly)
    return true;
  if (mComponent->getSignals().getUuidSet() != mOriginalSignalUuids)
    return true;
  for (const ComponentSymbolVariant& original : mOriginalSymbolVariants) {
    const ComponentSymbolVariant* current =
        mComponent->getSymbolVariants().find(original.getUuid()).get();
    if (!current) return true;
    if (current->getSymbolItems().getUuidSet() !=
        original.getSymbolItems().getUuidSet())
      return true;
    for (const ComponentSymbolVariantItem& originalItem :
         original.getSymbolItems()) {
      const ComponentSymbolVariantItem* currentItem =
          current->getSymbolItems().find(originalItem.getUuid()).get();
      if (currentItem->getSymbolUuid() != originalItem.getSymbolUuid())
        return true;
      if (currentItem->getPinSignalMap().getUuidSet() !=
          originalItem.getPinSignalMap().getUuidSet())
        return true;
      for (const ComponentPinSignalMapItem& originalMap :
           originalItem.getPinSignalMap()) {
        const ComponentPinSignalMapItem* currentMap =
            currentItem->getPinSignalMap().find(originalMap.getUuid()).get();
        if (!currentMap) return true;
        if (currentMap->getSignalUuid() != originalMap.getSignalUuid())
          return true;
      }
    }
  }
  return false;
}

bool ComponentEditorWidget::runChecks(RuleCheckMessageList& msgs) const {
  msgs = mComponent->runChecks();  // can throw
  mContext.dockProvider.getDockComponentMetadata()->setMessages(msgs);
  return true;
}

template <>
void ComponentEditorWidget::fixMsg(const MsgNameNotTitleCase& msg) {
  mContext.dockProvider.getDockComponentMetadata()->setName(
      *msg.getFixedName());
}

template <>
void ComponentEditorWidget::fixMsg(const MsgMissingAuthor& msg) {
  Q_UNUSED(msg)
  mContext.dockProvider.getDockComponentMetadata()->setAuthor(
      getWorkspaceSettingsUserName());
}

template <>
void ComponentEditorWidget::fixMsg(const MsgMissingCategories& msg) {
  Q_UNUSED(msg)
  mContext.dockProvider.getDockComponentMetadata()->addCategory();
}

template <>
void ComponentEditorWidget::fixMsg(const MsgMissingComponentDefaultValue& msg) {
  Q_UNUSED(msg);
  // User has to answer the one-million-dollar question :-)
  QString title = tr("Determine default value");
  QString question =
      tr("Is this rather a (manufacturer-)specific component than a generic "
         "component?");
  int answer = QMessageBox::question(this, title, question, QMessageBox::Cancel,
                                     QMessageBox::Yes, QMessageBox::No);
  if (answer == QMessageBox::Yes) {
    mContext.dockProvider.getDockComponentMetadata()->setDefaultValue(
        "{{MPN or DEVICE or COMPONENT}}");
  } else if (answer == QMessageBox::No) {
    mContext.dockProvider.getDockComponentMetadata()->setDefaultValue(
        "{{MPN or DEVICE}}");
  }
}

template <>
void ComponentEditorWidget::fixMsg(const MsgMissingSymbolVariant& msg) {
  Q_UNUSED(msg);
  std::shared_ptr<ComponentSymbolVariant> symbVar =
      std::make_shared<ComponentSymbolVariant>(Uuid::createRandom(), "",
                                               ElementName("default"), "");
  mUndoStack->execCmd(new CmdComponentSymbolVariantInsert(
      mComponent->getSymbolVariants(), symbVar));
}

template <>
void ComponentEditorWidget::fixMsg(
    const MsgNonFunctionalComponentSignalInversionSign& msg) {
  std::shared_ptr<ComponentSignal> signal =
      mComponent->getSignals().get(msg.getSignal().get());
  QScopedPointer<CmdComponentSignalEdit> cmd(
      new CmdComponentSignalEdit(*signal));
  cmd->setName(CircuitIdentifier("!" % signal->getName()->mid(1)));
  mUndoStack->execCmd(cmd.take());
}

template <typename MessageType>
bool ComponentEditorWidget::fixMsgHelper(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (msg) {
    if (auto m = msg->as<MessageType>()) {
      if (applyFix) fixMsg(*m);  // can throw
      return true;
    }
  }
  return false;
}

bool ComponentEditorWidget::processRuleCheckMessage(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (fixMsgHelper<MsgNameNotTitleCase>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingAuthor>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingCategories>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingComponentDefaultValue>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingSymbolVariant>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgNonFunctionalComponentSignalInversionSign>(msg, applyFix))
    return true;
  return false;
}

void ComponentEditorWidget::ruleCheckApproveRequested(
    std::shared_ptr<const RuleCheckMessage> msg, bool approve) noexcept {
  setMessageApproved(*mComponent, msg, approve);
  updateMetadata();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
