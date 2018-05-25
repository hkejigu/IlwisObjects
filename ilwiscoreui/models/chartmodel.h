#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QList>
#include <QQmlListProperty>
#include "ilwis.h"
#include "ilwiscoreui_global.h"

#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "table.h"

namespace Ilwis {
    namespace Ui {
        class TableModel;
		class DataseriesModel;

        class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
        {
            Q_OBJECT

        public:
            enum Axis{aXAXIS, aYAXIS, aZAXIS};

            Q_PROPERTY(QString chartType READ chartType WRITE chartType NOTIFY chartTypeChanged)
			Q_PROPERTY(QString name      READ name                      NOTIFY nameChanged)
			Q_PROPERTY(int seriesCount   READ seriesCount               NOTIFY seriesCountChanged)

			Q_PROPERTY(double minX         READ minx                    NOTIFY xAxisChanged)
			Q_PROPERTY(double maxX         READ maxx                    NOTIFY xAxisChanged)
			Q_PROPERTY(double minY         READ miny                    NOTIFY yAxisChanged)
			Q_PROPERTY(double maxY         READ maxy                    NOTIFY yAxisChanged)

            ChartModel();
            ChartModel(QObject *parent);

            quint32 createChart(const QString& name, const Ilwis::ITable& tbl, const QString& chartType, quint32 xaxis, quint32 yaxis, quint32 zaxis);

            QVariantList ChartModel::linkProperties() const;
            quint32 ChartModel::modelId() const;
            QString ChartModel::name() const;
            static QString mainPanelUrl();
            void chartType(const QString& tp);
            QString chartType() const;

			int seriesCount() const;
			Q_INVOKABLE Ilwis::Ui::DataseriesModel* getSeries(int seriesIndex) const;
			Ilwis::Ui::DataseriesModel* getSeriesByName(const QString name) const;
			bool isValidSeries(const QString columnName) const;	// check if column with column name exists
			quint32 addDataSeries(quint32 xaxis, quint32 yaxis, quint32 zaxis);
			Ilwis::ITable table() const;

        signals:
            void chartTypeChanged();
			void nameChanged();
			void chartModelChanged();
			void seriesCountChanged();
			void xAxisChanged();
			void yAxisChanged();

        private:
			double minx() {
				return _minx;
			}
			double maxx() {
				return _maxx;
			}
			double miny() {
				return _miny;
			}
			double maxy() {
				return _maxy;
			}

			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
			QString _chartType = sUNDEF;
            Ilwis::ITable _table;
            quint32 _modelId;
            QString _name = sUNDEF;
			QList<DataseriesModel *> _series;
        };
    }
}

#endif // CHARTMODEL_H
