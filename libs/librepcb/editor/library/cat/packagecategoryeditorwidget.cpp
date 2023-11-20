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
#include "packagecategoryeditorwidget.h"

#include "../cmd/cmdlibrarycategoryedit.h"
#include "categorychooserdialog.h"
#include "categorytreelabeltextbuilder.h"
#include "packagecategorymetadatadock.h"
#include "ui_packagecategoryeditorwidget.h"

#include <librepcb/core/library/cat/packagecategory.h>
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

PackageCategoryEditorWidget::PackageCategoryEditorWidget(const Context& context,
                                                         const FilePath& fp,
                                                         QWidget* parent)
  : EditorWidgetBase(context, fp, parent),
    mUi(new Ui::PackageCategoryEditorWidget) {
  mUi->setupUi(this);

  setWindowIcon(QIcon(":/img/places/folder_green.png"));

  // Load element.
  mCategory = PackageCategory::open(std::unique_ptr<TransactionalDirectory>(
      new TransactionalDirectory(mFileSystem)));  // can throw

  // Reload metadata on undo stack state changes.
  connect(mUndoStack.data(), &UndoStack::stateModified, this,
          &PackageCategoryEditorWidget::updateMetadata);
}

PackageCategoryEditorWidget::~PackageCategoryEditorWidget() noexcept {
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

QSet<EditorWidgetBase::Feature>
    PackageCategoryEditorWidget::getAvailableFeatures() const noexcept {
  return {
      EditorWidgetBase::Feature::Close,
  };
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void PackageCategoryEditorWidget::connectEditor(
    UndoStackActionGroup& undoStackActionGroup,
    ExclusiveActionGroup& toolsActionGroup, QToolBar& commandToolBar,
    StatusBar& statusBar) noexcept {
  EditorWidgetBase::connectEditor(undoStackActionGroup, toolsActionGroup,
                                  commandToolBar, statusBar);

  auto dock = mContext.dockProvider.getDockPackageCategoryMetadata();
  dock->connectItem(mCategory, &mContext, this);

  updateMetadata();
  undoStackStateModified();
  connect(dock.get(), &PackageCategoryMetadataDock::modified, this,
          &PackageCategoryEditorWidget::commitMetadata);
}

void PackageCategoryEditorWidget::disconnectEditor() noexcept {
  auto dock = mContext.dockProvider.getDockPackageCategoryMetadata();
  disconnect(dock.get(), &PackageCategoryMetadataDock::modified, this,
             &PackageCategoryEditorWidget::commitMetadata);

  dock->disconnectItem();

  EditorWidgetBase::disconnectEditor();
}

/*******************************************************************************
 *  Public Slots
 ******************************************************************************/

bool PackageCategoryEditorWidget::save() noexcept {
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

void PackageCategoryEditorWidget::updateMetadata() noexcept {
  setWindowTitle(*mCategory->getNames().getDefaultValue());
  mContext.dockProvider.getDockPackageCategoryMetadata()->updateDisplay();
}

QString PackageCategoryEditorWidget::commitMetadata() noexcept {
  try {
    // Commit all changes.
    mUndoStack->execCmd(mContext.dockProvider.getDockPackageCategoryMetadata()
                            ->commitChanges());  // can throw

    // Reload metadata into widgets to discard invalid input.
    updateMetadata();
  } catch (const Exception& e) {
    return e.getMsg();
  }
  return QString();
}

bool PackageCategoryEditorWidget::runChecks(RuleCheckMessageList& msgs) const {
  msgs = mCategory->runChecks();  // can throw
  mContext.dockProvider.getDockPackageCategoryMetadata()->setMessages(msgs);
  return true;
}

template <>
void PackageCategoryEditorWidget::fixMsg(const MsgNameNotTitleCase& msg) {
  mContext.dockProvider.getDockPackageCategoryMetadata()->setName(
      *msg.getFixedName());
}

template <>
void PackageCategoryEditorWidget::fixMsg(const MsgMissingAuthor& msg) {
  Q_UNUSED(msg);
  mContext.dockProvider.getDockPackageCategoryMetadata()->setAuthor(
      getWorkspaceSettingsUserName());
}

template <typename MessageType>
bool PackageCategoryEditorWidget::fixMsgHelper(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (msg) {
    if (auto m = msg->as<MessageType>()) {
      if (applyFix) fixMsg(*m);  // can throw
      return true;
    }
  }
  return false;
}

bool PackageCategoryEditorWidget::processRuleCheckMessage(
    std::shared_ptr<const RuleCheckMessage> msg, bool applyFix) {
  if (fixMsgHelper<MsgNameNotTitleCase>(msg, applyFix)) return true;
  if (fixMsgHelper<MsgMissingAuthor>(msg, applyFix)) return true;
  return false;
}

void PackageCategoryEditorWidget::ruleCheckApproveRequested(
    std::shared_ptr<const RuleCheckMessage> msg, bool approve) noexcept {
  setMessageApproved(*mCategory, msg, approve);
  updateMetadata();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
