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

#pragma once

#include <QGeoView/QGVDrawItem.h>
#include <QGeoView/QGVMap.h>

#include <QBrush>
#include <QPen>

class CustomSVGSymbol : public QGVDrawItem
{
    Q_OBJECT

public:
    explicit CustomSVGSymbol(const QGV::GeoRect& geoRect, QColor color);
    explicit CustomSVGSymbol(QGVMap* geoMap, const QGV::GeoPos& geoPos, QSizeF size);

    void setRect(const QGV::GeoRect& geoRect);
    void setRect(QGVMap* geoMap, const QGV::GeoPos& geoPos, QSizeF size);
    QGV::GeoRect getRect() const;

    QImage getImage() const;
    bool isImageNull() const;

    void loadImage(const QByteArray& rawData);
    void loadImage(const QImage& image);

private:
    void onProjection(QGVMap* geoMap) override;
    QPainterPath projShape() const override;
    void projPaint(QPainter* painter) override;
    QPointF projAnchor() const override;
    QTransform projTransform() const override;
    QString projTooltip(const QPointF& projPos) const override;
    void projOnMouseClick(const QPointF& projPos) override;
    void projOnMouseDoubleClick(const QPointF& projPos) override;
    void projOnObjectStartMove(const QPointF& projPos) override;
    void projOnObjectMovePos(const QPointF& projPos) override;
    void projOnObjectStopMove(const QPointF& projPos) override;

private:
    QGV::GeoRect mGeoRect;
    QRectF mProjRect;
    QColor mColor;

    QGV::GeoPos mGeoPos;
    QSizeF mSize;
    QImage mImage;
};
