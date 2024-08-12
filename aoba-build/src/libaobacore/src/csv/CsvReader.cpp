#include <aoba/csv/CsvReader.hpp>

using namespace Aoba; 

// -----------------------------------------------------------------------------
const QStringList &CsvReader::header() {
  if (!mHeader || mHeaderRead) {
    return mHeaderRow;
  }

  mHeaderRead = true;
  if (!nextRow()) {
    return mHeaderRow;
  }

  mHeaderRow = mRow;
  int headerIndex = 0;
  for (QString id : mHeaderRow) {
    mHeaderToIndex.insert(id, headerIndex);

    headerIndex++;
  }

  return mHeaderRow;
}

// -----------------------------------------------------------------------------
bool CsvReader::nextRow() {
  if (mHeader && !mHeaderRead) {
    header();
  }

  static const int delta[][5] = {
    //  ,    "   \n    ?  eof
    {   1,   2,  -1,   0,  -1  }, // 0: parsing (store char)
    {   1,   2,  -1,   0,  -1  }, // 1: parsing (store column)
    {   3,   4,   3,   3,  -2  }, // 2: quote entered (no-op)
    {   3,   4,   3,   3,  -2  }, // 3: parsing inside quotes (store char)
    {   1,   3,  -1,   0,  -1  }, // 4: quote exited (no-op)
    // -1: end of row, store column, success
    // -2: eof inside quotes
  };

  mRow.clear();

  if (mStream.atEnd()) {
    return false;
  }

  int state = 0, t;
  char ch;
  QString cell;

  while (state >= 0) {
    if (mStream.atEnd()) {
      t = 4;
    } else {
      mStream >> ch;
      if (ch == mSeparator) t = 0;
      else if (ch == mQuote) t = 1;
      else if (ch == mNewLine) t = 2;
      else t = 3;
    }

    state = delta[state][t];

    switch (state) {
    case 0:
    case 3:
      cell += ch;
      break;
    case -1:
    case 1:
      mRow.append(cell);
      cell = "";
      break;
    }
  }

  if (state == -2) {
    return false;
  }

  return true;
}
