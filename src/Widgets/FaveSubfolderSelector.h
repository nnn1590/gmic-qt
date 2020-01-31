/** -*- mode: c++ ; c-basic-offset: 2 -*-
 *
 *  @file FaveSubFolderSelector.h
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
#ifndef GMIC_QT_FAVESUBFOLDERSELECTOR_H
#define GMIC_QT_FAVESUBFOLDERSELECTOR_H

#include <QDialog>
#include <QStringList>

class QTreeWidgetItem;
class FavesModel;

namespace Ui
{
class FaveSubfolderSelector;
} // namespace Ui

class FaveSubfolderSelector : public QDialog {
  Q_OBJECT
public:
  FaveSubfolderSelector(QWidget * parent = nullptr);
  ~FaveSubfolderSelector() override;
  void setFavesModel(const FavesModel &);
  void createPath(QStringList path);
  const QList<QString> createdPaths() const;
  QString selectedPath() const;

public slots:
  void onCreateFolder();
  void onRenameFolder();
  void onRemoveFolder();
  void onSelectionChanged();

private slots:

  void onSelect();
  void onCancel();
  void itemChanged(QTreeWidgetItem * item, int column);

private:
  QStringList itemPath(QTreeWidgetItem * item);
  QString itemPathString(QTreeWidgetItem * item);
  QStringList childTexts(QTreeWidgetItem * item) const;
  void setItemEditable(QTreeWidgetItem * item, bool on) const;

  Ui::FaveSubfolderSelector * _ui;
  const FavesModel * _faveModel;
  QList<QString> _createdPaths;
  QString _selectedPath;
  QTreeWidgetItem * _rootItem;
  QTreeWidgetItem * _editedItem = nullptr;
  QStringList _namesToAvoid;

  static const int NewFolderRole = Qt::UserRole + 1;
};

#endif // GMIC_QT_FAVESUBFOLDERSELECTOR_H
