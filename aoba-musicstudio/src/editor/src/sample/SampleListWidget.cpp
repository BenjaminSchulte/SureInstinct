#include <QDebug>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QMessageBox>
#include <editor/sample/SampleFileDialog.hpp>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderButton.hpp>
#include <editor/sample/SampleListItem.hpp>
#include <editor/sample/SampleListWidget.hpp>
#include <editor/sample/InstrumentEditor.hpp>
#include <editor/project/Project.hpp>
#include <editor/project/ProjectContext.hpp>
#include <sft/song/Song.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/instrument/YamlInstrumentLoader.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
SampleListWidget::SampleListWidget(QWidget *parent)
  : RenderScrollableWidget(parent)
  , mProject(nullptr)
  , mCurrentInstrument(nullptr)
{
  setMinimumSize(QSize(200, 200));

  recreateContent();
}

// -----------------------------------------------------------------------------
void SampleListWidget::recreateContent() {
  if (!isRenderable()) { return; }

  removeAllRenderables();

  int halfHeight = mConfiguration->listItemHeight() / 2;
  int w = size().width();
  mAddButton = new RenderButton(this, RenderTheme::IconAdd, QRect(w - halfHeight - 10, halfHeight - 10, 20, 20));
  connect(mAddButton, &RenderButton::clicked, this, &SampleListWidget::addInstrumentDialog);

  int y = mConfiguration->listItemHeight() - mScrollPosition.y();
  for (const Sft::InstrumentPtr &instrument : mProject->song()->instruments().all()) {
    SampleListItem *item = new SampleListItem(this, mContext, instrument, QRect(0, y, w, mConfiguration->listItemHeight()));
    mRenderables.push_back(item);
    y += mConfiguration->listItemHeight();
  }

  mRenderables.push_back(mAddButton);
}

// -----------------------------------------------------------------------------
QSize SampleListWidget::scrollableSize() const {
  QSize area(size());
  return QSize(area.width(), mConfiguration->listItemHeight() * (mProject->song()->instruments().count() + 1));
}

// -----------------------------------------------------------------------------
void SampleListWidget::doScroll(int x, int y) {
  RenderScrollableWidget::doScroll(x, y);
  int halfHeight = mConfiguration->listItemHeight() / 2;
  int w = size().width();
  mAddButton->setPosition(QRect(w - halfHeight - 10, halfHeight - 10, 20, 20));
  update();
}

// -----------------------------------------------------------------------------
bool SampleListWidget::isRenderable() const {
  return RenderScrollableWidget::isRenderable() && !!mProject;
}

// -----------------------------------------------------------------------------
void SampleListWidget::setProject(ProjectContext *context) {
  mProject = context->project();
  mContext = context;
  recreateContent();
  update();
}

// -----------------------------------------------------------------------------
void SampleListWidget::addInstrumentDialog() {
  SampleFileDialog dialog(this, mProject->songPath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  if (!dialog.exec() || !dialog.selectedFiles().length()) {
    return;
  }

  Sft::InstrumentPtr lastInstrument;
  for (const QString &fileName : dialog.selectedFiles()) {
    QFileInfo fileInfo(fileName);

    if (fileInfo.suffix().toLower() == "yml") {
      Sft::YamlInstrumentLoader loader(mProject->song());
      Sft::InstrumentPtr instr = loader.load(fileName);
      instr->setId(fileInfo.baseName());

      if (!instr) {
        QMessageBox::critical(this, tr("Failed to load instrument"), tr("Could not load the instrument. Maybe the file is corrupted or unsupported."));
        continue;
      }

      lastInstrument = instr;
    } else {
      Sft::Sample *sample = mProject->song()->samples().create(fileInfo.baseName());
      if (!sample->load(fileName)) {
        mProject->song()->samples().remove(sample);
        QMessageBox::critical(this, tr("Failed to load sample"), tr("Could not load the instrument. Maybe the file is corrupted or unsupported."));
        continue;
      }

      lastInstrument = mProject->song()->instruments().create(sample);
    }

    if (lastInstrument) {
      qDebug() << "Loaded instrument with ID" << lastInstrument->id();
    }
  }

  setCurrentInstrument(lastInstrument);
  recreateContent();
  update();

  InstrumentEditor *editor = new InstrumentEditor(this);
  editor->setInstrument(lastInstrument);
  editor->show();
}

// -----------------------------------------------------------------------------
void SampleListWidget::setCurrentInstrument(const Sft::InstrumentPtr &instrument) {
  if (mCurrentInstrument == instrument) {
    return;
  }

  mCurrentInstrument = instrument;
  mContext->setActiveInstrument(instrument);
  emit instrumentChanged(instrument);
  update();
}

// -----------------------------------------------------------------------------
void SampleListWidget::drawGL(QPainter *painter, QOpenGLFunctions *) {
  QSize area(size());
  RenderHelper h(painter, mConfiguration, area);

  h.clear();

  drawAllRenderables(h);

  h.header(QRect(-1, 0, area.width() + 2, mConfiguration->listItemHeight()), tr("Instruments"));
  mAddButton->draw(h);
}
