//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "ldGradientObject.h"
#include <QtCore/QDebug>

using namespace std;


// Create the gradient object, setting spacing between gradient samples
ldGradientObject::ldGradientObject(float spacing)
{
    m_spacing = spacing;
}


// Start a new model
void ldGradientObject::addMesh()
{
    TMesh new_vertex_list;
    m_mesh.push_back(new_vertex_list);
    m_current_mesh = m_mesh.size()-1;
}


// Add a new vertex to the object,
// interpolating colors using spacing
void ldGradientObject::addVertex(ldVec2 pos, uint32_t color, bool is_last)
{
    // first vertex is just added as is
    if (m_mesh[m_current_mesh].vertices.size() == 0)
    {
        m_mesh[m_current_mesh].vertices.push_back(pos);
        m_mesh[m_current_mesh].colors.push_back(color);
        return;
    }

    // interpolate
    ldVec2 prev_vertex = m_mesh[m_current_mesh].vertices[m_mesh[m_current_mesh].vertices.size()-1];
    uint32_t prev_color = m_mesh[m_current_mesh].colors[m_mesh[m_current_mesh].colors.size()-1];
    interpolate(prev_vertex, prev_color, pos, color);

    if (is_last)
    {
        // also interpolate to the first
        // interpolate(pos, color, m_vertices[0], m_colors[0]);
        interpolate(m_mesh[m_current_mesh].vertices[m_mesh[m_current_mesh].vertices.size()-1], m_mesh[m_current_mesh].colors[m_mesh[m_current_mesh].colors.size()-1], m_mesh[m_current_mesh].vertices[0], m_mesh[m_current_mesh].colors[0]);
    }
}


// Add a new vertex to the object,
// interpolating colors using spacing
// and using relative coordinates (last added vertex is the origin reference)
void ldGradientObject::addRelativeVertex(ldVec2 pos, uint32_t color, bool is_last)
{
    ldVec2 prev_vertex = m_mesh[m_current_mesh].vertices[m_mesh[m_current_mesh].vertices.size()-1];
    addVertex(prev_vertex+pos, color, is_last);
}


// Render the gradient object
// the renderer must have been started and transformed already.
void ldGradientObject::renderGradient(ldRendererOpenlase* p_renderer, int repeats)
{
    for (int m=0; m<m_mesh.size(); m++)
    {
        p_renderer->begin(OL_LINESTRIP);
        for (int i=0; i<m_mesh[m].vertices.size(); i++)
        {
            p_renderer->vertex(m_mesh[m].vertices[i].x, m_mesh[m].vertices[i].y, m_mesh[m].colors[i], repeats);
        }
        p_renderer->end();
    }
}


// Render the gradient object *and* cycles colors, making it really really colorful
// the renderer must have been started and transformed already.
void ldGradientObject::renderGradientCyclic(ldRendererOpenlase* p_renderer, int repeats)
{
    for (int m=0; m<m_mesh.size(); m++)
    {
        m_mesh[m].frame = (m_mesh[m].frame + 1) % m_mesh[m].vertices.size();
        unsigned int idx = m_mesh[m].frame;

        p_renderer->begin(OL_LINESTRIP);
        for (int i=0; i<m_mesh[m].vertices.size(); i++)
        {
            p_renderer->vertex(m_mesh[m].vertices[i].x, m_mesh[m].vertices[i].y, m_mesh[m].colors[idx], repeats);
            idx = (idx + 1) % m_mesh[m].vertices.size();
        }
        p_renderer->end();
    }
}


// Interpolate between two vertices
void ldGradientObject::interpolate(ldVec2& start_pos, uint32_t start_color, ldVec2& target_pos, uint32_t target_color)
{
    for (float t = m_spacing; t <= 1; t += m_spacing)
    {
        float px = start_pos.x + (target_pos.x - start_pos.x) * t;
        float py = start_pos.y + (target_pos.y - start_pos.y) * t;
        ldVec2 new_pos(px, py);
        m_mesh[m_current_mesh].vertices.push_back(new_pos);

        uint32_t new_color = (uint32_t)(ldColorUtil::lerpInt(start_color, target_color, t));
        m_mesh[m_current_mesh].colors.push_back(new_color);
    }
}


// adjust color brightness
uint32_t ldGradientObject::brightness(uint32_t color, float brightness)
{
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    r = (int)(r * brightness);
    g = (int)(g * brightness);
    b = (int)(b * brightness);

    return ((r<<16) | (g<<8) | b);
}
