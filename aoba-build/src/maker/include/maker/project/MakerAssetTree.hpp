#pragma once

#include <QVector>
#include <yaml-cpp/yaml.h>

namespace Maker {
class GameProject;
class GameConfigNode;
}

struct MakerAssetTreeNode {
  //! Constructor
  MakerAssetTreeNode() {}

  //! Constructor
  MakerAssetTreeNode(const QString &name, const QString &type, const QString &id) 
    : mName(name)
    , mType(type)
    , mId(id)
  {}

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the type
  inline const QString &type() const { return mType; }

  //! Returns the id
  inline const QString &id() const { return mId; }

  //! Returns the children
  inline QVector<MakerAssetTreeNode> &children() { return mChildren; }

  //! Returns the children
  inline const QVector<MakerAssetTreeNode> &children() const { return mChildren; }

  //! Adds a child
  void addChild(const MakerAssetTreeNode &node) { mChildren.push_back(node); }

  //! To YAML
  void toYaml(YAML::Emitter &) const;

  //! Clears all children
  inline void removeAllChildren() { mChildren.clear(); }
  
private:
  //! The name
  QString mName;

  //! The asset type
  QString mType;

  //! The asset ID
  QString mId;

  //! List of all children
  QVector<MakerAssetTreeNode> mChildren;
};

class MakerAssetTree {
public:

  //! Reloads the tree from a project
  bool reloadFromProject(Maker::GameProject *project);

  //! Reloads the tree from a project
  bool saveToProject(Maker::GameProject *project);

  //! Returns the root
  inline MakerAssetTreeNode &root() { return mRoot; }

  //! Clears the tree
  inline void clear() { mRoot.removeAllChildren(); }

private:
  //! Adds nodes
  void addNodes(Maker::GameProject *project, MakerAssetTreeNode &node, const QVector<Maker::GameConfigNode> &nodes);

  //! The root node
  MakerAssetTreeNode mRoot;
};
