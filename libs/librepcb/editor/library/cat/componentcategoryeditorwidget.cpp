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
#include "componentcategoryeditorwidget.h"

#include "componentcategorymetadatadock.h"
#include "ui_componentcategoryeditorwidget.h"

#include <librepcb/core/library/cat/componentcategory.h>
#include <librepcb/core/library/librarybaseelementcheckmessages.h>
#include <librepcb/core/workspace/workspace.h>

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

ComponentCategoryEditorWidget::ComponentCategoryEditorWidget(
    const Context& context, const FilePath& fp, QWidget* parent)
  : EditorWidgetBase(context, fp, parent),
    mUi(new Ui::ComponentCategoryEditorWidget) {
  mUi->setupUi(this);

  setWindowIcon(QIcon(":/img/places/folder.png"));

  // Load element.
  mCategory = ComponentCategory::open(std::unique_ptr<TransactionalDirectory>(
      new TransactionalDirectory(mFileSystem)));  // can throw

  // Reload metadata on undo stack state changes.
  connect(mUndoStack.data(), &UndoStack::stateModified, this,
          &ComponentCategoryEditorWidget::updateMetadata);
}

ComponentCategoryEditorWidget::~ComponentCategoryEditorWidget() noexcept {
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

QSet<EditorWidgetBase::Feature>
    ComponentCategoryEditorWidget::getAvailableFeatures() const noexcept {
  return {
      EditorWidgetBase::Feature::Close,
  };
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void ComponentCategoryEditorWidget::connectEditor(
    UndoStackActionGroup& undoStackActionGroup,
    ExclusiveActionGroup& toolsActionGroup, QToolBar& commandToolBar,
    StatusBar& statusBar) noexcept {
  EditorWidgetBase::connectEditor(undoStackActionGroup, toolsActionGroup,
                                  commandToolBar, statusBar);

  auto dock = mContext.dockProvider.getDockComponentCategoryMetadata();
  dock->connectItem(mCategory, &mContext, this);

  updateMetadata();
  undoStackStateModified();
  connect(dock.get(), &ComponentCategoryMetadataDock::modified, this,
          &ComponentCategoryEditorWidget::commitMetadata);
}

void ComponentCategoryEditorWidget::disconnectEditor() noexcept {
  auto dock = mContext.dockProvider.getDockComponentCategoryMetadata();
  disconnect(dock.get(), &ComponentCategoryMetadataDock::modified, this,
             &ComponentCategoryEditorWidget::commitMetadata);

  dock->disconnectItem();

  EditorWidgetBase::disconnectEditor();
}

/*******************************************************************************
 *  Public Slots
 ******************************************************************************/

bool ComponentCategoryEditorWidget::save() noexcept {
  // Remove obsolete message approvals (bypassing the undo stack).
  mCategory->setMessageApprovals(mCategory->getMessageApprovals() -
                                 mDisappearedApprovals);

  // Commit metadata.
  QString errorMsg = commitMetadata();
  if (!errorMsg.isEmpty()) {
    QMessageBox::critical(this, tr("Invalid metadata"), errorMsg);
    return false;
  }

  // Save element.
  try {
    mCategory->save();  // can throw
    mFileSystem->save();  // can throw
    return EditorWidgetBase::save();
  } catch (const Exception& e) {
    QMessageBox::critical(this, tr("Save failed"), e.getMsg());
    return false;
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void ComponentCategoryEditorWidget::updateMetadata() noexcept {
  setWindowTitle(*mCategory->getNames().getDefaultValue());
  mContext.dockProvider.getDockComponentCategoryMetadata()->updateDisplay();
}

QString ComponentCategoryEditorWidget::commitMetadata() noexcept {
  try {
    // Commit all changes.
    mUndoStack->execCmd(
        mContext.dockProvider.getDockComponentCategoryMetadata()
            ->commitChanges());  // can throw

    // Reload metadata into widgets to discard invalid input.
    updateMetadata();
  } catch (const Exception& e) {
    return e.getMsg();
  }
  return QString();
}

bool ComponentCategoryEditorWidget::runChecks(
    RuleCheckMessageList& msgs) const {
  msgs = mCategory->runChecks();  // can throw
  mContext.dockProvider.getDockComponentCategoryMetadata()->setMessages(msgs);
  return true;
}

template <>
void ComponentCategoryEditorWidget::fixMsg(const MsgNameNotTitleCase& msg) {
  mContext.dockProvider.getDockComponentCategoryMetadata()->setName(
      *msg.getFixedName());
}

template <>
void ComponentCategoryEditorWidget::fixMsg(const MsgMissingAuthor& msg) {
  Q_UNUSED(msg)
  mContext.dockProvider.getDockComponentCategoryMetadata()->setAuthor(
      getWorkspaceSettingsUserName());
}

template <typename MessageType>
bool ComponentCategoryEditorWidget::fixMsgHelper(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (msg) {
    if (auto m = msg->as<MessageType>()) {
      if (applyFix) fixMsg(*m);  // can throw
      return true;
    }
  }
  return false;
}

bool ComponentCategoryEditorWidget::processRuleCheckMessage(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (fixMsgHelper<MsgNameNotTitleCase>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingAuthor>(msg, applyFix)) return true;
  return false;
}

void ComponentCategoryEditorWidget::ruleCheckApproveRequested(
    std::shared_ptr<const RuleCheckMessage> msg, bool approve) noexcept {
  setMessageApproved(*mCategory, msg, approve);
  updateMetadata();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
