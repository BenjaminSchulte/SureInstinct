#pragma once

#include <QString>
#include <QRect>
#include <QPoint>
#include <QVariant>
#include <QSize>
#include <yaml-cpp/yaml.h>
#include <aoba/yaml/YamlTools.hpp>

namespace Maker {
class GameProject;

class GameConfigNode {
public:
  //! Empty Constructor
  GameConfigNode() {}

  //! Constructor
  GameConfigNode(GameProject *project, const YAML::Node &node)
    : mProject(project)
    , mNode(node)
  {}

  //! Returns all keys
  QStringList keys() const;

  //! Returns another game config node
  GameConfigNode operator[](const QString &id) const;

  //! Returns another game config node
  inline GameConfigNode get(const QString &id) const { return operator[](id); }

  //! Returns an asset file name (type/id)
  QString asAssetFileName(const QString &type, const QString &file) const;

  //! Returns an asset file name (type/id)
  QString asAssetPath(const QString &type) const;

  //! Returns an asset file name (type/id.ext)
  QString asDirectAssetFileName(const QString &type, const QString &ext) const;

  //! Returns this as variant
  QVariant asVariant() const;

  //! Is the type valid
  template<typename T>
  bool is() const {
    try {
      mNode.as<T>();
      return true;
    } catch(YAML::BadConversion&) {
      return false;
    } catch(YAML::InvalidNode&) {
      return false;
    }
  }

  //! Is this valid?
  inline bool isValid() const { return mNode.IsDefined(); }

  //! Is this a string?
  inline bool isInt() const { return mNode.IsScalar(); }

  //! Is this a string?
  bool isString() const { return is<std::string>(); }

  //! Is this a string?
  bool isBoolean() const { return is<bool>(); }

  //! Is this a map?
  inline bool isMap() const { return mNode.IsMap(); }

  //! Returns a string
  QString asString(const QString &fallback=QString()) const;

  //! Returns an integer
  bool asBool(bool fallback=false) const;

  //! Returns an integer
  int asInt(int fallback=0) const;

  //! Returns an integer
  double asDouble(double fallback=0) const;

  //! Returns a size
  inline QPoint asQPoint() const {
    return QPoint(
      get("x").asDouble(),
      get("y").asDouble()
    );
  }

  //! Returns a size
  inline QSize asQSize() const {
    return QSize(
      get("width").asDouble(),
      get("height").asDouble()
    );
  }

  //! Returns a rectangle
  inline QRect asQRect() const {
    return QRect(
      asQPoint(),
      asQSize()
    );
  }

  //! Returns a list
  QVector<GameConfigNode> asList() const;

  //! Returns the internal node
  inline const YAML::Node &yamlNode() const { return mNode; }

  //! Returns the internal node
  inline YAML::Node &yamlNode() { return mNode; }

protected:
  //! The project
  GameProject *mProject;

  //! The current node
  YAML::Node mNode;
};

class GameConfigReader : public GameConfigNode {
public:
  //! Constructor
  GameConfigReader(GameProject *project, bool valid, const YAML::Node &node);

  //! Creates a new reader
  static GameConfigReader fromFile(GameProject *project, const QString &fileName);

  //! Is the reader valid?
  inline bool isValid() const { return mValid; }

private:
  //! Whether the file is valid
  bool mValid;
};

}