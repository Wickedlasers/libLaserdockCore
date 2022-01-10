//
// Created by Darren Otgaar on 2018/09/23.
//

#include "ldWorld.h"

namespace ldRazer
{

    ldWorld::ldWorld() = default;

    ldWorld::~ldWorld()
    {
        for (auto &b : m_scene) {
            b.clear();
        }
    }

    void ldWorld::clear()
    {
        m_physicsEngine->clear();
        m_scene.clear();
        m_navMeshes.clear();
    }

    bool ldWorld::initialise(ldPhysicsEngine *enginePtr)
    {
        m_physicsEngine = enginePtr;
        return true;
    }

    const AABB2f_t &ldWorld::worldBound() const
    {
        return m_worldBound;
    }

    void ldWorld::loadQuadTree()
    {
        std::vector<ldBody *> bodies;
        for (auto &b : m_scene) {
            if (b.isStatic()) bodies.emplace_back(b.bodyPtr);
        }

        m_physicsEngine->createTree(bodies);
    }

    uint16_t ldWorld::addStaticObject(const ldBound &bound, uint32_t colour, float orn)
    {
        if (!m_physicsEngine) return uint16_t(-1);

        if (bound.isHull()) {
            auto copy = bound;
            vec2f centre{0.f, 0.f};
            for (const auto &v : bound.hull.vertices) centre += v;
            centre /= bound.hull.vertices.size();
            for (auto &v : copy.hull.vertices) v -= centre;
            ldBody b;
            b.setBody(centre, orn, 0.f, copy);
            auto bodyId = m_physicsEngine->addBody(b);
            auto id = m_scene.size();
            if (colour != 0x00000000)
                m_scene.emplace_back(
                        WorldObject(WorldObject::WO_STATIC, m_physicsEngine->getBody(bodyId), true, colour));
            else
                m_scene.emplace_back(WorldObject(WorldObject::WO_STATIC, m_physicsEngine->getBody(bodyId)));
            return uint16_t(id);
        } else if (bound.isMesh()) {
            assert(false && "TODO");
            return uint16_t(-1);
        }

        return uint16_t(-1);
    }

    uint16_t ldWorld::addStaticObject(const vec2f &pos, const ldBound &bound, uint32_t colour, float orn)
    {
        if (!m_physicsEngine) return uint16_t(-1);

        ldBody b;
        b.setBody(pos, orn, 0.f, bound);
        auto bodyId = m_physicsEngine->addBody(b);
        auto id = m_scene.size();
        if (colour != 0x00000000)
            m_scene.emplace_back(WorldObject(WorldObject::WO_STATIC, m_physicsEngine->getBody(bodyId), true, colour));
        else
            m_scene.emplace_back(WorldObject(WorldObject::WO_STATIC, m_physicsEngine->getBody(bodyId)));
        return uint16_t(id);
    }

    uint16_t ldWorld::addDynamicObject(const vec2f &pos, const ldBound &bound, float mass, uint32_t colour, float orn)
    {
        if (!m_physicsEngine) return uint16_t(-1);

        ldBody b;
        b.setBody(pos, orn, mass, bound);
        auto bodyId = m_physicsEngine->addBody(b);
        auto id = m_scene.size();
        if (colour != 0x00000000)
            m_scene.emplace_back(WorldObject(WorldObject::WO_DYNAMIC, m_physicsEngine->getBody(bodyId), true, colour));
        else
            m_scene.emplace_back(WorldObject(WorldObject::WO_DYNAMIC, m_physicsEngine->getBody(bodyId)));
        return uint16_t(id);
    }

    uint16_t ldWorld::addDynamicObject(ldBody *b, uint32_t colour)
    {
        if (!m_physicsEngine) return uint16_t(-1);

        auto id = m_scene.size();
        if (colour != 0x00000000)
            m_scene.emplace_back(WorldObject(WorldObject::WO_DYNAMIC, b, true, colour));
        else
            m_scene.emplace_back(WorldObject(WorldObject::WO_DYNAMIC, b));
        return uint16_t(id);
    }

    const WorldObject *ldWorld::getObject(uint16_t id) const
    {
        if (!m_physicsEngine || id >= m_scene.size()) return nullptr;
        return &m_scene[id];
    }

    WorldObject *ldWorld::getObject(uint16_t id)
    {
        if (!m_physicsEngine || id >= m_scene.size()) return nullptr;
        return &m_scene[id];
    }

    std::vector<const WorldObject *> ldWorld::query(const vec2f &pos, float radius) const
    {
        std::vector<const WorldObject *> scene;
        const auto radSq = radius * radius;

        for (auto &s : m_scene) {
            if ((s.bodyPtr->pos - pos).lengthSq() < radSq) scene.emplace_back(&s);
        }

        return scene;
    }

    std::vector<const WorldObject *> ldWorld::query(const AABB2f_t & /*bound*/) const
    {
        return std::vector<const WorldObject *>();
    }

    uint16_t ldWorld::addNavMesh(const NavMesh &navMesh)
    {
        if (m_navMeshes.size() == MAX_NAVMESHES) return uint16_t(-1);
        uint16_t id = static_cast<uint16_t>(m_navMeshes.size());
        m_navMeshes.emplace_back(navMesh);
        return id;
    }

    const NavMesh *ldWorld::getNavMesh(uint16_t id) const
    {
        if (id >= m_navMeshes.size()) return nullptr;
        return &m_navMeshes[id];
    }

    NavMesh *ldWorld::getNavMesh(uint16_t id)
    {
        if (id >= m_navMeshes.size()) return nullptr;
        return &m_navMeshes[id];
    }

    void drawBound(const ldBound &bound, uint32_t colour, const mat3f &T, ldRazer::ldMesh2f_t &mesh)
    {
        switch (bound.type) {
            case BT_HULL: {
                const auto &geo = bound.hull;
                mesh.indices.emplace_back(ldLineStrip(uint16_t(mesh.vertices.size()), geo.getVertexCount()));
                for (uint i = 0; i != geo.getVertexCount(); ++i)
                    mesh.vertices.emplace_back(ldVertex2f_t{transform(T, geo[i]), colour});
                break;
            }
            case BT_LINEMESH: {
                const auto &geo = *bound.meshPtr;
                for (uint s = 0; s != geo.getStripCount(); ++s) {
                    const auto len = geo.getStripLength(s);
                    const auto start = mesh.vertices.size();
                    for (uint i = 0; i != len; ++i) {
                        mesh.vertices.emplace_back(ldVertex2f_t{transform(T, geo(s, i)), colour});
                    }
                    mesh.indices.emplace_back(ldLineStrip(uint16_t(start), len));
                }
                break;
            }
            case BT_LINE: {
                const auto &geo = bound.line;
                mesh.indices.emplace_back(ldLineStrip(uint16_t(mesh.vertices.size()), 2));
                mesh.vertices.emplace_back(ldVertex2f_t{transform(T, geo.A), colour});
                mesh.vertices.emplace_back(ldVertex2f_t{transform(T, geo.B), colour});
                break;
            }
            default:
                qDebug() << "NOP";
        }
    }

    void drawNavMesh(const NavMesh &navMesh, uint32_t colour, const mat3f &T, ldRazer::ldMesh2f_t &mesh)
    {
        mesh.indices.emplace_back(ldLineStrip(mesh.vertices.size(), navMesh.mesh.size()));

        for (const auto &v : navMesh.mesh) {
            mesh.vertices.emplace_back(ldVertex2f_t{transform(T, v), colour});
        }
    }

}
