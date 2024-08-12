#include <aoba/log/Log.hpp>
#include <QDropEvent>
#include <maker/project/MakerProject.hpp>
#include <maker/project/ProjectTreeWidget.hpp>
#include <maker/GlobalContext.hpp>

// -----------------------------------------------------------------------------
ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
  : QTreeWidget(parent)
{
	invisibleRootItem()->setFlags(invisibleRootItem()->flags() ^ Qt::ItemIsDropEnabled);
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::regenerate() {
	hideColumn(1);

	clear();
  mRoot = nullptr;
	mProjectTreeItems.clear();

	const auto &project = context().project();
	if (!project) {
		return;
	}

	mRoot = new QTreeWidgetItem();
	mRoot->setData(1, Qt::DisplayRole, "");
	mRoot->setData(0, Qt::DisplayRole, project->title());
	mRoot->setIcon(0, QIcon(":/icons/share/old_folder.png"));
	mRoot->setFlags(mRoot->flags() ^ Qt::ItemIsDragEnabled);

	rebuildMapTree(mRoot, project->assetTree().root().children());
	addTopLevelItem(mRoot);
	mRoot->setExpanded(true);
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::rebuildMapTree(QTreeWidgetItem *parent, const QVector<MakerAssetTreeNode> &nodes) {
	for (const auto &node : nodes) {
		QString id = node.type() + "/" + node.id();

		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setData(1, Qt::DisplayRole, id);
		item->setData(0, Qt::DisplayRole, node.name());
		item->setIcon(0, QIcon(":/icons/share/old_map.png"));
		parent->addChild(item);
		item->setExpanded(true);
		mProjectTreeItems.insert(id, item);

		rebuildMapTree(item, node.children());
  }
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::rename(const QString &id, const QString &name) {
	if (mProjectTreeItems.contains(id)) {
		mProjectTreeItems[id]->setText(0, name);
	}
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::dropEvent(QDropEvent *event) {
  QTreeWidget::dropEvent(event);

  recreateProjectAssetTree();
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::recreateProjectAssetTree() const {
  MakerProjectPtr project = context().project();
  if (!project) {
    return;
  }

  MakerAssetTree &tree = project->assetTree();
  tree.clear();
  recreateProjectAssetTree(mRoot, tree.root());

  if (!tree.saveToProject(project->gameProject())) {
    Aoba::log::warn("Unable to save asset tree");
  } 
}

// -----------------------------------------------------------------------------
void ProjectTreeWidget::recreateProjectAssetTree(QTreeWidgetItem *from, MakerAssetTreeNode &to) const {
  int numChildren = from->childCount();

  for (int i=0; i<numChildren; i++) {
    QTreeWidgetItem *child = from->child(i);

    QStringList parts = child->text(1).split('/');

    to.addChild(MakerAssetTreeNode(child->text(0), parts[0], parts[1]));
    recreateProjectAssetTree(child, to.children().last());
  }
}