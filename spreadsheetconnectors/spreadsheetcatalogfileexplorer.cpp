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

#include <QSqlError>
#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "dataformat.h"
#include "mastercatalog.h"
#include "spreadsheetcatalogfileexplorer.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;


REGISTER_CATALOGEXPLORER(SpreadSheetCatalogFileExplorer)

Ilwis::CatalogExplorer *SpreadSheetCatalogFileExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new SpreadSheetCatalogFileExplorer(resource, options);
}

SpreadSheetCatalogFileExplorer::SpreadSheetCatalogFileExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
}
std::vector<Ilwis::Resource> SpreadSheetCatalogFileExplorer::loadItems(const IOOptions &)
{
    QFileInfo containerInf = source().toLocalFile();
    if ( containerInf.isDir()){
        ERROR2(ERR_COULD_NOT_CONVERT_2,"container", "file" );
        return std::vector<Ilwis::Resource>();
    }
    std::vector<Ilwis::Resource> result;
    QString query = QString("Select itemid from mastercatalog where container='%1'").arg(source().url().toString());
    InternalDatabaseConnection db;
    if (db.exec(query)) {
        while ( db.next()){
            quint64 id = db.value(0).toULongLong();
            Resource resource = mastercatalog()->id2Resource(id);
            if ( resource.isValid())
                result.push_back(resource);

        }
    }
    return result;
}

bool SpreadSheetCatalogFileExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    QFileInfo fileInfo(resource.url(true).toLocalFile());
    if ( fileInfo.isFile()) { // must be a file
        if ( fileInfo.suffix().toLower() == "xls" || fileInfo.suffix().toLower() == "ods" || fileInfo.suffix() == "xslx")
        return true;
    }
    return false;
}

QString SpreadSheetCatalogFileExplorer::provider() const
{
    return "spreadsheet";
}

QFileInfo SpreadSheetCatalogFileExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

CatalogExplorer::ExplorerType SpreadSheetCatalogFileExplorer::explorerType() const
{
    return etIMPLICIT;
}
