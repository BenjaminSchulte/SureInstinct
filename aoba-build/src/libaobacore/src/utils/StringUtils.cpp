#include <QRegExp>
#include <aoba/utils/StringUtils.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString StringUtils::escape(const QString &identifier) {
  QRegExp invalidChar("[^a-zA-Z0-9]");
  QString newIdentifier = identifier;
  int pos = 0;

  while ((pos = newIdentifier.indexOf(invalidChar, pos)) != -1) {
    QChar c(invalidChar.cap(0).front());
    
    if (c == '_') {
      //newIdentifier.insert(pos, '_');
      pos += 1;
    } else {
      QString newPart;
      newPart.setNum(c.toLatin1(), 16);
      newPart = newPart.leftJustified(2, '0').prepend("_");

      newIdentifier.remove(pos, 1);
      newIdentifier.insert(pos, newPart);
      pos += newPart.size();
    }
  }

  return newIdentifier;
}
