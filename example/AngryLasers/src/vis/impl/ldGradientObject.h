//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef ldGradientObject_H
#define ldGradientObject_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include <ldCore/Helpers/Color/ldColorUtil.h>


class ldGradientObject
{
public:

    ldGradientObject(float spacing);
    void renderGradient(ldRendererOpenlase* p_renderer, int repeats);
    void renderGradientCyclic(ldRendererOpenlase* p_renderer, int repeats);

    typedef struct
    {
        QList<ldVec2> vertices;
        QList<uint32_t> colors;
        unsigned int frame;
    } TMesh;

    void addMesh();
    void addVertex(ldVec2 pos, uint32_t color, bool is_last = false);
    void addRelativeVertex(ldVec2 pos, uint32_t color, bool is_last = false);
    uint32_t brightness(uint32_t color, float brightness);

protected:

    float m_spacing;
    int m_current_mesh;

    void interpolate(ldVec2& start_pos, uint32_t start_color, ldVec2& target_pos, uint32_t target_color);

    QList<TMesh> m_mesh;
};

#endif // ldGradientObject_H
