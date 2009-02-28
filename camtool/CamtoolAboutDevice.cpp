/*
 * Copyright (c) 2009 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  <qapplication.h>
# include  "CamtoolAboutDevice.h"
# include  <QFileDialog>
# include  <fstream>
# include  <iomanip>

using namespace std;

CamtoolAboutDevice::CamtoolAboutDevice(QWidget*parent)
: QDialog(parent)
{
      ui.setupUi(this);

      connect(ui.buttonBox,
	      SIGNAL(clicked(QAbstractButton*)),
	      SLOT(button_box_slot_(QAbstractButton*)));
}

CamtoolAboutDevice::~CamtoolAboutDevice()
{
}

void CamtoolAboutDevice::set_devicetree(QTreeWidgetItem*tree)
{
      ui.about_device_tree->clear();
      ui.about_device_tree->addTopLevelItem(tree);
}

void CamtoolAboutDevice::button_box_slot_(QAbstractButton*button)
{
      if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Save)
	    do_save_();
}

static void dump_tree(ostream&file, QTreeWidgetItem*root, int indent)
{
      file << setw(indent) << "";
      for (int idx = 0 ; idx < root->columnCount() ; idx += 1) {
	    QString text = root->text(idx);
	    if (idx > 0)
		  file << " / ";
	    if (text.isEmpty())
		  file << "-";
	    else
		  file << text.toStdString();
      }
      file << endl;

      for (int idx = 0 ; idx < root->childCount() ; idx += 1)
	    dump_tree(file, root->child(idx), indent + 4);
}

void CamtoolAboutDevice::do_save_()
{
      QString path = QFileDialog::getSaveFileName(0, tr("Where to write Camera Details"));
      if (path.isEmpty())
	    return;

      ofstream file (path.toAscii());
      QTreeWidgetItem*root = ui.about_device_tree->invisibleRootItem();

      if (root == 0) {
	    file << "*** No description? ***" << endl;
	    return;
      }

      dump_tree(file, root, 0);
}
