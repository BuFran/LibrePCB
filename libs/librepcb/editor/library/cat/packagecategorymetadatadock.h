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

#ifndef LIBREPCB_EDITOR_PACKAGECATEGORYMETADATADOCK_H
#define LIBREPCB_EDITOR_PACKAGECATEGORYMETADATADOCK_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../editorwidgetbase.h"

#include <librepcb/core/types/uuid.h>

#include <QtCore>
#include <QtWidgets>

#include <optional.hpp>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class PackageCategory;
class Workspace;

namespace editor {

class CategoryListEditorWidget;
class IF_RuleCheckHandler;

namespace Ui {
class PackageCategoryMetadataDock;
}

/*******************************************************************************
 *  Class PackageCategoryMetadataDock
 ******************************************************************************/

/**
 * @brief The PackageCategoryMetadataDock class
 */
class PackageCategoryMetadataDock final : public QDockWidget {
  Q_OBJECT
  Q_DISABLE_COPY(PackageCategoryMetadataDock)

signals:
  void modified();

public:
  // Constructors / Destructor
  explicit PackageCategoryMetadataDock(Workspace& workspace) noexcept;
  ~PackageCategoryMetadataDock() noexcept override;

  // Setters
  void setName(QString name) noexcept;
  void setAuthor(QString author) noexcept;
  void setMessages(const RuleCheckMessageList& messages) noexcept;

  void updateDisplay() noexcept;
  UndoCommand* commitChanges();

  void connectItem(const std::shared_ptr<PackageCategory>& category,
                   EditorWidgetBase::Context* context,
                   IF_RuleCheckHandler* handler) noexcept;

  void disconnectItem() noexcept;

private:
  // Private methods
  void btnChooseParentCategoryClicked() noexcept;
  void btnResetParentCategoryClicked() noexcept;
  void updateCategoryLabel() noexcept;

private:
  // General
  QScopedPointer<Ui::PackageCategoryMetadataDock> mUi;
  std::shared_ptr<PackageCategory> mCategory;
  tl::optional<Uuid> mParentUuid;
  Workspace& mWorkspace;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
