#pragma once

#include <QTextStream>
#include <QStringList>

namespace Aoba {

class CsvReader {
public:
  //! Constructor
  CsvReader(QTextStream &stream, bool header=false, char sep=',', char quote='\"', char nl='\n')
    : mStream(stream)
    , mHeader(header)
    , mSeparator(sep)
    , mQuote(quote)
    , mNewLine(nl)
  {}

  //! New row
  bool nextRow();
  
  //! Returns whether the row is empty
  bool isEmptyRow() const { return mRow.size() == 0 || (mRow.size() == 1 && mRow[0].isEmpty()); }

  //! Returns a cel
  inline int numColumns() const { return mRow.size(); }

  //! Returns a cel
  inline const QString &at(int index) const { return (index < 0 || index >= numColumns()) ? emptyString : mRow[index]; }

  //! Returns a cel
  inline const QString &get(const QString &name) const { return mHeaderToIndex.contains(name) ? at(mHeaderToIndex[name]) : emptyString; }

  //! Returns the row
  inline const QStringList &row() const { return mRow; }

  //! Returns the row
  const QStringList &header();

private:
  //! Empty string
  QString emptyString;

  //! The text stream
  QTextStream &mStream;

  //! Header
  bool mHeader;

  //! Whether the header has been read
  bool mHeaderRead = false;

  //! Separator
  char mSeparator;

  //! Quote char
  char mQuote;

  //! New line
  char mNewLine;

  //! The current row
  QStringList mRow;

  //! The header row
  QStringList mHeaderRow;

  //! Header -> index map
  QMap<QString, int> mHeaderToIndex;
};

}