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

#ifndef LIBREPCB_GUI_LIBRARYINTERFACE_H
#define LIBREPCB_GUI_LIBRARYINTERFACE_H

/*******************************************************************************
*  Includes
******************************************************************************/
#include <QtCore>
#include <QQmlEngine>

/*******************************************************************************
*  Namespace / Forward Declarations
******************************************************************************/
namespace librepcb {
namespace gui {

/*******************************************************************************
*  Class SchematicInterface
******************************************************************************/

class LibraryInterface : public QObject {
  Q_OBJECT
  QML_ELEMENT

public:
  // Constructors / Destructor
  LibraryInterface() noexcept;
  LibraryInterface(const LibraryInterface& other) noexcept = delete;
  ~LibraryInterface() noexcept override;

  // Operator Overloadings
  LibraryInterface& operator=(const LibraryInterface& rhs) = delete;

  static constexpr const char * const QmlName = "LibraryInterface";
};

/*******************************************************************************
*  End of File
******************************************************************************/

}  // namespace gui
}  // namespace librepcb

#endif
