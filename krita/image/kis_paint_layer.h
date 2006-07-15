/*
 *  Copyright (c) 2005 Casper Boemann <cbr@boemann.dk>
 *
 *  this program is free software; you can redistribute it and/or modify
 *  it under the terms of the gnu general public license as published by
 *  the free software foundation; either version 2 of the license, or
 *  (at your option) any later version.
 *
 *  this program is distributed in the hope that it will be useful,
 *  but without any warranty; without even the implied warranty of
 *  merchantability or fitness for a particular purpose.  see the
 *  gnu general public license for more details.
 *
 *  you should have received a copy of the gnu general public license
 *  along with this program; if not, write to the free software
 *  foundation, inc., 675 mass ave, cambridge, ma 02139, usa.
 */
#ifndef KIS_PAINT_LAYER_H_
#define KIS_PAINT_LAYER_H_

#include "kis_types.h"
#include "kis_layer.h"
#include "kis_paint_device.h"
#include "KoColorSpace.h"
/**
 * This layer is of a type that can be painted on.
 */
class KRITAIMAGE_EXPORT KisPaintLayer : public KisLayer {
    typedef KisLayer super;

    Q_OBJECT

public:
    KisPaintLayer(KisImage *img, const QString& name, quint8 opacity, KisPaintDeviceSP dev);
    KisPaintLayer(KisImage *img, const QString& name, quint8 opacity);
    KisPaintLayer(KisImage *img, const QString& name, quint8 opacity, KoColorSpace * colorSpace);
    KisPaintLayer(const KisPaintLayer& rhs);
    virtual ~KisPaintLayer();

    virtual QIcon icon() const;
    virtual PropertyList properties() const;
    virtual KisLayerSP clone() const;
public:

    virtual qint32 x() const;
    virtual void setX(qint32 x);

    virtual qint32 y() const;
    virtual void setY(qint32 y);

    virtual QRect extent() const;
    virtual QRect exactBounds() const;

    virtual void paintSelection(QImage &img, qint32 x, qint32 y, qint32 w, qint32 h);
    virtual void paintSelection(QImage &img, const QRect& scaledImageRect, const QSize& scaledImageSize, const QSize& imageSize);

    virtual void paintMaskInactiveLayers(QImage &img, qint32 x, qint32 y, qint32 w, qint32 h);

    virtual QImage createThumbnail(qint32 w, qint32 h);

    virtual bool accept(KisLayerVisitor &v)
        {
//            kDebug(41001) << "\tPAINT\t" << name()
//                    << " dirty: " << dirty() << "\n";
            return v.visit(this);
        };


    /// Returns the paintDevice that accompanies this layer
    inline KisPaintDeviceSP paintDevice() const { return m_paintdev; };

private slots:
    void slotColorSpaceChanged();

private:
    void init();
    KisPaintDeviceSP m_paintdev;
};

#endif // KIS_PAINT_LAYER_H_

