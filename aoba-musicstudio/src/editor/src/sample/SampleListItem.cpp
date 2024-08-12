#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <sft/instrument/Instrument.hpp>
#include <sft/sample/WavSampleWriter.hpp>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/mml/MmlEditor.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderWidget.hpp>
#include <editor/render/RenderButton.hpp>
#include <editor/sample/SampleListWidget.hpp>
#include <editor/sample/SampleListItem.hpp>
#include <editor/sample/InstrumentEditor.hpp>
#include <editor/project/ProjectContext.hpp>

using namespace Editor;

// --------------------------------------------------------------------
SampleListItem::SampleListItem(SampleListWidget *parent, ProjectContext *context, const Sft::InstrumentPtr &instr, const QRect &pos)
  : Renderable(parent, pos)
  , mSampleList(parent)
  , mContext(context)
  , mInstrument(instr)
{
  mMuteButton = new RenderButton(parent, RenderTheme::IconSpeaker, QRect());
  mRenderables.push_back(mMuteButton);

  connect(instr.get(), &Sft::Instrument::nameChanged, this, &SampleListItem::redraw);
  connect(instr.get(), &Sft::Instrument::colorChanged, this, &SampleListItem::redraw);
  
  setPosition(pos);
}

// --------------------------------------------------------------------
void SampleListItem::redraw() {
  mParent->update();
}

// --------------------------------------------------------------------
void SampleListItem::draw(RenderHelper &h) {
  h.listItemBackground(mPosition, h.color(mInstrument->color()));
  
  int totalButtonWidth = 24;
  h.text(QRect(
    mPosition.x() + h.config()->defaultTextMargin(),
    mPosition.y(),
    mPosition.width() - h.config()->defaultTextMargin() - totalButtonWidth,
    mPosition.height()
  ),
    mInstrument->name(),
    h.color(QPalette::WindowText), RenderTheme::FontDefault, Qt::AlignVCenter
  );

  drawAllRenderables(h);
}

// --------------------------------------------------------------------
void SampleListItem::setPosition(const QRect &pos) {
  mPosition = pos;

  int w = pos.width();
  int halfHeight = pos.height() / 2;
  mMuteButton->setPosition(QRect(pos.x() + w - 10 - halfHeight, pos.y() + halfHeight - 10, 20, 20));
}

// --------------------------------------------------------------------
bool SampleListItem::mousePressEvent(Qt::MouseButton button, const QPoint &p) {
  if (button == Qt::LeftButton) {
    mContext->setActiveInstrument(mInstrument);
  } else if (button == Qt::RightButton) {
    QMenu contextMenu(tr("Context menu"), mParent);

    QAction editInstrument(tr("Settings"), mParent);
    connect(&editInstrument, &QAction::triggered, this, &SampleListItem::showInstrumentSettings);
    contextMenu.addAction(&editInstrument);

    QAction exportSample(tr("Export sample"), mParent);
    connect(&exportSample, &QAction::triggered, this, &SampleListItem::exportSample);
    contextMenu.addAction(&exportSample);

    contextMenu.exec(mParent->mapToGlobal(p + mPosition.topLeft()));
  }

  return true;
}

// --------------------------------------------------------------------
void SampleListItem::showInstrumentSettings() {
  InstrumentEditor *editor = new InstrumentEditor(mSampleList);
  editor->setInstrument(mInstrument);
  editor->show();
}

// --------------------------------------------------------------------
void SampleListItem::exportSample() {
  QString file = QFileDialog::getSaveFileName(mParent, tr("Export sample"), mInstrument->id() + ".wav", tr("WAV files (*.wav)"));
  if (file.isEmpty()) {
    return;
  }

  Sft::WavSampleWriter writer(mInstrument->sample());
  if (!writer.save(file)) {
    QMessageBox::critical(mParent, tr("Export sample"), tr("Could not export sample"));
  }
}

// --------------------------------------------------------------------
bool SampleListItem::mouseDoubleClickEvent(Qt::MouseButton button, const QPoint &) {
  if (button == Qt::LeftButton) {
    mContext->editor()->insertAtCursor("@\"" + mInstrument->id() + "\"");
  }

  return true;
}