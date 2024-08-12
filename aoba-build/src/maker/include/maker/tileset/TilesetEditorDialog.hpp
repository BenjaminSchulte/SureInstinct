#pragma once

#include <QDialog>

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class TilesetEditor;

class TilesetEditorDialog : public QDialog {
  Q_OBJECT

public:
  //! Constructor
  TilesetEditorDialog(const MakerProjectPtr &project, QWidget *parent);

private slots:
  void saveAndClose();

private:
  //! The editor
  TilesetEditor *mEditor;
};