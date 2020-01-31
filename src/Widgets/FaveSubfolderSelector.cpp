/** -*- mode: c++ ; c-basic-offset: 2 -*-
 *
 *  @file FaveSubFolderSelector.cpp
 *
 *  Copyright 2017 Sebastien Fourey
 *
 *  This file is part of G'MIC-Qt, a generic plug-in for raster graphics
 *  editors, offering hundreds of filters thanks to the underlying G'MIC
 *  image processing framework.
 *
 *  gmic_qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gmic_qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gmic_qt.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "FaveSubfolderSelector.h"
#include <QDebug>
#include <QTreeWidgetItem>
#include "Common.h"
#include "FilterSelector/FavesModel.h"
#include "Globals.h"
#include "Utils.h"
#include "ui_favesubfolderselector.h"

FaveSubfolderSelector::FaveSubfolderSelector(QWidget * parent) : QDialog(parent), _ui(new Ui::FaveSubfolderSelector())
{
  _ui->setupUi(this);
  _ui->pbCreateFolder->setEnabled(false);
  _ui->pbSelect->setEnabled(false);
  _ui->pbRename->setEnabled(false);
  _ui->pbRemove->setEnabled(false);
  _ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  _ui->treeWidget->setHeaderLabel("Fave folders");
  _ui->treeWidget->setHeaderHidden(true);
  _rootItem = new QTreeWidgetItem(QStringList() << "Faves");
  QFont font = _rootItem->font(0);
  font.setBold(true);
  _rootItem->setFont(0, font);
  _ui->treeWidget->addTopLevelItem(_rootItem);

  setWindowTitle(tr("Select fave subfolder..."));

  connect(_ui->pbCreateFolder, SIGNAL(clicked(bool)), this, SLOT(onCreateFolder()));
  connect(_ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
  connect(_ui->pbCancel, SIGNAL(clicked(bool)), this, SLOT(onCancel()));
  connect(_ui->pbSelect, SIGNAL(clicked(bool)), this, SLOT(onSelect()));
  connect(_ui->pbRename, SIGNAL(clicked(bool)), this, SLOT(onRenameFolder()));
  connect(_ui->pbRemove, SIGNAL(clicked(bool)), this, SLOT(onRemoveFolder()));
  connect(_ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));

  _ui->pbRename->setToolTip(tr("Only newly created folders may be renamed here."));
  _ui->pbRemove->setToolTip(tr("Only newly created folders may be removed here."));
}

FaveSubfolderSelector::~FaveSubfolderSelector() {}

void FaveSubfolderSelector::setFavesModel(const FavesModel & model)
{
  _faveModel = &model;
  QList<QString> paths = model.paths();
  SHOW(paths);
  for (const QString & path : paths) {
    createPath(path.split(FAVE_PATH_SEPATATOR, QString::SkipEmptyParts));
  }
  _ui->treeWidget->expandAll();
  _ui->treeWidget->sortItems(0, Qt::AscendingOrder);
  if (_rootItem->childCount() == 0) {
    _rootItem->setSelected(true);
    _ui->treeWidget->setCurrentItem(_rootItem);
    _ui->pbCreateFolder->setEnabled(true);
    _ui->pbSelect->setEnabled(true);
  }
}

void FaveSubfolderSelector::createPath(QStringList path)
{
  TSHOW(path);
  QTreeWidgetItem * parent = _ui->treeWidget->invisibleRootItem()->child(0);
  while (!path.isEmpty()) {
    int count = parent->childCount();
    QTreeWidgetItem * subfolder = nullptr;
    for (int i = 0; i < count && !subfolder; ++i) {
      QTreeWidgetItem * child = parent->child(i);
      if (child->text(0) == path.front()) {
        subfolder = child;
      }
    }
    if (subfolder) {
      parent = subfolder;
      path.pop_front();
    } else {
      subfolder = new QTreeWidgetItem(QStringList() << path.front());
      path.pop_front();
      parent->addChild(subfolder);
      parent = subfolder;
    }
  }
}

const QList<QString> FaveSubfolderSelector::createdPaths() const
{
  return _createdPaths;
}

QString FaveSubfolderSelector::selectedPath() const
{
  return _selectedPath;
}

void FaveSubfolderSelector::onCreateFolder()
{
  auto items = _ui->treeWidget->selectedItems();
  if (items.size() != 1) {
    return;
  }
  QTreeWidgetItem * parent = items.front();
  _namesToAvoid = childTexts(parent);
  QString name = FAVE_NEW_FOLDER_TEXT;
  GmicQt::makeUniqueName(name, _namesToAvoid);
  QTreeWidgetItem * newItem = new QTreeWidgetItem(QStringList() << name);
  newItem->setData(0, NewFolderRole, QVariant(true));
  parent->addChild(newItem);

  setItemEditable(newItem, true);
  _ui->treeWidget->expandItem(parent);
  _editedItem = newItem;
  _ui->pbSelect->setEnabled(false);
  _ui->treeWidget->editItem(newItem);
}

void FaveSubfolderSelector::onRenameFolder()
{
  auto items = _ui->treeWidget->selectedItems();
  if (items.size() != 1) {
    return;
  }
  QTreeWidgetItem * item = items.front();
  _namesToAvoid = childTexts(item->parent());
  _namesToAvoid.removeAll(item->text(0));
  setItemEditable(item, true);
  _editedItem = item;
  _ui->pbSelect->setEnabled(false);
  _ui->treeWidget->editItem(item, 0);
}

void FaveSubfolderSelector::onRemoveFolder()
{
  auto items = _ui->treeWidget->selectedItems();
  if (items.size() != 1) {
    return;
  }
  QTreeWidgetItem * item = items.front();
  _createdPaths.removeAll(itemPathString(item));
  item->parent()->removeChild(item);
}

void FaveSubfolderSelector::onSelectionChanged()
{
  QList<QTreeWidgetItem *> items = _ui->treeWidget->selectedItems();
  _ui->pbCreateFolder->setEnabled(items.size() == 1);
  _ui->pbSelect->setEnabled(items.size() == 1);
  if (items.size() == 1) {
    QTreeWidgetItem * item = items.front();
    const bool isNew = item->data(0, NewFolderRole).toBool();
    _ui->pbRename->setEnabled(isNew);
    _ui->pbRemove->setEnabled(isNew);
  } else {
    _ui->pbRename->setEnabled(false);
    _ui->pbRemove->setEnabled(false);
  }
}

void FaveSubfolderSelector::onSelect()
{
  auto items = _ui->treeWidget->selectedItems();
  if (items.size() == 1) {
    _selectedPath = itemPathString(items.front());
  }
  accept();
}

void FaveSubfolderSelector::onCancel()
{
  _createdPaths.clear();
  reject();
}

void FaveSubfolderSelector::itemChanged(QTreeWidgetItem * item, int /* column */)
{
  if (_editedItem && _editedItem == item) {
    _editedItem = nullptr;
    if (_namesToAvoid.contains(item->text(0))) {
      QString uniqueName = item->text(0);
      GmicQt::makeUniqueName(uniqueName, _namesToAvoid);
      item->setText(0, uniqueName);
    }
    setItemEditable(item, false);
    _ui->treeWidget->sortItems(0, Qt::AscendingOrder);
    _ui->treeWidget->clearSelection();
    item->setSelected(true);
    _createdPaths.push_back(itemPathString(item));
  }
  onSelectionChanged();
}

QStringList FaveSubfolderSelector::itemPath(QTreeWidgetItem * item)
{
  QStringList result;
  while (item && item != _ui->treeWidget->invisibleRootItem() && item != _rootItem) {
    result.push_front(item->text(0));
    item = item->parent();
  }
  return result;
}

QString FaveSubfolderSelector::itemPathString(QTreeWidgetItem * item)
{
  return itemPath(item).join(FAVE_PATH_SEPATATOR);
}

QStringList FaveSubfolderSelector::childTexts(QTreeWidgetItem * item) const
{
  QStringList result;
  if (!item) {
    return result;
  }
  for (int i = 0; i < item->childCount(); ++i) {
    result.push_back(item->child(i)->text(0));
  }
  return result;
}

void FaveSubfolderSelector::setItemEditable(QTreeWidgetItem * item, bool on) const
{
  if (!item) {
    return;
  }
  auto flags = item->flags();
  flags.setFlag(Qt::ItemIsEditable, on);
  item->setFlags(flags);
}
