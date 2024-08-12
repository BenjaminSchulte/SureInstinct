#include <aoba/csv/CsvWriter.hpp>

using namespace Aoba; 

// -----------------------------------------------------------------------------
void CsvWriter::nextRow() {
  mCsv += mRow.join(mSeparator) + mNewLine;
  mRow.clear();
}

// -----------------------------------------------------------------------------
QString CsvWriter::toString() const {
  return mCsv + mNewLine + mRow.join(mSeparator);
}

// -----------------------------------------------------------------------------
void CsvWriter::add(const QString &item) {
  if (item.indexOf(mNewLine) == -1 && item.indexOf(mSeparator) == -1 && item.indexOf(mQuote) == -1) {
    mRow.push_back(item);
    return;
  }

  QString newText = item;
  newText = newText.replace(mQuote, QString(mQuote) + mQuote);
  mRow.push_back(mQuote + newText + mQuote);
}
