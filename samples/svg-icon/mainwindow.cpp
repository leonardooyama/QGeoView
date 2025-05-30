/***************************************************************************
 * QGeoView is a Qt / C ++ widget for visualizing geographic data.
 * Copyright (C) 2018-2025 Andrey Yaroshenko.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see https://www.gnu.org/licenses.
 ****************************************************************************/

#include "mainwindow.h"

#include <QApplication>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QTimer>
#include <QTreeWidgetItem>

#include <QGeoView/QGVLayerBDGEx.h>
#include <QGeoView/QGVLayerGoogle.h>
#include <QGeoView/Raster/QGVIcon.h>
#include <helpers.h>
#include <CustomSVGSymbol.h>
#include <MovableIcon.h>
#include <rectangle.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("QGeoView Samples - drag and drop");
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(new QHBoxLayout());

    // setup tree widget
    sampleTree = new QTreeWidget(this);
    sampleTree->setDragEnabled(true); // enable dragging items from the tree
    sampleTree->setMinimumWidth(200);
    sampleTree->setMaximumWidth(250);

    // setup data into the tree widget
    sampleTree->setColumnCount(1);
    sampleTree->setHeaderLabel("Sample Tree Items with Icons");

    // QTreeWidgetItem* itemArrowUp = new QTreeWidgetItem;
    // QIcon arrowUp_icon = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
    // itemArrowUp->setData(0, Qt::DisplayRole, "Arrow Up");
    // itemArrowUp->setData(0, Qt::DecorationRole, arrowUp_icon);
    // sampleTree->addTopLevelItem(itemArrowUp);

    // QTreeWidgetItem* itemArrowDown = new QTreeWidgetItem;
    // QIcon arrowDown_icon = QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
    // itemArrowDown->setData(0, Qt::DisplayRole, "Arrow Down");
    // itemArrowDown->setData(0, Qt::DecorationRole, arrowDown_icon);
    // sampleTree->addTopLevelItem(itemArrowDown);

    // QTreeWidgetItem* itemArrowRight = new QTreeWidgetItem;
    // QIcon arrowRight_icon = QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
    // itemArrowRight->setData(0, Qt::DisplayRole, "Arrow Right");
    // itemArrowRight->setData(0, Qt::DecorationRole, arrowRight_icon);
    // sampleTree->addTopLevelItem(itemArrowRight);

    // QTreeWidgetItem* itemArrowLeft = new QTreeWidgetItem;
    // QIcon arrowLeft_icon = QApplication::style()->standardIcon(QStyle::SP_ArrowLeft);
    // itemArrowLeft->setData(0, Qt::DisplayRole, "Arrow Left");
    // itemArrowLeft->setData(0, Qt::DecorationRole, arrowLeft_icon);
    // sampleTree->addTopLevelItem(itemArrowLeft);

    QTreeWidgetItem* itemSvg = new QTreeWidgetItem;
    QIcon svg_icon(":/images/star.svg");
    itemSvg->setData(0, Qt::DisplayRole, "SVG");
    itemSvg->setData(0, Qt::DecorationRole, svg_icon);
    sampleTree->addTopLevelItem(itemSvg);

    centralWidget()->layout()->addWidget(sampleTree);

    // QGeoView setup
    Helpers::setupCachedNetworkAccessManager(this);
    mMap = new QGVMap(this);



    QGVLayer* layerGoogleSchema = new QGVLayerGoogle(QGV::TilesType::Schema);
    layerGoogleSchema->setName("Google Schema");
    mMap->addItem(layerGoogleSchema);

    // QGVLayerBDGEx *m_bdgexLayer50k = new QGVLayerBDGEx(QGV::BDGExLayer::ctm50);
    // m_bdgexLayer50k->setName("BDGEx 50k");
    // mMap->addItem(m_bdgexLayer50k);

    // initial area to show
    QTimer::singleShot(500, this, &MainWindow::targetArea);
    centralWidget()->layout()->addWidget(mMap); // add map to central widget

    // handle drop of itens on map
    connect(mMap, &QGVMap::dropOnMap, this, &MainWindow::handleDropOnMap);
    iconsLayer = new QGVLayer();
    mMap->addItem(iconsLayer);
}

MainWindow::~MainWindow()
{
}

void MainWindow::targetArea()
{
    QGV::GeoPos geoPos(-23.55651916354534, -46.61844717963553);
    auto scaleRect = QGV::GeoRect{
        geoPos.latitude() - 0.33, geoPos.longitude() - 0.33, geoPos.latitude() + 0.33, geoPos.longitude() + 0.33
    };
    mMap->cameraTo(QGVCameraActions(mMap).scaleTo(scaleRect));
    return;
}

void MainWindow::handleDropOnMap(QGV::GeoPos pos, const QMimeData* data)
{
    QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QIcon iconDropped;
    QString strDropped;
    while (!stream.atEnd()) {
        int row, col;
        QMap<int, QVariant> roleDataMap;
        stream >> row >> col >> roleDataMap;
        for (auto i = 0; i < roleDataMap.size(); i++) {
            std::string typeName = roleDataMap[i].typeName();
            auto variant = roleDataMap[i];
            if (typeName == "QString") {
                strDropped = qvariant_cast<QString>(variant);
            }
            if (typeName == "QIcon") {
                iconDropped = qvariant_cast<QIcon>(variant);
            }
        }
    }
    qDebug() << strDropped << "(lat, lon):" << pos.latToString() << pos.lonToString();
    if (!iconDropped.isNull() && !strDropped.isEmpty()) {
        // ######################################## //
        // ############### new code ############### //
        // ######################################## //
        //QImage m_Image;
        //m_Image.load(":/images/star.svg");
        // auto *m_CustomSVGSymbol = new CustomSVGSymbol(mMap, pos, m_Image.size());
        // m_Image.load(":/images/star.svg");
        // if(!m_Image.isNull())
        // {
        //     qDebug() << "loading image...";
        //     m_CustomSVGSymbol->loadImage(m_Image);
        // }
        // else
        // {
        //     qDebug() << "from main: image is null";
        // }
        // m_CustomSVGSymbol->setRect(mMap, pos, m_Image.size());
        // iconsLayer->addItem(m_CustomSVGSymbol);
        auto pixmap = iconDropped.pixmap(QSize(32, 32));
        auto image = pixmap.toImage();
        auto *m_MovableIcon = new MovableIcon();
        m_MovableIcon->loadImage(image);
        m_MovableIcon->setGeometry(pos, QSizeF(32, 32));
        iconsLayer->addItem(m_MovableIcon);
    }
}
