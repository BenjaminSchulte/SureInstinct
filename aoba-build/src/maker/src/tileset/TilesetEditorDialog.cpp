#include <QApplication>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <maker/tileset/TilesetEditorDialog.hpp>
#include <maker/tileset/TilesetEditor.hpp>
#include <maker/project/MakerProject.hpp>

// -----------------------------------------------------------------------------
TilesetEditorDialog::TilesetEditorDialog(const MakerProjectPtr &project, QWidget *parent)
  : QDialog(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0);
  mEditor = new TilesetEditor(project, this);

  QWidget *buttonsWidget = new QWidget(this);
  QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

  QDialogButtonBox *buttons = new QDialogButtonBox(buttonsWidget);
  buttons->addButton(QDialogButtonBox::Save);
  buttons->addButton(QDialogButtonBox::Cancel);

  buttonsLayout->addWidget(buttons);
  buttonsWidget->setLayout(buttonsLayout);

  connect(buttons, &QDialogButtonBox::rejected, this, &TilesetEditorDialog::close);
  connect(buttons, &QDialogButtonBox::accepted, this, &TilesetEditorDialog::saveAndClose);

  layout->addWidget(mEditor);
  layout->addWidget(buttonsWidget);
  setLayout(layout);

  resize(QSize(1000, 650));
  setWindowTitle(mEditor->windowTitle());
}

// -----------------------------------------------------------------------------
void TilesetEditorDialog::saveAndClose() {
  if (!mEditor->save()) {
    QMessageBox::critical(this, tr("Save tilesets"), tr("Not all tilesets could be saved"));
    return;
  }

  close();
}
