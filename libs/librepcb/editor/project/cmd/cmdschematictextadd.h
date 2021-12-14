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

#ifndef LIBREPCB_PROJECT_CMDSCHEMATICTEXTADD_H
#define LIBREPCB_PROJECT_CMDSCHEMATICTEXTADD_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <librepcb/common/undocommand.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {
namespace project {

class SI_Text;

/*******************************************************************************
 *  Class CmdSchematicTextAdd
 ******************************************************************************/

/**
 * @brief The CmdSchematicTextAdd class
 */
class CmdSchematicTextAdd final : public UndoCommand {
public:
  // Constructors / Destructor
  CmdSchematicTextAdd() = delete;
  CmdSchematicTextAdd(const CmdSchematicTextAdd& other) = delete;
  explicit CmdSchematicTextAdd(SI_Text& text) noexcept;
  ~CmdSchematicTextAdd() noexcept;

  // Operator Overloadings
  CmdSchematicTextAdd& operator=(const CmdSchematicTextAdd& rhs) = delete;

private:  // Methods
  /// @copydoc ::librepcb::UndoCommand::performExecute()
  bool performExecute() override;

  /// @copydoc ::librepcb::UndoCommand::performUndo()
  void performUndo() override;

  /// @copydoc ::librepcb::UndoCommand::performRedo()
  void performRedo() override;

private:  // Data
  SI_Text& mText;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace project
}  // namespace librepcb

#endif
