/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <QCursor>
#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "mousegotopixeleditor.h"
#include "coveragelayermodel.h"
#include "raster.h"
#include "ilwiscontext.h"


using namespace Ilwis;
using namespace Ui;

MouseGotoPixelEditor::MouseGotoPixelEditor()
{

}

MouseGotoPixelEditor::MouseGotoPixelEditor(VisualAttribute *p) :
	VisualPropertyEditor(p, "mousegotopixeleditor", TR("Go to"), QUrl("MouseGoToPixelEditor.qml"))
{

}

bool MouseGotoPixelEditor::canUse(const IIlwisObject& obj, const QString& name) const
{
	if (!obj.isValid())
		return false;
	if (!hasType(obj->ilwisType(), itRASTER))
		return false;
	return name == VisualAttribute::LAYER_ONLY;

}

VisualPropertyEditor *MouseGotoPixelEditor::create(VisualAttribute *p)
{
	return new MouseGotoPixelEditor(p);
}

int MouseGotoPixelEditor::row() const {
	return _row;
}
void MouseGotoPixelEditor::row(int tr) {
	_row = tr;
}
int MouseGotoPixelEditor::column() const {
	return _column;
}
void MouseGotoPixelEditor::column(int tr) {
	_column = tr;
}

QVariantMap MouseGotoPixelEditor::screenPixel(int column, int row) const {
	QVariantMap mp;
	IRasterCoverage raster = static_cast<CoverageLayerModel *>(vpmodel()->layer())->coverage().as<RasterCoverage>();
	if (raster.isValid()) {
		auto crd = raster->georeference()->pixel2Coord(Pixel(column, row));
		auto pix = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->coord2Pixel(crd);
		mp["x"] = pix.x;
		mp["y"] = pix.y;
		
	}
	return mp;
}
void MouseGotoPixelEditor::moveCursor(int column, int row) {
		QCursor cursor;
		cursor.setPos(column, row);
}

