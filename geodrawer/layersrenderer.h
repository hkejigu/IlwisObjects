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

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef LOGORENDERER_H
#define LOGORENDERER_H

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QQuickFramebufferObject>

#include <QTime>
#include <QVector>

#include "geometries.h"

namespace Ilwis {
class Resource;

namespace Ui{
class LayerManager;


class LayersRenderer : public QObject, public QQuickFramebufferObject::Renderer, public QOpenGLFunctions {

    Q_OBJECT
public:
    LayersRenderer(const QQuickFramebufferObject *fbo, bool active);
    ~LayersRenderer();

    void render();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        _fbo = new QOpenGLFramebufferObject(size, format);

        return _fbo;
    }

    void synchronize(QQuickFramebufferObject *item);

    void cleanup();

signals:
    void synchronizeDone();
    void drawDone();
    void addDataSource(const QUrl& url, quint64 tp, Ilwis::Geodrawer::DrawerInterface *drawer);

private:
    LayerManager *_rootDrawer = 0;
    QSize _viewPortSize;
    QSize _windowSize;
    QPointF _originInLocalCS;
    QPointF _originInWindowCS;
    Ilwis::Envelope _zoomEnvelope;
    QOpenGLFramebufferObject *_fbo=0;
    QString _saveImagePath;
    std::unique_ptr<QOpenGLContext> _oglContext;

    void handleRendererAttributes(const QString &code, const QVariant &value);
    void saveAsImage() const;
private slots:
    void updateRenderer();
};
#endif
