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

#include "CustomSVGSymbol.h"

#include <QBrush>
#include <QPainter>
#include <QPen>

CustomSVGSymbol::CustomSVGSymbol(const QGV::GeoRect& geoRect, QColor color)
    : mGeoRect(geoRect)
    , mColor(color)
{
}

CustomSVGSymbol::CustomSVGSymbol(QGVMap *geoMap, const QGV::GeoPos &geoPos, QSizeF size)
    :mGeoPos(geoPos)
    , mSize(size)
{
    setFlag(QGV::ItemFlag::Movable);
    mColor = Qt::blue;

    if (geoMap == nullptr)
    {
        qDebug() << "Map is null";
        return;
    }
    QPointF anchorPoint = geoMap->getProjection()->geoToProj(geoPos);
    QPointF bottomLeft(anchorPoint.rx() - size.rwidth()/2, anchorPoint.ry());
    QPointF topRight(anchorPoint.rx() + size.rwidth()/2, anchorPoint.ry() - size.rheight());
    QGV::GeoPos geoBottomLeft (geoMap->getProjection()->projToGeo(bottomLeft));
    QGV::GeoPos geoTopRight (geoMap->getProjection()->projToGeo(topRight));
    mGeoRect = QGV::GeoRect(geoBottomLeft, geoTopRight);
}

void CustomSVGSymbol::setRect(const QGV::GeoRect& geoRect)
{
    mGeoRect = geoRect;

    // Geo coordinates need to be converted manually again to projection
    onProjection(getMap());

    // Now we can inform QGV about changes for this
    resetBoundary();
    refresh();
}

void CustomSVGSymbol::setRect(QGVMap *geoMap, const QGV::GeoPos& geoPos, QSizeF size)
{
    if (geoMap == nullptr)
    {
        qDebug() << "Map is null";
        return;
    }
    QPointF anchorPoint = geoMap->getProjection()->geoToProj(geoPos);
    QPointF bottomLeft(anchorPoint.rx() - size.rwidth()/2, anchorPoint.ry());
    QPointF topRight(anchorPoint.rx() + size.rwidth()/2, anchorPoint.ry() - size.rheight());
    QGV::GeoPos geoBottomLeft (geoMap->getProjection()->projToGeo(bottomLeft));
    QGV::GeoPos geoTopRight (geoMap->getProjection()->projToGeo(topRight));
    mGeoRect = QGV::GeoRect(geoBottomLeft, geoTopRight);
    mColor = Qt::green;
    // Geo coordinates need to be converted manually again to projection
    onProjection(geoMap);
    // Now we can inform QGV about changes for this
    resetBoundary();
    refresh();
}

QGV::GeoRect CustomSVGSymbol::getRect() const
{
    return mGeoRect;
}

QImage CustomSVGSymbol::getImage() const
{
    return mImage;
}

bool CustomSVGSymbol::isImageNull() const
{
    return mImage.isNull();
}

void CustomSVGSymbol::loadImage(const QByteArray &rawData)
{
    QImage image;
    image.loadFromData(rawData);
    loadImage(image);
}

void CustomSVGSymbol::loadImage(const QImage &image)
{
    mImage = image;
    qDebug() << "Is mImage null? Answer:" << mImage.isNull();
    resetBoundary();
    refresh();
}

void CustomSVGSymbol::onProjection(QGVMap* geoMap)
{
    QGVDrawItem::onProjection(geoMap);
    mProjRect = geoMap->getProjection()->geoToProj(mGeoRect);
}

QPainterPath CustomSVGSymbol::projShape() const
{
    QPainterPath path;
    path.addRect(mProjRect);
    return path;
}

void CustomSVGSymbol::projPaint(QPainter* painter)
{
    if (!mImage.isNull())
    {
        painter->drawImage(mProjRect, mImage);
    }
    else
    {
        qDebug() << "Image is null";
        QPen pen = QPen(QBrush(Qt::black), 1);

               // Custom item highlight indicator
        if (isFlag(QGV::ItemFlag::Highlighted) && isFlag(QGV::ItemFlag::HighlightCustom)) {
            // We will use pen with bigger width
            pen = QPen(QBrush(Qt::black), 5);
        }

        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(QBrush(mColor));
        painter->drawRect(mProjRect);
    }

    // Custom item select indicator
    if (isSelected() && isFlag(QGV::ItemFlag::SelectCustom)) {
        // We will draw additional rect around our item
        painter->drawLine(mProjRect.topLeft(), mProjRect.bottomRight());
        painter->drawLine(mProjRect.topRight(), mProjRect.bottomLeft());
    }
}

QPointF CustomSVGSymbol::projAnchor() const
{
    // This method is optional (needed flag is QGV::ItemFlag::Transformed).
    // In this case we will use center of item as base

    return mProjRect.center();
}

QTransform CustomSVGSymbol::projTransform() const
{
    // This method is optional (needed flag is QGV::ItemFlag::Transformed).
    // Custom transformation for item.
    // In this case we rotate item by 45 degree.

    return QGV::createTransfromAzimuth(projAnchor(), 45);
}

QString CustomSVGSymbol::projTooltip(const QPointF& projPos) const
{
    // This method is optional (when empty return then no tooltip).
    // Text for mouse tool tip.

    auto geo = getMap()->getProjection()->projToGeo(projPos);

    return "Rectangle with color " + mColor.name() + "\nPosition " + geo.latToString() + " " + geo.lonToString();
}

void CustomSVGSymbol::projOnMouseClick(const QPointF& projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Clickable).
    // Custom reaction to item single mouse click.
    // To avoid collision with item selection this code applies only if item selection disabled.
    // In this case we change opacity for item.

    if (!isSelectable()) {
        if (getOpacity() <= 0.5)
            setOpacity(1.0);
        else
            setOpacity(0.5);

        qInfo() << "single click" << projPos;
    } else {
        setOpacity(1.0);
    }
}

void CustomSVGSymbol::projOnMouseDoubleClick(const QPointF& projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Clickable).
    // Custom reaction to item double mouse click.
    // In this case we change color for item.

    const QList<QColor> colors = { Qt::red, Qt::blue, Qt::green, Qt::gray, Qt::cyan, Qt::magenta, Qt::yellow };

    const auto iter =
            std::find_if(colors.begin(), colors.end(), [this](const QColor& color) { return color == mColor; });
    mColor = colors[(iter - colors.begin() + 1) % colors.size()];
    repaint();

    setOpacity(1.0);

    qInfo() << "double click" << projPos;
}

void CustomSVGSymbol::projOnObjectStartMove(const QPointF& projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move start.
    // In this case we only log message.

    qInfo() << "object move started at" << projPos;
}

void CustomSVGSymbol::projOnObjectMovePos(const QPointF& projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to mouse pos change when item move is started.
    // In this case actually changing location of object.

    auto newRect = mProjRect;

    QPointF mprojPos(projPos.x(), projPos.y() - mProjRect.height()/2);
    newRect.moveCenter(mprojPos);
    setRect(getMap()->getProjection()->projToGeo(newRect));

    qInfo() << "object moved" << mGeoRect;
}

void CustomSVGSymbol::projOnObjectStopMove(const QPointF& projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move finished.
    // In this case we only log message.
    qInfo() << "object move stopped" << projPos;
}
