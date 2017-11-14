#pragma once
#include "coveragedisplay/layermodel.h"
#include "vectorlayermodel.h"
#include "ilwiscoreui_global.h"

#include <QColor>
#include <QVector3D>

namespace Ilwis {
	class Coverage;
	typedef IlwisData<Coverage> ICoverage;

	namespace Ui {
		class FeatureLayerModel;

		class PolygonLayerModel : public VectorLayerModel
		{
		public:
			PolygonLayerModel();
			PolygonLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options);

			bool prepare(int prepType);
			QString value2string(const QVariant &value, const QString &attrName) const;
			void addFeature(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			Q_INVOKABLE virtual int numberOfBuffers(const QString& dummy) const;
			Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<qreal> colors(qint32 bufferIndex, const QString& ) const;

			static LayerModel *create(LayerManager *, LayerModel *, const QString&, const QString&, const IOOptions&);

			NEW_LAYERMODEL(PolygonLayerModel);
			NEW_LAYERMODEL2(PolygonLayerModel);


		private:
			std::shared_ptr<BaseSpatialAttributeSetter> _polygonsetter;
		};
	}
}

