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

#ifndef GLOBALLAYERMODEL_H
#define GLOBALLAYERMODEL_H

#include <QColor>
#include "geometries.h"
#include "layermanager.h"
#include "layermodel.h"

namespace Ilwis {
class GeoReference;
typedef IlwisData<GeoReference> IGeoReference;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Ui {
class IlwisObjectModel;

class ILWISCOREUISHARED_EXPORT RootLayerModel : public LayerModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QVariantMap viewEnvelope READ viewEnvelopePrivate NOTIFY viewEnvelopeChanged)
    Q_PROPERTY(QVariantMap zoomEnvelope READ zoomEnvelopePrivate NOTIFY zoomEnvelopeChanged)
    Q_PROPERTY(QVariantMap latlonEnvelope READ latlonEnvelope NOTIFY latlonEnvelopeChanged)
    Q_PROPERTY(bool zoomInMode READ zoomInMode WRITE setZoomInMode NOTIFY zoomInModeChanged)
    Q_PROPERTY(bool panningMode READ panningMode WRITE setPanningMode NOTIFY panningModeChanged)
    Q_PROPERTY(bool showLayerInfo READ showLayerInfo WRITE showLayerInfo NOTIFY showLayerInfoChanged)
    Q_PROPERTY(QVariantList layerInfoItems READ layerInfoItems NOTIFY layerInfoItemsChanged)
    Q_PROPERTY(Ilwis::Ui::IlwisObjectModel* screenCsy READ screenCsyPrivate NOTIFY coordinateSystemChanged)
    Q_PROPERTY(Ilwis::Ui::IlwisObjectModel* screenGrf READ screenGrfPrivate NOTIFY georefChanged)
    Q_PROPERTY(QString projectionInfo READ projectionInfoPrivate NOTIFY projectionInfoChanged)
    Q_PROPERTY(QString currentLatLon READ currentLatLon NOTIFY currentLatLonChanged)
    Q_PROPERTY(QString currentCoordinate READ currentCoordinate WRITE setCurrentCoordinate NOTIFY currentcurrentCoordinateChanged)
	Q_PROPERTY(double left READ left NOTIFY extentChanged)
	Q_PROPERTY(double right READ right NOTIFY extentChanged)
	Q_PROPERTY(double top READ top NOTIFY extentChanged)
	Q_PROPERTY(double bottom READ bottom NOTIFY extentChanged)
	Q_PROPERTY(double zoomFactor READ zoomFactor WRITE zoomFactor NOTIFY zoomChanged)
	Q_PROPERTY(QVariantMap cameraPosition READ cameraPosition NOTIFY cameraPositionChanged)
	Q_PROPERTY(QVariantMap scrollInfo READ scrollInfo WRITE scrollInfo NOTIFY scrollInfoChanged)

     Q_INVOKABLE QVariantMap screen2Coord(const QVariantMap &var) const;
     Q_INVOKABLE QVariantMap coord2Screen(const QVariantMap &var) const;
	 Q_INVOKABLE QVariantMap screen2raster(const QString& rasterid, int c, int r) const;
	 Q_INVOKABLE QVariantMap raster2screen(IlwisObjectModel *obj, double x, double y) const;
     Q_INVOKABLE QVariantMap drawEnvelope(const QString &envelope) const;
     Q_INVOKABLE QString layerInfo(const QString &pixelpair) ;
     Q_INVOKABLE QVariant vproperty(const QString& attrName) const;
     Q_INVOKABLE void vproperty(const QString& attrName, const QVariant& value);
	 Q_INVOKABLE void initSizes(int newwidth, int newheight, bool initial);
     Q_INVOKABLE void setActiveAttribute(int idx);
     Q_INVOKABLE void reset(int pixwidth, int pixheight);
     Q_INVOKABLE QVariantMap coord2coord(LayerModel* source, double x, double y) const;


	 //void setEnvelopeView(const BoundingBox& pixelArea);


    RootLayerModel();
    RootLayerModel(LayerManager *ln, QStandardItem* parent);

     QString url() const;

     IGeoReference screenGrf() const;
     int numberOfBuffers(const QString&) const;
     QVariantList calcAxisValues(const QString& axisType, const Coordinate & cmin, const Coordinate & cmax) const;
     void screenGrf(const IGeoReference &screenGrf);
     ICoordinateSystem screenCsy() const;
     void screenCsy(const ICoordinateSystem &screenCsy);

     QString currentCoordinate() const;
     void setCurrentCoordinate(const QString& var);
     QString currentLatLon() const;
     
    // QString layerInfo(const Coordinate &crdIn, const QString& attrName, QVariantList &items) ;


     bool zoomInMode() const;
     void setZoomInMode(bool yesno) ;
     bool panningMode() const;
     void setPanningMode(bool yesno) ;
     void viewEnvelope(const Envelope& env);
	 Envelope viewEnvelope() const;
	 void viewBox(const BoundingBox& env);
	 BoundingBox viewBox() const;
	 Envelope coverageEnvelope() const;
	 void coverageEnvelope(const Envelope &coverageEnvelope);
     void clearLayers();
	 double zoomFactor() const;
	 void zoomFactor(double zf);
	 QVariantMap cameraPosition() const;
     void active(bool yesno);
     bool active() const;

     Envelope zoomEnvelope() const;
     void zoomEnvelope(const Envelope &zoomEnvelope);
     bool showLayerInfo() const;
     void showLayerInfo(bool showLayerInfo);
	 void scrollInfo(const QVariantMap& si);
	 QVariantMap scrollInfo() const;
     bool prepare(int prepType);
     QVariantList gridAxis(const QString& type) const;
     virtual bool renderReady();
     virtual void renderReady(bool yesno);


signals:
     void zoomInModeChanged();
     void panningModeChanged();
     void hasSelectionDrawerChanged();
     void coordinateSystemChanged();
     void viewEnvelopeChanged();
     void zoomEnvelopeChanged();
     void latlonEnvelopeChanged();
     void showLayerInfoChanged();
     void layerInfoItemsChanged();
     void currentLatLonChanged();
     void currentcurrentCoordinateChanged();
	 void extentChanged();
	 void zoomChanged();
	 void cameraPositionChanged();
	 void scrollInfoChanged();
     void activeAttributeChanged();
     void georefChanged();
     void projectionInfoChanged();
     

private:
    QColor _backgroundColor;
    IGeoReference _screenGrf;
    ICoordinateSystem  _screenCsy;
    IlwisObjectModel *_csy = 0;
    IlwisObjectModel *_grf = 0;
    Envelope _viewEnvelope;
    Envelope _zoomEnvelope;
	BoundingBox _viewBox; // base size of the BB which contains the whole map
	Envelope _coverageEnvelope; //base size of the original first layer
    bool _zoomInMode = false;
    bool _panningMode = false;
    bool _showLayerInfo = true;
    QVariantList _layerInfoItems;
	Coordinate _cameraPosition;
	double _zoomFactor = 1.0;
	Coordinate _currentCoordinate;
    QString _activeAttribute; // pseudo attributes as they represent things like grids, geometry, 3d etc.. global properties for the whole MapPane

    QVariantList layerInfoItems() ;
    QVariantMap viewEnvelopePrivate() const;
    QVariantMap zoomEnvelopePrivate() const;
    QVariantMap latlonEnvelope() const;
    IlwisObjectModel *screenCsyPrivate();
    IlwisObjectModel *screenGrfPrivate();
    QString projectionInfoPrivate() const;
    double left() const;
    double right() const;
    double top() const;
    double bottom() const;
    void RecenterZoomHorz(Envelope & cbZoom, const Envelope & cbMap);
    void RecenterZoomVert(Envelope & cbZoom, const Envelope & cbMap);
    void modifyZoomX(double rFactor);
    void modifyZoomY(double rFactor);
    void SetCameraPosition();

    //NEW_LAYERMODEL(GlobalLayerModel);

};
}
}

#endif // GLOBALLAYERMODEL_H
