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
#include "librarylayersdock.h"

#include "ui_librarylayersdock.h"

#include <librepcb/core/types/layer.h>

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

LibraryLayersDock::LibraryLayersDock() noexcept
  : QDockWidget(nullptr),
    mUi(new Ui::LibraryLayersDock),
    mUpdateScheduled(true),
    mOnLayerEditedSlot(*this, &LibraryLayersDock::layerEdited) {
  mUi->setupUi(this);
}

LibraryLayersDock::~LibraryLayersDock() noexcept {
}

void LibraryLayersDock::connectItem(const IF_GraphicsLayerProvider* lp,
                                    const QList<LayerStackSetup>& lc) noexcept {
  mLayerProvider = lp;

  foreach (auto& layer, mLayerProvider->getAllLayers()) {
    layer->onEdited.attach(mOnLayerEditedSlot);
  }

  updateListWidget();
  updateButtons(lc);
  show();
}

void LibraryLayersDock::disconnectItem() noexcept {
  foreach (auto& layer, mLayerProvider->getAllLayers()) {
    layer->onEdited.detach(mOnLayerEditedSlot);
  }

  // remove buttons
  QLayoutItem* item;
  while ((item = mUi->horizontalLayout->takeAt(0))) {
    delete item;
  }
  mLayerProvider = nullptr;
  hide();
}

/*******************************************************************************
 *  Private Slots
 ******************************************************************************/

void LibraryLayersDock::on_listWidget_itemChanged(QListWidgetItem* item) {
  const QString name = item->data(Qt::UserRole).toString();
  if (std::shared_ptr<GraphicsLayer> layer = mLayerProvider->getLayer(name)) {
    layer->setVisible(item->checkState() == Qt::Checked);
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void LibraryLayersDock::layerEdited(const GraphicsLayer& layer,
                                    GraphicsLayer::Event event) noexcept {
  Q_UNUSED(layer);
  switch (event) {
    case GraphicsLayer::Event::ColorChanged:
    case GraphicsLayer::Event::VisibleChanged:
    case GraphicsLayer::Event::EnabledChanged:
      mUpdateScheduled = true;
      QTimer::singleShot(10, this, &LibraryLayersDock::updateListWidget);
      break;
    case GraphicsLayer::Event::HighlightColorChanged:
      break;
    default:
      qWarning() << "Unhandled switch-case in "
                    "PackageLayersDock::layerEdited():"
                 << static_cast<int>(event);
      break;
  }
}

void LibraryLayersDock::updateListWidget() noexcept {
  if (!mUpdateScheduled) {
    return;
  }

  QList<QString> layerNames = getAllLayers();

  mUi->listWidget->setUpdatesEnabled(false);
  mUi->listWidget->blockSignals(true);
  bool simpleUpdate = (mUi->listWidget->count() == layerNames.count());
  if (!simpleUpdate) {
    mUi->listWidget->clear();
  }
  for (int i = 0; i < layerNames.count(); i++) {
    QString layerName = layerNames.at(i);
    std::shared_ptr<GraphicsLayer> layer = mLayerProvider->getLayer(layerName);
    Q_ASSERT(layer);
    QListWidgetItem* item = nullptr;
    if (simpleUpdate) {
      item = mUi->listWidget->item(i);
      Q_ASSERT(item);
    } else {
      item = new QListWidgetItem(layer->getNameTr());
    }
    item->setData(Qt::UserRole, layerName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(layer->getVisible() ? Qt::Checked : Qt::Unchecked);
    QColor color = layer->getColor(false);
    color.setAlphaF(0.3);
    item->setBackground(color);
    // still add, but hide disabled layers because of the condition above:
    // "mUi->listWidget->count() == layerNames.count()"
    item->setHidden(!layer->isEnabled());
    if (!simpleUpdate) {
      mUi->listWidget->addItem(item);
    }
  }
  mUi->listWidget->blockSignals(false);
  mUi->listWidget->setUpdatesEnabled(true);
  mUpdateScheduled = false;
}

void LibraryLayersDock::setVisibleLayers(
    const QList<QString>& layers) noexcept {
  foreach (auto& layer, mLayerProvider->getAllLayers()) {
    layer->setVisible(layers.contains(layer->getName()));
  }
}

QList<QString> LibraryLayersDock::getAllLayers() const noexcept {
  QList<QString> layers;
  foreach (auto& layer, mLayerProvider->getAllLayers()) {
    if (layer->isEnabled()) {
      layers.append(layer->getName());
    }
  }
  return layers;
}

void LibraryLayersDock::updateButtons(
    const QList<LayerStackSetup>& cfg) noexcept {
  foreach (const auto& item, cfg) {
    auto tb = new QToolButton(this);
    tb->setText(item.DisplayName);
    connect(tb, &QToolButton::clicked,
            [this, item] { setVisibleLayers(item.VisibleLayers); });
    tb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    mUi->horizontalLayout->addWidget(tb);
  }

  auto tbAll = new QToolButton(this);
  tbAll->setText(tr("ALL"));
  connect(tbAll, &QToolButton::clicked,
          [this] { setVisibleLayers(getAllLayers()); });
  tbAll->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  mUi->horizontalLayout->addWidget(tbAll);

  auto tbNone = new QToolButton(this);
  tbNone->setText(tr("NONE"));
  connect(tbNone, &QToolButton::clicked, [this] { setVisibleLayers({}); });
  tbNone->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  mUi->horizontalLayout->addWidget(tbNone);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
