#include "layermodel.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "layermanager.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "layermodel.h"

using namespace Ilwis;
using namespace Ui;

//REGISTER_LAYERMODEL("layermodel",LayerModel)

quint64 LayerModel::_layerId = 0;
Ilwis::Ui::LayerModel::LayerModel()
{
}

LayerModel::LayerModel(TreeNode *parent) : TreeNode(QList<QVariant>(), parent)
{

}

LayerModel::LayerModel(LayerManager *manager, QObject *parent, const QString& name, const QString& desc, const IOOptions& opt) : TreeNode({ name }, parent), Identity(name, _layerId++, "", desc), _layerManager(manager), _parent(dynamic_cast<LayerModel *>(parent)), _geometryChanged(true)
{

}

LayerModel *LayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new LayerModel(manager, layer, name, desc, options);
}

void LayerModel::redraw() const
{

}

QVariant LayerModel::vproperty(const QString &key) const
{
	QVariant var;
	QVariant parentValue;
	if ( parentLayer())
		parentValue = parentLayer()->vproperty(key);

	if (key == "opacity") {
		double pv = parentValue.isValid() ? parentValue.toDouble() : 1.0;
		var = _opacity * pv;
	} if (key == "active") {
		bool pv = parentValue.isValid() ? parentValue.toBool() : true;
		var = pv && _active;
	}

    if ( key.indexOf("visualattribute") == 0){
        QStringList parts = key.split("|");
        if ( parts.size() == 3){
            auto *attr = visualAttribute(parts[2]);
            if ( attr){
                if ( parts[1] == "representation")
                    var.setValue<IRepresentation>(attr->representation());
                else if ( parts[1] == "stretchrange"){
                    var.setValue<NumericRange>(attr->stretchRange());
                } else if ( parts[1] == "domain"){
                    var.setValue<IDomain>(attr->attributeDomain());
                }
            }
        }
    }
    return var;
}

void LayerModel::vproperty(const QString &key, const QVariant &value)
{
    if ( key == "opacity")
       opacity(value.toFloat());
    if ( key == "active")
        active(value.toBool());



    if ( key.indexOf("visualattribute") == 0){
        QStringList parts = key.split("|");
       /* auto iter = _visualAttributes.find(parts[2]);
        if (iter == _visualAttributes.end())
            return;
        if ( parts.size() == 3){
            if ( parts[1] == "representation"){
                IRepresentation rpr = attrib.value<IRepresentation>();
                if ( rpr.isValid()){
                    VisualPropertyModel& attr = _visualAttributes[parts[2]];
                    attr.representation(rpr);
                }
            }else if( parts[1] == "domain"){
                IDomain dom = attrib.value<IDomain>();
                if ( dom.isValid()){
                    VisualPropertyModel& attr = _visualAttributes[parts[2]];
                    attr.domain(dom);
                }
            }
        }*/
    }
}

void LayerModel::addVisualAttribute(VisualAttribute *attr)
{
    if ( !visualAttribute(attr->attributename())){
        _visualAttributes.append(attr);
        emit visualAttributesChanged();
    }
}

QString LayerModel::layerInfo(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const
{
    QString alltext;
    for(auto node : children()){
		auto lyr = node->as<LayerModel>();
        QString txt = lyr->layerInfo(crdIn, attrName, items);
        if ( alltext != "")
            alltext += ";";
        alltext += txt;

    }
    return alltext;
}

QString LayerModel::icon() const
{
    return _icon;
}

QString Ilwis::Ui::LayerModel::layerId() const
{
	return QString::number(id());
}

IlwisTypes LayerModel::layerType() const
{
	return _layerType;
}

Q_INVOKABLE int LayerModel::numberOfBuffers(const QString&) const
{
	return 0;
}

QVector<qreal> LayerModel::vertices(qint32 index, const QString& tp) const
{
	return QVector<qreal>();
}

Q_INVOKABLE QVector<int> LayerModel::indices(qint32 bufferIndex, const QString& tp) const
{
	return QVector<int>();
}


QVector<qreal> LayerModel::colors(qint32 index, const QString& tp) const
{
	return QVector<qreal>();
}

const LayerManager *LayerModel::layersManager() const
{
    return _layerManager;
}

LayerManager * Ilwis::Ui::LayerModel::layersManager()
{
	return _layerManager;
}


QQmlListProperty<VisualAttribute> LayerModel::vattributes()
{
    return QQmlListProperty<VisualAttribute>(this, _visualAttributes);
}

int LayerModel::activeVProperty() const
{
    return _activeVProperty;
}

void LayerModel::activeVProperty(int index)
{
  if ( index < _visualAttributes.size() && index >= 0){
      _activeVProperty = index;
  }
}

QString LayerModel::visualVPropertyByIndex(int index) const
{
    if ( index < _visualAttributes.size()){
       return _visualAttributes[index]->attributename();
    }
    return sUNDEF;
}

void LayerModel::addLayer(LayerModel *layer)
{
    appendChild(layer);
}

VisualAttribute *LayerModel::visualAttribute(const QString &attrName) const
{
    for(VisualAttribute *attr : _visualAttributes){
        if ( attr->attributename() == attrName)
            return attr;
    }
    return 0;
}

double LayerModel::opacity() const
{
    return _opacity;
}

void LayerModel::opacity(double opacity)
{
	if (opacity >= 0 && opacity <= 1.0) {
		if (opacity != _opacity) {
			_opacity = opacity;
			add2ChangedProperties("opacity", childCount() > 0);

			emit opacityChanged();
		}
	}
}

LayerModel *LayerModel::layer(int index){
    if ( index < childCount())
        return child(index)->as<LayerModel>();
    return 0;
}

LayerModel *LayerModel::parentLayer() const
{
    return _parent;
}

void LayerModel::clearLayers()
{
	removeChildren(iUNDEF, 0);
    _visualAttributes = QList<VisualAttribute *>();
	_changedProperties = std::set<QString>();
	_geometryChanged = true;
	_meshes = std::vector<quint32>();
}

int LayerModel::layerCount() const
{
	return childCount();
}

LayerModel * Ilwis::Ui::LayerModel::findLayerByName(const QString & name)
{
	for (auto *node : children()) {
		auto lyr = node->as<LayerModel>();
		if (lyr->name() == name) {
			return lyr;
		}
		else {
			LayerModel *child = lyr->findLayerByName(name);
			if (child)
				return child;
		}
	}
	return 0;
}

void LayerModel::moveLayer(LayerModel *lyr, int type)
{
    int index = 0;
    for(;index < layerCount(); ++index){
        if ( lyr == child(index)->as<LayerModel>())    {
            break;
        }
    }

   /* if ( type == lmDOWN){
        if ( index < layerCount())
            _layers.swap(index, index + 1);
    }else if ( type == lmUP ){
        if ( index > 0  && index < _layers.size())
            _layers.swap(index, index);
    } else if ( type == lmREMOVE){
        if ( index >= 0 && index < _layers.size())
        _layers.removeAt(index);
    }*/
}

QString LayerModel::url() const
{
    return "";
}

QQmlListProperty<TreeNode> LayerModel::layersPrivate()
{
    return QQmlListProperty<TreeNode>(this, children());
}

bool Ilwis::Ui::LayerModel::isDrawable() const
{
	return _isDrawable;
}

bool Ilwis::Ui::LayerModel::isVectorLayer() const
{
	return _isVector;
}

bool Ilwis::Ui::LayerModel::usesColorData() const
{
	return false;
}

bool LayerModel::active() const
{
    return _active;
}

void LayerModel::active(bool yesno)
{
	if (yesno != _active) {
		_active = yesno;
		add2ChangedProperties("active", childCount() > 0);
		emit onActiveChanged();
	}
}

quint32 Ilwis::Ui::LayerModel::meshCount() const
{
	return _meshes.size();
}

quint32 Ilwis::Ui::LayerModel::meshIndex(int n) const
{
	if (n < _meshes.size())
		return _meshes[n];
	return -1;
}

void Ilwis::Ui::LayerModel::addMeshIndex(quint32 idx)
{
	_meshes.push_back(idx);
}

QStringList Ilwis::Ui::LayerModel::changedProperties() const
{
	QStringList props;
	for (auto item : _changedProperties) {
		props.append(item);
	}
	return props;
}

void Ilwis::Ui::LayerModel::removeFromChangedProperties(const QString & prop)
{
	auto iter = std::find(_changedProperties.begin(), _changedProperties.end(), prop);
	if (iter != _changedProperties.end())
		_changedProperties.erase(iter);
}

void Ilwis::Ui::LayerModel::add2ChangedProperties(const QString & prop, bool propagate)
{
	_changedProperties.insert(prop);
	if (propagate) {
		for (auto *node : children()) {
			auto childLayer = node->as<LayerModel>();
			childLayer->add2ChangedProperties(prop, propagate);
		}
	}
}

bool Ilwis::Ui::LayerModel::updateGeometry() const
{
	return _geometryChanged;
}

void Ilwis::Ui::LayerModel::updateGeometry(bool yesno)
{
	_geometryChanged = yesno;
}

bool Ilwis::Ui::LayerModel::isValid() const
{
	return _isValid;
}

bool LayerModel::prepare(int) {
	return true;
}

bool LayerModel::isPrepared(quint32 type) const
{
	return hasType(_prepared, type);
}

void LayerModel::fillAttributes()
{

}

void LayerModel::fillData() {

	QVariantMap vmap;
	vmap["name"] = name();
	vmap["icon"] = icon();
	vmap["readonly"] = readonly();
	vmap["nodeid"] = nodeId();
	vmap["active"] = active();
	setData(0, vmap);
}

