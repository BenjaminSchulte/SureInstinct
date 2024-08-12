#include <QJsonDocument>
#include <QJsonObject>
#include <aoba/json/JsonParser.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
QString JsonParser::parseString(const QString &data, bool *ok) { 
  QString fullJson = QString("{\"v\":") + data + "}";

  QJsonDocument doc(QJsonDocument::fromJson(fullJson.toUtf8()));
  QJsonObject obj(doc.object());

  if (obj.contains("v") && obj["v"].isString()) {
    if (ok != nullptr) {
      *ok = true;
    }

    return obj["v"].toString();
  } else {
    if (ok != nullptr) {
      *ok = false;
    }

    return "";
  }
}
