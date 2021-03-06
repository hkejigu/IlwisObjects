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

#include "tabmodel.h"
#include "uicontextmodel.h"
#include "mastercatalog.h"
#include "oshelper.h"
#include "datapanemodel.h"
#include "layermanager.h"
#include "coveragelayermodel.h"
#include "modelregistry.h"
#include "operationhelper.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace Ui;

DataPaneModel::DataPaneModel(QObject *parent) :
    QObject(parent),
    _leftside(new SidePanelModel("left",this)),
    _rightside(new SidePanelModel("right", this))
{

}

bool DataPaneModel::removeTab(bool lftside, int tabIndex)
{
    try{
        bool ok = false;
        if ( lftside){
            if ( _rightside->tabCount() > 0 || _leftside->tabCount() > 1) { // you can not remove the last tab
                ok  = _leftside->removeTab(tabIndex);
            }
        }else {
            if ( _leftside->tabCount() > 0 || _rightside->tabCount() > 1) {
                ok = _rightside->removeTab(tabIndex);
            }
        }
        if ( ok){
            if ( lftside ){
                if ( _leftside->tabCount() > 0)
                    _leftside->select(std::max(0,tabIndex - 1), true);
                else
                    _rightside->select(0, true);
            }else {
                if ( _rightside->tabCount() > 0)
                    _rightside->select(std::max(0,tabIndex - 1), true);
                else
                    _leftside->select(0, true);
            }

        }
        return ok;
    } catch(const ErrorObject& ){}
    return false;
}

TabModel *DataPaneModel::tab(bool lftside, int index)
{
    if ( lftside)
        return _leftside->tab(index);
    return _rightside->tab(index);
}

void DataPaneModel::select(bool lftside, int index, bool yesno)
{
    try{
        if ( lftside){
            _rightside->select(100000, false);
            _leftside->select(index, yesno);
        } else {
            _leftside->select(100000, false);
            _rightside->select(index, yesno);
        }
    } catch (const ErrorObject& ){

    }
}

bool DataPaneModel::isSelected(bool lftside, int index) const
{
    if ( lftside)
        return _leftside->isSelected(index);
    return _rightside->isSelected(index);
}

bool DataPaneModel::leftActive() const
{
    return _leftside->isActive();
}

SidePanelModel *DataPaneModel::rightSide() const
{
    return _rightside;
}

SidePanelModel *DataPaneModel::leftSide() const
{
    return _leftside;
}

TabModel *DataPaneModel::createPanel(const QString &filter2, const QString &outputtype, const QString& url, const QString& side)
{
    QString filter = OperationHelper::unquote(filter2);
    SidePanelModel *sidepanel;
    if ( side == "other"){
        sidepanel = leftActive() ? _rightside : _leftside;
    }else
        sidepanel = (side == "right") ? _rightside : _leftside; // in the inital situation the leftpane is always choosen

    TabModel *tab =sidepanel->createPanel(10000,filter,outputtype, url);
    QObject *pane = uicontext()->rootObject()->findChild<QObject *>("datapane_mainui");
    if (tab && pane){
        QVariant w = pane->property("width");
        if ( sidepanel->width() < w.toInt()){
            sidepanel->width(w.toInt()/2);
        }
        if ( side == "other"){
            select(!leftActive(),sidepanel->tabCount() - 1, true);
        }else
            select(side == "left",sidepanel->tabCount() - 1, true); //in the initial situation always the left pane will be choosen else it will flip to the other pane
    }

    return tab;
}

SidePanelModel *DataPaneModel::activeSide() const
{
    if ( _leftside->tabCount() == 0 && _rightside->tabCount() == 0) // start situation, left will be active
        return _leftside;
    return _leftside->isActive() ? _leftside : _rightside;
}

//-----------------------------------------------------------------------
SidePanelModel::SidePanelModel(QObject *parent) : QObject(parent)
{

}

SidePanelModel::SidePanelModel(const QString &side, QObject *parent) : QObject(parent)
{
    _side = side;
}

QQuickItem *SidePanelModel::tabview() const
{
    QQuickItem *tbview = uicontext()->rootObject()->findChild<QQuickItem *>(_tabview);
    return tbview;
}

void SidePanelModel::setTabview(const QString& name)
{
    _tabview = name;
}

QString SidePanelModel::state() const
{
    return _state;
}

void SidePanelModel::setState(const QString &st)
{
    _state = st;
}

int SidePanelModel::tabCount() const
{
    return (int)_tabs.size();
}

QString SidePanelModel::side() const
{
    return _side;
}

bool SidePanelModel::removeTab(int tabIndex)
{
    if ( tabIndex < _tabs.size()){
        _tabs.erase(_tabs.begin() + tabIndex);
        return true;
    }
    return false;
}

int SidePanelModel::width() const
{
    QVariant w = tabview()->property("width");
    return w.toInt();
}

void SidePanelModel::width(int w)
{
    tabview()->setProperty("width", w);
}

void SidePanelModel::select(int index, bool yesno)
{
    for(int i=0; i < _tabs.size(); ++i)
        _tabs[i]->setSelected(false);
    if ( index < _tabs.size() && index >= 0)
        _tabs[index]->setSelected(yesno);
}

bool SidePanelModel::isSelected(int index) const
{
      if ( index < _tabs.size()){
          return _tabs[index]->selected();
      }
      return false;
}

bool SidePanelModel::isActive() const
{
    for(int i=0; i < _tabs.size(); ++i){
        if ( _tabs[i]->selected())
            return true;
    }
    return false;
}

TabModel *SidePanelModel::createPanel(quint32 index, const QString &filter, const QString& outputtype, const QString& url)
{
    TabModel *tab = 0;

    if (outputtype == "chart") {
        tab = new TabModel(url, modelregistry()->mainPanelUrl("chart"), outputtype, this);
    }
    else if ( outputtype == "catalog"){
        context()->configurationRef().addValue("users/" + Ilwis::context()->currentUser() +"/workingcatalog",url);
        if ( filter == "container='ilwis://operations'")
            tab = new TabModel(url,"catalog/OperationPanel.qml", outputtype,this);
        else
            tab = new TabModel(url,"catalog/CatalogPanel.qml", outputtype, this);

    }else {
        std::vector<Ilwis::Resource> resources = Ilwis::mastercatalog()->select(Ilwis::OSHelper::neutralizeFileName(filter));
        if ( resources.size() > 0 ){
            if (hasType(resources[0].ilwisType(), itCOVERAGE)) {
				if (outputtype == "table")
					tab = new TabModel(url, modelregistry()->mainPanelUrl("table"), outputtype, this);
				else {
					tab = new TabModel(url, modelregistry()->mainPanelUrl("coverage"), outputtype, this);
				}
            }
            else if (hasType(resources[0].ilwisType(), itTABLE)) {
                tab = new TabModel(url, modelregistry()->mainPanelUrl("table"), outputtype, this);
            } else if (hasType(resources[0].ilwisType(), itWORKFLOW|itMODEL)){
                tab = new TabModel(url,"modeller/ModellerDataPane.qml", outputtype, this);
            } else if (hasType(resources[0].ilwisType(), itSCRIPT)){
                tab = new TabModel(url,"script/ScriptPane.qml", outputtype, this);
            } else if (hasType(resources[0].ilwisType(), itSINGLEOPERATION)){
                if ( resources[0].url(true).toString().indexOf(".py") != -1)
                    tab = new TabModel(url,"script/ScriptPane.qml", outputtype, this);
            }
        }
    }
    if ( tab){
        if ( index >= _tabs.size())
            _tabs.push_back(tab);
        else
            _tabs[index] = tab;
    }
    return tab;
}

TabModel *SidePanelModel::tab(int index)
{
    if ( index < _tabs.size()){
        return _tabs[index];
    }
    return 0;
}

