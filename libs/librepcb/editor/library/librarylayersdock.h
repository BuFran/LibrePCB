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

#ifndef LIBREPCB_EDITOR_LIBRARYLAYERSDOCK_H
#define LIBREPCB_EDITOR_LIBRARYLAYERSDOCK_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../graphics/graphicslayer.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class Layer;

namespace editor {

struct LayerStackSetup;

namespace Ui {
class LibraryLayersDock;
}

/*******************************************************************************
 *  Class LibraryLayersDock
 ******************************************************************************/

/**
 * @brief The LibraryLayersDock class
 */
class LibraryLayersDock final : public QDockWidget {
  Q_OBJECT
  Q_DISABLE_COPY(LibraryLayersDock)

public:
  // Constructors / Destructor
  LibraryLayersDock() noexcept;
  ~LibraryLayersDock() noexcept;

  void connectItem(const IF_GraphicsLayerProvider* lp,
                   const QList<LayerStackSetup>& lc) noexcept;
  void disconnectItem() noexcept;

private slots:
  void on_listWidget_itemChanged(QListWidgetItem* item);

private:
  // Private Methods
  void layerEdited(const GraphicsLayer& layer,
                   GraphicsLayer::Event event) noexcept;
  void updateListWidget() noexcept;
  void updateButtons(const QList<LayerStackSetup>& cfg) noexcept;
  void setVisibleLayers(const QList<QString>& layers) noexcept;
  QList<QString> getAllLayers() const noexcept;

  // General
  const IF_GraphicsLayerProvider* mLayerProvider;
  QScopedPointer<Ui::LibraryLayersDock> mUi;
  bool mUpdateScheduled;

  // Slots
  GraphicsLayer::OnEditedSlot mOnLayerEditedSlot;
};

struct LayerStackSetup {
  QString DisplayName;
  QList<QString> VisibleLayers;
};


/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
