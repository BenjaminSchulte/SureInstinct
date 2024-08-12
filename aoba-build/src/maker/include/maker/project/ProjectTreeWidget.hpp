#pragma once

#include <QTreeWidget>

struct MakerAssetTreeNode;

class ProjectTreeWidget : public QTreeWidget {
public:
  //! Constructor
  ProjectTreeWidget(QWidget *parent = nullptr);

  //! Regenerates the tree 
  void regenerate();

  //! Renames an item
  void rename(const QString &id, const QString &name);

protected:
  //! Drop event
  void dropEvent(QDropEvent *event);

private:
	//! Rebuilds the map tree
	void rebuildMapTree(QTreeWidgetItem *parent, const QVector<MakerAssetTreeNode> &nodes);

  //! Recreates the project asset tree
  void recreateProjectAssetTree() const;

  //! Recreates the project asset tree
  void recreateProjectAssetTree(QTreeWidgetItem *from, MakerAssetTreeNode &to) const;

  //! The root item
  QTreeWidgetItem *mRoot = nullptr;

	//! Project tree map
	QMap<QString, QTreeWidgetItem*> mProjectTreeItems;
};
