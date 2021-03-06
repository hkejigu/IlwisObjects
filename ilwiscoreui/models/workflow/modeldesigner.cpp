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
#include "mastercatalog.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "workflownode.h"

#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationcatalogmodel.h"
#include "ilwiscontext.h"
#include "ilwistypes.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "oshelper.h"
#include "modellerfactory.h"
#include "modelbuilder.h"
#include "modeldesigner.h"

using namespace Ilwis;
using namespace Ui;

ModelDesigner::ModelDesigner(QObject *parent) : ResourceModel(Resource(), 0)
{
    if ( parent){
        _model.prepare();
        resource(_model->resource());
    }
}

ModelDesigner::ModelDesigner(ResourceModel *rmodel, QObject *parent) : ResourceModel(rmodel->resource(),parent)
{
    if ( rmodel->type() == itWORKFLOW){
        _model.prepare(); // anonymous model
        //addWorkflow("itemid=" + QString::number(rmodel->resource().id()));
    }else
        _model.prepare(rmodel->id().toULongLong());
    if ( _model.isValid()){
        if ( rmodel->type() != itWORKFLOW){ // special case, workflows are openend as anon models and are added above, no replication here
            for(int i=0; i < _model->workflowCount(); ++i){
                _workflowmodels.push_back(new WorkflowModel(_model->workflow(i)->resource(), this));
            }
        }
        for(int i=0; i < _model->analysisCount(); ++i){
            AnalysisModel *amodel = modelbuilder()->createAnalysisModel(_model->analysisPattern(i).get(), this);
            if ( amodel){
                _analysismodels.push_back(amodel);
            }
        }
        for(int i=0; i < _model->applicationCount(); ++i){
            ApplicationModelUI *amodel = modelbuilder()->createApplicationModelUI(_model->application(i).get(), parent);
            if ( amodel){
                _appmodels.push_back(amodel);
            }
        }
    }
}

qint32 ModelDesigner::workflowCount() const
{
    return _workflowmodels.size();
}

WorkflowModel *ModelDesigner::workflow(qint32 index) const
{
    if ( index < _workflowmodels.size())
        return _workflowmodels[index];
    return 0;
}

WorkflowModel *ModelDesigner::workflow(const QString &name) const
{
    for(WorkflowModel *wf : _workflowmodels ){
        if ( wf->name() == name){
            return wf;
        }
    }
    return 0;
}

WorkflowModel *ModelDesigner::addWorkflow(const QString& filter)
{
    try {
        if (_model.isValid()){
            std::vector<Resource> res = Ilwis::mastercatalog()->select(filter);
            if ( res.size() == 1){
                WorkflowModel *wfm = workflow(res[0].name()); // see if is a duplicate
                   if ( wfm)
                       return wfm;
                IWorkflow wf;
                wf.prepare(res[0]);
                if (wf.isValid() && _model->addWorklfow(wf)){
                    wfm = new WorkflowModel(res[0], this);
                    _workflowmodels.push_back(wfm);
                    emit workflowCountChanged();
                    return wfm;
                }
                Ilwis::kernel()->issues()->log(QString(TR("workflow couldnt be added, maybe a duplicate?")).arg(filter),Ilwis::IssueObject::itWarning);
                return 0;
            }else {

                Ilwis::kernel()->issues()->log(QString(TR("workflow %1 doesnt exist")).arg(filter),Ilwis::IssueObject::itWarning);
            }
        }
    }
    catch(const ErrorObject& ){}

    return 0;
}

void ModelDesigner::removeWorkflow(const QString &name)
{
    for(int i =0; i < _workflowmodels.size(); ++i ){
        if ( _workflowmodels[i]->name() == name){
            _workflowmodels.erase(_workflowmodels.begin() + i);
            break;
        }
    }
}

void ModelDesigner::removeWorkflow(qint32 index)
{
    if ( index < _workflowmodels.size())
        _workflowmodels.erase(_workflowmodels.begin() + index);
}

QStringList ModelDesigner::analysisTypes() const
{
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
    if ( factory){
        return factory->analysisTypes();
    }
    return QStringList();
}

QStringList ModelDesigner::workflowNames() const{
    QStringList names;
    for(WorkflowModel *wf : _workflowmodels )    {
        names.push_back(wf->name());
    }
    return names;
}

QStringList ModelDesigner::applicationNames(const QString& analysisName) const
{
    QStringList names;
    if ( analysisName != "") {
        for(ApplicationModelUI *app : _appmodels )    {
            if ( app->attachedAnalysis() == analysisName)
                names.push_back(app->name());
        }
    }
    return names;
}

QStringList ModelDesigner::applicationsByAnalysis(const QString &analysisType) const
{
    if ( analysisType == "")
        return QStringList();
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
    if ( factory){
        return factory->applications(analysisType);
    }
    return QStringList();
}

QStringList ModelDesigner::analysisNames() const
{
    QStringList names;
    for(AnalysisModel *am : _analysismodels )    {
        names.push_back(am->name());
    }
    return names;
}

QStringList ModelDesigner::conceptNames() const
{
    return QStringList();
}

WorkflowModel *ModelDesigner::currentWorkflow()
{
    return _currentWorkflow;
}

void ModelDesigner::currentWorkflow(WorkflowModel *cwf)
{
    for(WorkflowModel *wf : _workflowmodels ){
        if ( wf->name() == cwf->name()){
            _currentWorkflow = cwf;
            break;
        }
    }
}

qint32 ModelDesigner::analysisCount() const
{
    return _analysismodels.size();
}

AnalysisModel *ModelDesigner::analysisModel(qint32 index) const
{
    if ( index < _analysismodels.size()){
        return _analysismodels[index]    ;
    }
    return 0;
}

bool ModelDesigner::addAnalysisPattern(AnalysisModel *amodel)
{
    if ( !amodel)
        return false;
    for(int i =0; i < _analysismodels.size() && amodel != 0; ++i ){
        if ( _analysismodels[i]->name() == amodel->name()){
            return false;
        }
    }
    _analysismodels.push_back(amodel);

    return true;
}

void ModelDesigner::removeAnalysisPattern(const QString &name)
{
    for(int i =0; i < _analysismodels.size(); ++i ){
        if ( _analysismodels[i]->name() == name){
            _analysismodels.erase(_analysismodels.begin() + i);
            break;
        }
    }
}

void ModelDesigner::removeAnalysisPattern(qint32 index)
{
    if ( index < _analysismodels.size())
        _analysismodels.erase(_analysismodels.begin() + index);
}

qint32 ModelDesigner::applicationCount() const
{
    return _appmodels.size();
}

ApplicationModelUI *ModelDesigner::application(qint32 index) const
{
    if ( index < _appmodels.size())
        return _appmodels[index];

    return 0;
}

bool ModelDesigner::addApplication(ApplicationModelUI *app)
{
    if (!app)
        return false;

    for(int i =0; i < _appmodels.size() && app != 0; ++i ){
        if ( _appmodels[i]->name() == app->name()){
            return false;
        }
    }
    _appmodels.push_back(app);
    return true;
}

void ModelDesigner::removeApplication(const QString &name)
{

}

void ModelDesigner::removeApplication(qint32 index)
{

}

bool ModelDesigner::store(const QString &container, const QString &name)
{
    if ( container == "" && name == ""){
        _model->store();
    } else if ( container == "" && name != "") {
        _model->name(name);
        _model->store();
    }else if ( OSHelper::neutralizeFileName(container) == OSHelper::neutralizeFileName(_model->resource().container().toString())){
        if ( name == "")    {
            _model->store();
        }else {
            _model->name(name);
            _model->store();
        }
    }else {
        QString cont = container;
        if ( container.indexOf(".ilwis") != 0){ // oops somebody put a file, not a container
            int index = container.lastIndexOf("/");
            cont = container.left(index);
        }
        QString newName = name == "" ? _model->name() : name;
        QUrl newUrl = cont + "/" + newName;
        _model->connectTo(newUrl,"model","stream",IlwisObject::cmOUTPUT);
        _model->store();
    }
    return true;
}

qint32 ModelDesigner::conceptCount() const
{
    return _conceptmodels.size();
}
