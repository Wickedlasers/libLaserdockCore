//
// Created by Darren Otgaar on 2018/08/19.
//

#include "ldLaserFont.h"

using namespace ldRazer;

/*
 * NOTE:  Each char is defined in local space of 30 x 50 pixels.  Upper case = Lower case
 */

const static size_t INVALID_CHAR = size_t(-1);
const static vec2f char_dims = {30.f, 50.f};

// Lower case (65 - 90), Upper case (97 - 122), Digits (48 - 57), 46 (.), 90 ([), 93 (])
static std::vector<std::vector<vec2f>> chars = {
        {{0,  0},  {0,  35}, {0,  35}, {15, 50}, {15, 50}, {30, 35}, {30, 35}, {30, 0},  {0,  20}, {30, 20}},             // A
        {{0,  0},  {0,  50}, {0,  50}, {25, 50}, {25, 50}, {30, 42}, {30, 42}, {30, 33}, {30, 33}, {25, 25}, {25, 25},
                                                                                                                       {0,  25}, {25, 25}, {30, 17}, {30, 17}, {30, 8}, {30, 8}, {25, 0}, {25, 0}, {0, 0}},                      // B
        {{30, 0},  {0,  0},  {0,  0},  {0,  50}, {0,  50}, {30, 50}},                                                  // C
        {{0,  0},  {0,  50}, {0,  50}, {10, 50}, {10, 50}, {30, 35}, {30, 35}, {30, 15}, {30, 15}, {10, 0},  {10, 0},      // D
                                                                                                                       {0,  0}},
        {{30, 0},  {0,  0},  {0,  0},  {0,  50}, {0,  50}, {30, 50}, {0,  25}, {25, 25}},                                 // E
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {0,  25}, {25, 25}},                                                // F
        {{15, 15}, {30, 15}, {30, 15}, {30, 0},  {30, 0},  {0,  0},  {0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50},
                                                                                                                       {30, 30}},                                                                                        // G
        {{0,  0},  {0,  50}, {30, 0},  {30, 50}, {0,  25}, {30, 25}},                                                // H
        {{0,  0},  {30, 0},  {15, 0},  {15, 50}, {0,  50}, {30, 50}},                                                // I
        {{0,  15}, {15, 0},  {15, 0},  {30, 0},  {30, 0},  {30, 50}},                                                // J
        {{0,  0},  {0,  50}, {30, 50}, {0,  25}, {0,  25}, {30, 0}},                                                 // K
        {{30, 0},  {0,  0},  {0,  0},  {0,  50}},                                                                   // L
        {{0,  0},  {0,  50}, {0,  50}, {15, 40}, {15, 40}, {30, 50}, {30, 50}, {30, 0}},                              // M
        {{0,  0},  {0,  50}, {0,  50}, {30, 0},  {30, 0},  {30, 50}},                                                 // N
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50}, {30, 0},  {30, 0},  {0,  0}},                                 // O
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50}, {30, 25}, {30, 25}, {0,  25}},                              // P
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50}, {30, 15}, {30, 15}, {15, 0},  {15, 0},  {0,  0},  {15, 15},
                                                                                                                       {30, 0}},                                                                                         // Q
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50}, {30, 25}, {30, 25}, {0,  25}, {0,  25}, {30, 0}},              // R
        {{0,  0},  {30, 0},  {30, 0},  {30, 25}, {30, 25}, {0,  25}, {0,  25}, {0,  50}, {0,  50}, {30, 50}},               // S
        {{15, 0},  {15, 50}, {0,  50}, {30, 50}},                                                               // T
        {{0,  50}, {0,  0},  {0,  0},  {30, 0},  {30, 0},  {30, 50}},                                                  // U
        {{0,  50}, {15, 0},  {15, 0},  {30, 50}},                                                                // V
        {{0,  50}, {0,  0},  {0,  0},  {15, 15}, {15, 15}, {30, 0},  {30, 0},  {30, 50}},                                // W
        {{0,  50}, {30, 0},  {0,  0},  {30, 50}},                                                                 // X
        {{0,  50}, {15, 35}, {15, 35}, {30, 50}, {15, 35}, {15, 0}},                                             // Y
        {{0,  50}, {30, 50}, {30, 50}, {0,  0},  {0,  0},  {30, 0}},                                                 // Z
        {{0,  0},  {0,  50}, {0,  50}, {30, 50}, {30, 50}, {30, 0},  {30, 0},  {0,  0},  {30, 35}, {0,  15}},                // 0
        {{15, 0},  {15, 50}},                                                                               // 1
        {{0,  50}, {30, 50}, {30, 50}, {30, 25}, {30, 25}, {0,  25}, {0,  25}, {0,  0},  {0,  0},  {30, 0}},               // 2
        {{0,  0},  {30, 0},  {30, 0},  {30, 50}, {30, 50}, {0,  50}, {0,  25}, {30, 25}},                               // 3
        {{0,  50}, {0,  25}, {0,  25}, {30, 25}, {30, 50}, {30, 0}},                                               // 4
        {{0,  0},  {30, 0},  {30, 0},  {30, 25}, {30, 25}, {0,  25}, {0,  25}, {0,  50}, {0,  50}, {30, 50}},               // 5
        {{0,  50}, {0,  0},  {0,  0},  {30, 0},  {30, 0},  {30, 25}, {30, 25}, {0,  25}},                                 // 6
        {{0,  50}, {30, 50}, {30, 50}, {30, 0}},                                                               // 7
        {{0,  0},  {30, 0},  {30, 0},  {30, 25}, {30, 25}, {0,  25}, {0,  25}, {0,  50}, {0,  50}, {30, 50},
                                                                                                             {30, 50}, {30, 25}, {0,  0},  {0,  25}},                                                                // 8
        {{30, 0},  {30, 50}, {30, 50}, {0,  50}, {0,  50}, {0,  25}, {0,  25}, {30, 25}},                              // 9
        {{13, 0},  {14, 0},  {13, 1},  {14, 0}},                                                                 // .
        {{30, 50}, {13, 50}, {13, 50}, {13, 0},  {13, 0},  {30, 0}},                                           // [
        {{0,  50}, {14, 50}, {14, 50}, {14, 0},  {14, 0},  {0,  0}}                                              // ]
};


ldLaserFont::ldLaserFont() : charSpacing(10), m_scale(.5f), m_invScale(2.f)
{
}

ldLaserFont::~ldLaserFont() = default;

size_t char_to_idx(char ch)
{
    if (ch >= 48 && ch <= 57) return size_t(ch - 48 + 26);
    else if (ch >= 65 && ch <= 90) return size_t(ch - 65);
    else if (ch >= 97 && ch <= 122) return size_t(ch - 97);
    else if (ch == 46) return size_t(chars.size() - 3);
    else if (ch == 91) return size_t(chars.size() - 2);
    else if (ch == 93) return size_t(chars.size() - 1);
    else return size_t(-1);
}

ldMesh2f_t ldLaserFont::buildString(const vec2f &P, const QString &str, uint32_t colour, bool strict)
{
    const auto horz_spacing = vec2f(char_dims[0] + charSpacing, 0);
    auto lP = P;

    linecolf_arr_t vertices;

    for (auto &ch : str) {
        auto idx = char_to_idx(ch.toLatin1());
        if (idx == INVALID_CHAR) lP += m_scale * horz_spacing;
        else {
            for (size_t i = 0; i != chars[idx].size() / 2; ++i) {
                auto chidx = 2 * i;
                vertices.emplace_back(
                        std::make_pair(
                                line2f_t(m_scale * chars[idx][chidx] + lP,
                                         m_scale * chars[idx][chidx + 1] + lP), colour));
            }
            lP += m_scale * horz_spacing;
        }
    }

    ldMesh2f_t mesh;
    mergeSegments(vertices, mesh, strict);
    return mesh;
}

size_t
ldLaserFont::buildString(ldMesh2f_t &mesh, const ldRazer::vec2f &P, const QString &str, uint32_t colour, bool strict)
{
    const auto horz_spacing = vec2f(char_dims[0] + charSpacing, 0);
    auto lP = P;

    linecolf_arr_t vertices;

    for (auto &ch : str) {
        auto idx = char_to_idx(ch.toLatin1());
        if (idx == INVALID_CHAR) lP += m_scale * horz_spacing;
        else {
            for (size_t i = 0; i != chars[idx].size() / 2; ++i) {
                auto chidx = 2 * i;
                vertices.emplace_back(
                        std::make_pair(
                                line2f_t(m_scale * chars[idx][chidx] + lP,
                                         m_scale * chars[idx][chidx + 1] + lP), colour));
            }
            lP += m_scale * horz_spacing;
        }
    }

    return mergeSegments(vertices, mesh, strict);
}

AABB2f_t ldLaserFont::getStringAABB(const QString &str)
{
    return AABB2f_t(vec2f(0.f, 0.f),
                    vec2f(m_scale[0] * str.length() * (char_dims.x + charSpacing) - m_scale[0] * charSpacing,
                          m_scale[1] * char_dims.y));
}