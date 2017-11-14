#pragma once
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\oglbuffer.h"

namespace Ilwis {
	class SPFeatureI;


	namespace Ui {
		class FeatureLayerModel;
		class VisualAttribute;

		class VectorLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			VectorLayerModel(QObject *parent = 0);
			VectorLayerModel(LayerManager * manager, QObject * parent, const QString & name, const QString & desc, const IOOptions& options);
			QVariant vproperty(const QString & attrName) const;
			void vproperty(const QString & attrName, const QVariant & value);
			void fillAttributes();
			void activeAttribute(const QString & pName);
			virtual void addFeature(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			virtual int numberOfBuffers(const QString&) const;
		signals:
			void activeAttributeChanged();

		protected:
			FeatureLayerModel *_featureLayer;
			OGLBuffer _buffer;
			QString _activeAttribute;
		};
	}
}