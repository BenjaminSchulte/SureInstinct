#pragma once

#include <QRegExpValidator>

class IdValidator : public QRegExpValidator {
public:
  //! Constructor
  IdValidator(QObject *parent)
    : QRegExpValidator(QRegExp("[a-z0-9_]*"), parent)
  {}
};