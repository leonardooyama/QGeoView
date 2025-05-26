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

#include "MovableIcon.h"
#include "QGVMap.h"

#include <QPainter>

MovableIcon::MovableIcon()
{
    setFlag(QGV::ItemFlag::IgnoreScale);
    setFlag(QGV::ItemFlag::IgnoreAzimuth);
    setFlag(QGV::ItemFlag::Movable);
}

void MovableIcon::setGeometry(const QGV::GeoPos& geoPos, const QSizeF& imageSize)
{
    mGeoPos = geoPos;
    mProjPos = {};
    mImageSize = imageSize;
    mProjRect = {};
    calculateGeometry();
}

void MovableIcon::setGeometry(const QPointF& projPos, const QSizeF& imageSize)
{
    mGeoPos = {};
    mProjPos = projPos;
    mImageSize = imageSize;
    mProjRect = {};
    calculateGeometry();
}

QImage MovableIcon::getImage() const
{
    return mImage;
}

bool MovableIcon::isImage() const
{
    return !mImage.isNull();
}

void MovableIcon::loadImage(const QByteArray& rawData)
{
    QImage image;
    image.loadFromData(rawData);
    loadImage(image);
}

void MovableIcon::loadImage(const QImage& image)
{
    mImage = image;
    calculateGeometry();
}

void MovableIcon::onProjection(QGVMap* geoMap)
{
    QGVDrawItem::onProjection(geoMap);
    calculateGeometry();
}

QPainterPath MovableIcon::projShape() const
{
    QPainterPath path;
    path.addRect(mProjRect);
    return path;
}

void MovableIcon::projPaint(QPainter* painter)
{
    if (mImage.isNull() || mProjRect.isEmpty()) {
        return;
    }

    QRectF paintRect = mProjRect;

    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(paintRect, getImage());
}

void MovableIcon::projOnObjectStartMove(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move finished.
    // In this case we only log message.
    qInfo() << "object move started" << projPos;
}

void MovableIcon::projOnObjectMovePos(const QPointF &projPos)
{
    // auto newRect = mProjRect;

    // QPointF mprojPos(projPos.x(), projPos.y() - mProjRect.height()/2);
    // newRect.moveCenter(mprojPos);
    mGeoPos = getMap()->getProjection()->projToGeo(projPos);
    calculateGeometry();
}

void MovableIcon::projOnObjectStopMove(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move finished.
    // In this case we only log message.
    qInfo() << "object move stopped" << projPos;
}

void MovableIcon::calculateGeometry()
{
    if (getMap() == nullptr) {
        return;
    }

    if (!mGeoPos.isEmpty()) {
        mProjPos = getMap()->getProjection()->geoToProj(mGeoPos);
    }

    const QSizeF baseSize = !mImageSize.isEmpty() ? mImageSize : mImage.size();
    const QPointF baseAnchor = QPointF(baseSize.width() / 2, baseSize.height() / 2);
    mProjRect = QRectF(mProjPos - baseAnchor, baseSize);

    resetBoundary();
    refresh();
}
