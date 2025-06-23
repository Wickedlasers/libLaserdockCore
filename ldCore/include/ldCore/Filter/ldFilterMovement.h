//
//  ldFilterMovement.h
//
//  Created by Eric Brugère on 01/14/22.
//  Copyright (c) 2022 Wicked Lasers. All rights reserved.
//

#ifndef LDFILTERMOVEMENT_H
#define LDFILTERMOVEMENT_H

#include "ldCore/Filter/ldFilter.h"

class LDCORESHARED_EXPORT ldFilterMovement : public ldFilter
{
    Q_OBJECT
    LD_WRITABLE_MIN_MAX_PROPERTY(float, horizontal)
    LD_WRITABLE_MIN_MAX_PROPERTY(float, vertical)
    QML_WRITABLE_PROPERTY(bool, mirror)
    LD_WRITABLE_MIN_MAX_PROPERTY(float, rotateX)
    LD_WRITABLE_MIN_MAX_PROPERTY(float, rotateY)
    LD_WRITABLE_MIN_MAX_PROPERTY(float, rotateZ)
    QML_READONLY_PROPERTY(bool,isMovementFilterActive) // set to false only when all params are zero (movement filter is bypassed)
public:
    ldFilterMovement();

    virtual void process(ldVertex &v) override;
    virtual QString name() override { return QObject::tr("Movement"); }

    bool m_enabled = true;
private:

    bool checkIsMovementFilterActive();
    float m_decay_x;
    float m_decay_y;
    float m_decay_rx;
    float m_decay_ry;
    float m_decay_rz;
};

#endif // FILTERMOVEMENT_H

