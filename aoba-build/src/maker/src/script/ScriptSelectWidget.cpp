#include <QVBoxLayout>
#include <QLineEdit>
#include <maker/script/ScriptSelectWidget.hpp>

// -----------------------------------------------------------------------------
ScriptSelectWidget::ScriptSelectWidget(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *box = new QVBoxLayout(this);
  box->setMargin(0);

  mScriptId = new QLineEdit(this);

  box->addWidget(mScriptId);
  setLayout(box);

  connect(mScriptId, &QLineEdit::textChanged, this, &ScriptSelectWidget::scriptIdChanged);
}

// -----------------------------------------------------------------------------
QString ScriptSelectWidget::scriptId() const {
  return mScriptId->text();
}

// -----------------------------------------------------------------------------
void ScriptSelectWidget::setScriptId(const QString &text) {
  if (text == mScriptId->text()) {
    return;
  }

  mScriptId->setText(text);
  emit scriptIdChanged(text);
}