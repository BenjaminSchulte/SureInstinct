#include <QApplication>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <maker/sprite/SpriteEditorDialog.hpp>
#include <maker/sprite/SpriteEditor.hpp>
#include <maker/project/MakerProject.hpp>

// -----------------------------------------------------------------------------
SpriteEditorDialog::SpriteEditorDialog(const MakerProjectPtr &project, QWidget *parent)
  : QDialog(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0);
  mEditor = new SpriteEditor(project, this);

  QWidget *buttonsWidget = new QWidget(this);
  QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

  QDialogButtonBox *buttons = new QDialogButtonBox(buttonsWidget);
  buttons->addButton(QDialogButtonBox::Save);
  buttons->addButton(QDialogButtonBox::Cancel);

  buttonsLayout->addWidget(buttons);
  buttonsWidget->setLayout(buttonsLayout);

  connect(buttons, &QDialogButtonBox::rejected, this, &SpriteEditorDialog::close);
  connect(buttons, &QDialogButtonBox::accepted, this, &SpriteEditorDialog::saveAndClose);

  layout->addWidget(mEditor);
  layout->addWidget(buttonsWidget);
  setLayout(layout);

  resize(QSize(1000, 650));
  setWindowTitle(mEditor->windowTitle());
}

// -----------------------------------------------------------------------------
void SpriteEditorDialog::saveAndClose() {
  if (!mEditor->save()) {
    QMessageBox::critical(this, tr("Save sprites"), tr("Not all sprites could be saved"));
    return;
  }

  close();
}
