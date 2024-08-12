#pragma once

#include <QDialog>

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class SpriteEditor;

class SpriteEditorDialog : public QDialog {
  Q_OBJECT

public:
  //! Constructor
  SpriteEditorDialog(const MakerProjectPtr &project, QWidget *parent);

private slots:
  void saveAndClose();

private:
  //! The editor
  SpriteEditor *mEditor;
};