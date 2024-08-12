#include <aoba/log/Log.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameAssetTypes.hpp>
#include <maker/project/MakerAssetTree.hpp>

// -----------------------------------------------------------------------------
void MakerAssetTree::addNodes(Maker::GameProject *project, MakerAssetTreeNode &target, const QVector<Maker::GameConfigNode> &nodes) {
  for (const auto &node : nodes) {
    QStringList keys = node.keys();
    if (keys.isEmpty()) {
      continue;
    }

    QString type = keys[0];
    if (type == "children") {
      if (keys.size() == 1) {
        Aoba::log::warn("Children-only tree nodes are invalid");
        continue;
      }

      type = keys[1];
    }

    QString id = node[type].asString();

    Maker::GameAssetType<Aoba::AbstractAsset> *typeRecord = project->assetTypes().getAbstract(type);
    if (!typeRecord) {
      Aoba::log::warn("Warning: Unsupported asset type " + type);
      continue;
    }

    Maker::AbstractGameAsset *asset = project->assets().getAsset(type, id);
    if (!asset) {
      Aoba::log::warn("Warning: " + type + " " + id + " not found.");
      continue;
    }

    QString name = asset->aobaAsset()->name();
    target.children().push_back(MakerAssetTreeNode(name, type, id));

    addNodes(project, target.children().last(), node["children"].asList());
  }
}

// -----------------------------------------------------------------------------
void MakerAssetTreeNode::toYaml(YAML::Emitter &node) const {
  node << YAML::BeginMap;
  node << YAML::Key << mType.toStdString() << YAML::Value << mId.toStdString();
  
  if (!mChildren.isEmpty()) {
    node << YAML::Key << "children" << YAML::Value << YAML::BeginSeq;
    for (const auto &child : mChildren) {
      child.toYaml(node);
    }
    node << YAML::EndSeq;
  }

  node << YAML::EndMap;
}

// -----------------------------------------------------------------------------
bool MakerAssetTree::reloadFromProject(Maker::GameProject *project) {
  Maker::GameConfigReader reader(Maker::GameConfigReader::fromFile(project, project->path().absoluteFilePath("editor/assettree.yml")));
  addNodes(project, mRoot, reader["children"].asList());

  return true;
}

// -----------------------------------------------------------------------------
bool MakerAssetTree::saveToProject(Maker::GameProject *project) {
  YAML::Emitter root;
  root << YAML::BeginMap;
  root << YAML::Key << "children" << YAML::BeginSeq;
  for (const auto &child : mRoot.children()) {
    child.toYaml(root);
  }
  root << YAML::EndMap;

  QFile file(project->path().absoluteFilePath("editor/assettree.yml"));
  if (!file.open(QIODevice::WriteOnly)) {
    Aoba::log::warn("Unable to open assettree.yml for writing");
    return false;
  }

  file.write(root.c_str(), root.size());
  return true;
}
