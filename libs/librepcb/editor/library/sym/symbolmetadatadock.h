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

#ifndef LIBREPCB_EDITOR_SYMBOLMETADATADOCK_H
#define LIBREPCB_EDITOR_SYMBOLMETADATADOCK_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../editorwidgetbase.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class Symbol;
class Workspace;

namespace editor {

class CategoryListEditorWidget;
class IF_RuleCheckHandler;

namespace Ui {
class SymbolMetadataDock;
}

/*******************************************************************************
 *  Class SymbolMetadataDock
 ******************************************************************************/

/**
 * @brief The SymbolMetadataDock class
 */
class SymbolMetadataDock final : public QDockWidget {
  Q_OBJECT
  Q_DISABLE_COPY(SymbolMetadataDock)

signals:
  void modified();

public:
  // Constructors / Destructor
  explicit SymbolMetadataDock(Workspace& workspace) noexcept;
  ~SymbolMetadataDock() noexcept override;

  // Setters
  void setName(QString name) noexcept;
  void setAuthor(QString author) noexcept;
  void setMessages(const RuleCheckMessageList& messages) noexcept;
  void addCategory() noexcept;

  void updateDisplay() noexcept;
  UndoCommand* commitChanges();

  void connectItem(const std::shared_ptr<Symbol>& symbol,
                   EditorWidgetBase::Context* context,
                   IF_RuleCheckHandler* handler) noexcept;

  void disconnectItem() noexcept;

private:
  // General
  QScopedPointer<Ui::SymbolMetadataDock> mUi;
  QScopedPointer<CategoryListEditorWidget> mCategoriesEditorWidget;
  std::shared_ptr<Symbol> mSymbol;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
