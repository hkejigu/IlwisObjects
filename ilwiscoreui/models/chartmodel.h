#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QList>
#include <QQmlListProperty>
#include <QColor>
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

        class ItemCategory {
        public:
            int upper;
            QString label;
        };

        class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
        {
            Q_OBJECT

        public:
            enum Axis { AXAXIS = 1, AYAXIS = 2, AZAXIS = 3};
            enum AxisType { AT_VALUE = 1, AT_DATETIME = 2, AT_CATEGORIES = 3};
            Q_ENUM(Axis)
            Q_ENUM(AxisType)

            Q_PROPERTY(QString chartType READ chartType WRITE chartType NOTIFY chartTypeChanged)
            Q_PROPERTY(QString name      READ name                      NOTIFY nameChanged)
            Q_PROPERTY(int seriesCount   READ seriesCount               NOTIFY seriesCountChanged)
            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::DataseriesModel> series READ series NOTIFY chartModelChanged)

                Q_PROPERTY(quint16 xaxisType   READ xaxisType)
                Q_PROPERTY(double minX         READ minx                    NOTIFY xAxisChanged)
                Q_PROPERTY(double maxX         READ maxx                    NOTIFY xAxisChanged)
                Q_PROPERTY(double minY         READ miny                    NOTIFY yAxisChanged)
                Q_PROPERTY(double maxY         READ maxy                    NOTIFY yAxisChanged)
                Q_PROPERTY(bool fixedY         READ fixedYAxis  WRITE setFixedYAxis  NOTIFY yAxisChanged)

                Q_PROPERTY(bool updateSeries READ updateSeries NOTIFY updateSeriesChanged)
                Q_PROPERTY(int tickCountX READ tickCountX WRITE tickCountX NOTIFY tickCountXChanged)
                Q_PROPERTY(int tickCountY READ tickCountY WRITE tickCountY NOTIFY tickCountYChanged)
                Q_PROPERTY(QString formatXAxis READ formatXAxis NOTIFY xAxisChanged)
                Q_PROPERTY(QString formatYAxis READ formatYAxis NOTIFY yAxisChanged)

                Q_INVOKABLE QColor seriesColor(int seriesIndex);
                Q_INVOKABLE QColor seriesColorItem(int seriesIndex, double v);
                Q_INVOKABLE Ilwis::Ui::DataseriesModel* getSeries(int seriesIndex) const;
                Q_INVOKABLE bool addDataTable(const QString& objid);
                Q_INVOKABLE bool addDataTable(const QString & objid, const QString& xcolumn, const QString& ycolumn, const QString& color);
                Q_INVOKABLE void assignParent(QObject *parent);
                Q_INVOKABLE quint32 modelId() const;
                Q_INVOKABLE QString dataTableUrl();

            ChartModel();
            ChartModel(QObject *parent);
            ~ChartModel();

            quint32 createChart(const QString& name, const Ilwis::ITable& tbl, const QString & cType, const QString& xaxis, const QString& yaxis, const QString& zaxis);

            QVariantList linkProperties() const;
            QString name() const;
            static QString mainPanelUrl();
            static QString minimalPanelUrl();
            void chartType(const QString& tp);
            QString chartType() const;

			int seriesCount() const;
			DataseriesModel* getSeriesByName(const QString name) const;
            DataseriesModel* getSeries(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) const;
            quint32 deleteSerie(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn);
			bool isValidSeries(const ITable& inputTable, const QString columnName) const;	// check if column with column name exists
            quint32 insertDataSeries(const ITable& inputTable, quint32 index, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn, const QColor& color);
            void updateDataSeries(const ITable& inputTable, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn);
            bool updateSeries() const;
            void tickCountX(int tc);
            int tickCountX() const;
            void tickCountY(int tc);
            int tickCountY() const;
            QColor newColor() const;

            quint16 xaxisType() const;

            // value axis
            double minx() const;
            double maxx() const;
            double miny() const;
            double maxy() const;
            void setMinX(double val);
            void setMaxX(double val);
            void setMinY(double val);
            void setMaxY(double val);
            bool fixedYAxis() const;
            void setFixedYAxis(bool fixed);

        signals:
            void chartTypeChanged();
			void nameChanged();
			void chartModelChanged();
			void seriesCountChanged();
			void xAxisChanged();
			void yAxisChanged();
            void updateSeriesChanged();
            void tickCountXChanged();
            void tickCountYChanged();

        private:
			QQmlListProperty<DataseriesModel> series();

            void initializeDataSeries(DataseriesModel *newseries);
            QString formatXAxis() const;
            QString formatYAxis() const;
            bool axisCompatible(const DataDefinition& inputDef, Axis axis, bool basicCheck=true);

			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
            bool _fixedY = false;
            bool _fixedX = false;
            AxisType _xaxisType;
			QString _chartType = sUNDEF;
            int _tickCountX = 5;
            int _tickCountY = 5;
            quint32 _modelId;
            QString _name = sUNDEF;
            ITable _datatable;

			QList<DataseriesModel *> _series;
            std::vector<QString> _graphColors = { "crimson", "blueviolet","olive","deeppink","darkblue","darkred","cadetblue","tomato", "darkslateblue","orangered", "dodgerblue","forestgreen","goldenrod",
            "green","indigo","limegreen","orange", "mediumslateblue","orchid","seagreen","sienna","teal" };
        };
    }
}

#endif // CHARTMODEL_H
