#pragma once

#include <QTextStream>
#include <QStringList>

namespace Aoba {

class CsvWriter {
public:
  //! Constructor
  CsvWriter(char sep=',', char quote='\"', char nl='\n')
    : mSeparator(sep)
    , mQuote(quote)
    , mNewLine(nl)
  {}

  //! Resumes to the next row
  void nextRow();

  //! Returns the text
  QString toString() const;

  //! Adds text
  void add(const QString &);

  //! Adds a text
  inline CsvWriter &operator<<(const QString &text) { add(text); return *this; }

  //! Adds a text
  inline CsvWriter &operator<<(const char *text) { add(text); return *this; }

  //! Adds a text
  inline CsvWriter &operator<<(int num) { add(QString::number(num)); return *this; }

private:
  //! The CSV
  QString mCsv;

  //! The current row
  QStringList mRow;

  //! Separator
  char mSeparator;

  //! Quote char
  char mQuote;

  //! New line
  char mNewLine;
};

}