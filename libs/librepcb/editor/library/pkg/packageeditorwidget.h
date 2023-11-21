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

#ifndef LIBREPCB_EDITOR_PACKAGEEDITORWIDGET_H
#define LIBREPCB_EDITOR_PACKAGEEDITORWIDGET_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../../graphics/graphicslayer.h"
#include "../../widgets/if_graphicsvieweventhandler.h"
#include "../cat/categorylisteditorwidget.h"
#include "../editorwidgetbase.h"

#include <librepcb/core/library/pkg/footprint.h>
#include <librepcb/core/types/lengthunit.h>
#include <librepcb/core/workspace/theme.h>

#include <QtCore>
#include <QtWidgets>

#include <memory>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class Package;
class PackageModel;

namespace editor {

class GraphicsScene;
class OpenGlSceneBuilder;
class OpenGlView;
class PackageEditorFsm;
struct LayerStackSetup;

namespace Ui {
class PackageEditorWidget;
}

/*******************************************************************************
 *  Class PackageEditorWidget
 ******************************************************************************/

/**
 * @brief The PackageEditorWidget class
 */
class PackageEditorWidget final : public EditorWidgetBase,
                                  public IF_GraphicsViewEventHandler,
                                  public IF_GraphicsLayerProvider {
  Q_OBJECT

public:
  // Constructors / Destructor
  PackageEditorWidget() = delete;
  PackageEditorWidget(const PackageEditorWidget& other) = delete;
  PackageEditorWidget(const Context& context, const FilePath& fp,
                      QWidget* parent = nullptr);
  ~PackageEditorWidget() noexcept;

  // Getters
  QSet<Feature> getAvailableFeatures() const noexcept override;

  // Setters
  void connectEditor(UndoStackActionGroup& undoStackActionGroup,
                     ExclusiveActionGroup& toolsActionGroup,
                     QToolBar& commandToolBar,
                     StatusBar& statusBar) noexcept override;
  void disconnectEditor() noexcept override;

  // Operator Overloadings
  PackageEditorWidget& operator=(const PackageEditorWidget& rhs) = delete;

public slots:
  bool save() noexcept override;
  bool selectAll() noexcept override;
  bool cut() noexcept override;
  bool copy() noexcept override;
  bool paste() noexcept override;
  bool move(Qt::ArrowType direction) noexcept override;
  bool rotate(const librepcb::Angle& rotation) noexcept override;
  bool mirror(Qt::Orientation orientation) noexcept override;
  bool flip(Qt::Orientation orientation) noexcept override;
  bool snapToGrid() noexcept override;
  bool remove() noexcept override;
  bool editProperties() noexcept override;
  bool zoomIn() noexcept override;
  bool zoomOut() noexcept override;
  bool zoomAll() noexcept override;
  bool toggle3D() noexcept override;
  bool abortCommand() noexcept override;
  bool processGenerateOutline() noexcept override;
  bool processGenerateCourtyard() noexcept override;
  bool importDxf() noexcept override;
  bool editGridProperties() noexcept override;
  bool increaseGridInterval() noexcept override;
  bool decreaseGridInterval() noexcept override;

private:  // Methods
  void updateMetadata() noexcept;
  QString commitMetadata() noexcept;
  /// @see ::librepcb::editor::IF_GraphicsViewEventHandler
  bool graphicsViewEventHandler(QEvent* event) noexcept override;
  QList<std::shared_ptr<GraphicsLayer>> getAllLayers() const noexcept override;
  std::shared_ptr<GraphicsLayer> getLayer(
      const QString& name) const noexcept override;
  bool toolChangeRequested(Tool newTool,
                           const QVariant& mode) noexcept override;
  void currentFootprintChanged(int index) noexcept;
  void currentModelChanged(int index) noexcept;
  void scheduleOpenGlSceneUpdate() noexcept;
  void updateOpenGlScene() noexcept;
  void memorizePackageInterface() noexcept;
  bool isInterfaceBroken() const noexcept override;
  bool runChecks(RuleCheckMessageList& msgs) const override;
  template <typename MessageType>
  void fixMsg(const MessageType& msg);
  template <typename MessageType>
  void fixPadFunction(const MessageType& msg);
  template <typename MessageType>
  bool fixMsgHelper(std::shared_ptr<const RuleCheckMessage> msg, bool applyFix);
  bool processRuleCheckMessage(std::shared_ptr<const RuleCheckMessage> msg,
                               bool applyFix) override;
  void ruleCheckApproveRequested(std::shared_ptr<const RuleCheckMessage> msg,
                                 bool approve) noexcept override;
  bool execGraphicsExportDialog(GraphicsExportDialog::Output output,
                                const QString& settingsKey) noexcept override;
  void setGridProperties(const PositiveLength& interval, const LengthUnit& unit,
                         Theme::GridStyle style) noexcept;
  void toggle3DMode(bool enable) noexcept;
  bool is3DModeEnabled() const noexcept;
  static const QList<LayerStackSetup>& getLayerStackSetup() noexcept;

private:  // Data
  QScopedPointer<Ui::PackageEditorWidget> mUi;
  QScopedPointer<OpenGlView> mOpenGlView;
  QScopedPointer<GraphicsScene> mGraphicsScene;
  QScopedPointer<OpenGlSceneBuilder> mOpenGlSceneBuilder;
  bool mOpenGlSceneBuildScheduled;
  LengthUnit mLengthUnit;
  std::shared_ptr<Package> mPackage;
  std::shared_ptr<Footprint> mCurrentFootprint;
  std::shared_ptr<PackageModel> mCurrentModel;

  // broken interface detection
  QSet<Uuid> mOriginalPadUuids;
  FootprintList mOriginalFootprints;

  /// Editor state machine
  QScopedPointer<PackageEditorFsm> mFsm;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
