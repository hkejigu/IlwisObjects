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

#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "uicontextmodel.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "coveragedisplay/rootlayermodel.h"
#include "showcoverage.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ShowCoverage)

ShowCoverage::ShowCoverage()
{
}

ShowCoverage::ShowCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{
}

bool ShowCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    qint64 id = uicontext()->addMapPanel("itemid=" + QString::number(_id),_side, _url, _extraParameters);
    if ( id == -1)
        return false;

    QVariant v1;
    v1.setValue(id);
    ctx->setOutput(symTable, QVariant(v1), sUNDEF, itINTEGER, Resource());


    return true;
}

Ilwis::OperationImplementation *ShowCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ShowCoverage(metaid, expr);
}

Ilwis::OperationImplementation::State ShowCoverage::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    _url = _expression.input<QString>(0);
    ICoverage cov(_url, itCOVERAGE,{"mustexist", true});
    if ( !cov.isValid()){
        kernel()->issues()->log(_url + TR(" is not a valid coverage"));
        return sPREPAREFAILED ;
    }
    if ( cov->isAnonymous()){
        QString newName = "output_" + QString::number(cov->id()) + ".ilwis";
        cov->name(newName);
        cov->connectTo(cov->resource().url(true),TypeHelper::type2name(cov->ilwisType()),"stream",IlwisObject::cmOUTPUT);
        cov->store();
        _url = cov->resource().url(true).toString();
    }
    _id = cov->id();
    _side = _expression.input<QString>(1).toLower();
    if ( !(_side == "left" || _side == "right" || _side == "other")){
        kernel()->issues()->log(_side + TR(" is not a valid side type"));
        return sPREPAREFAILED ;
    }
	_extraParameters = _expression.input<QString>(2);


    return sPREPARED;
}

quint64 ShowCoverage::createMetadata()
{
    OperationResource operation({"ilwis://operations/showcoverage"});
    operation.setSyntax("showcoverage(coverageurl,!other|left|right)");
    operation.setDescription(TR("Creates a new mappanel with the indicated coverage"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itCOVERAGE , TR("Coverage url"),TR("The url of the coverage to be displayed"));
    operation.addInParameter(1,itSTRING , TR("Panel side"),TR("Which side should the panel open, left, right or at the other side of the active panel"));
	operation.addInParameter(2, itSTRING, TR("Extra Parameters"), TR("Extra info on how the coverage should be displayed/openend"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itINTEGER , TR("View id"),TR("Id of the view that is opened. The id can be used in subsequent visualization operations"));
    operation.setKeywords("visualization");
	operation.parameterNeedsQuotes(2);
    operation.addProperty("runinmainthread",true); // this operation invokes a qml method so it must run in the main thread

    mastercatalog()->addItems({operation});
    return operation.id();
}

