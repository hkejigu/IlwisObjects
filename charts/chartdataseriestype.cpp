#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "domain.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "chartmodel.h"
#include "dataseriesmodel.h"
#include "chartoperation.h"
#include "chartdataseriestype.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ChartDataseriesType)

ChartDataseriesType::ChartDataseriesType()
{

}

ChartDataseriesType::ChartDataseriesType(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation *ChartDataseriesType::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ChartDataseriesType(metaid, expr);
}

Ilwis::OperationImplementation::State ChartDataseriesType::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    ChartOperation::prepare(ctx, st);

    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not assign new dataseries type"));
        return sPREPAREFAILED;
    }

    _seriesName = _expression.input<QString>(1);
    _newType = (_expression.input<QString>(2)).toLower();

    return sPREPARED;
}

bool ChartDataseriesType::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    auto serie = _chartmodel->getSeriesByName(_seriesName);
    serie->setType(_newType);
    _chartmodel->updateSeriesChanged();

    return true;
}

quint64 ChartDataseriesType::createMetadata()
{
    OperationResource resource({ "ilwis://operations/chartdataseriestype" });
    resource.setLongName("Change type of a chart series");
    resource.setSyntax("chartseriestype(parameters))");
    resource.setInParameterCount({ 3 });
    resource.addInParameter(0, itINT32, TR("chart ID"), TR("the ID identifying the chart containing the series."));
    resource.addInParameter(1, itSTRING, TR("series name"), TR(""));
    resource.addInParameter(2, itSTRING, TR("type"), TR("the type of the dataseries (f.e. line, spline, pie, bar)"));
    resource.setKeywords("chart series, table, chart, charttype");

    mastercatalog()->addItems({ resource });
    return resource.id();
}
