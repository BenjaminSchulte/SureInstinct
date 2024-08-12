#pragma once

#include <QWidget>

class QLineEdit;

class ScriptSelectWidget : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  ScriptSelectWidget(QWidget *parent = nullptr);

  //! Returns the current script as string
  QString scriptId() const;

signals:
  void scriptIdChanged(const QString &);

public slots:
  //! Sets the script from a text
  void setScriptId(const QString &);

private:
  //! The current line editor
  QLineEdit *mScriptId;
};