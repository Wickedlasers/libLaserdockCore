//
// Created by Darren Otgaar on 2018/10/22.
//

#ifndef LASERDOCKCORE_LDQUADTREE_H
#define LASERDOCKCORE_LDQUADTREE_H

// A quad tree is a 2D spatial sorting structure that speeds up collision queries by enabling a prior broad phase search.
// This allows us to increase the number of collidable objects (i.e. like a race-track boundary) so that we can make
// it smoother and more curve-like.

#include "ldRazer.h"
#include "ldBody.h"
#include "ldQueries.h"

namespace ldRazer
{

// Currently accepts any NodeT that contains a public member called "pos" of type vec2f
    template<typename NodeT=ldBody>
    class ldQuadTree
    {
    public:
        ldQuadTree();

        explicit ldQuadTree(const AABB2f_t &bound);

        ~ldQuadTree();

        std::vector<NodeT *> queryBound(const AABB2f_t &query, bool strict = false) const;

        void clear();

        int depth() const;

        int count() const;

        bool intersection(const vec2f &point) const
        {
            return ldRazer::intersection(m_bound, point);
        }

        bool intersection(const AABB2f_t &bound) const
        {
            return ldRazer::intersection(m_bound, bound);
        }

        bool intersection(const hull2f_t &hull) const
        {
            return ldRazer::intersection(m_bound, hull);
        }

        void createTree(const std::vector<NodeT *> &lst);

        void createTree(const std::vector<NodeT *> &lst, const vec2f &centre, float width);

        bool addObject(NodeT *nodePtr);

        void updateTree(ldQuadTree *root);

        static void computeDims(const std::vector<NodeT *> &lst, vec2f &centre, float &width);

    protected:
        void createTree(const std::vector<NodeT *> &lst, const vec2f &centre, float width, uint16_t level);

        void queryBound(const AABB2f_t &query, std::vector<NodeT *> &result, bool strict = false) const;

        void depth(int &counter) const;

        void count(int &counter) const;

        ldQuadTree *quadIntersection(const vec2f &P);

        void createChildren(const vec2f &centre, float width);

        uint m_nodeMax = 10;
        AABB2f_t m_bound;
        std::vector<NodeT *> m_nodes;
        std::unique_ptr<ldQuadTree> m_quads[4];
    };

    template<typename NodeT>
    ldQuadTree<NodeT>::ldQuadTree() = default;

    template<typename NodeT>
    ldQuadTree<NodeT>::ldQuadTree(const AABB2f_t &bound) : m_bound(bound)
    {
    }

    template<typename NodeT>
    ldQuadTree<NodeT>::~ldQuadTree() = default;

    template<typename NodeT>
    std::vector<NodeT *> ldQuadTree<NodeT>::queryBound(const AABB2f_t &query, bool strict) const
    {
        std::vector<NodeT *> result;

        if (!ldRazer::intersection(m_bound, query)) return result;
        queryBound(query, result, strict);
        return result;
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::clear()
    {
        for (auto &q : m_quads) if (q) q.reset(nullptr);
        m_nodes.clear();
        m_bound.setCorners(vec2f{0.f, 0.f}, vec2f{0.f, 0.f});
    }

    template<typename NodeT>
    int ldQuadTree<NodeT>::depth() const
    {
        int counter = 0;
        depth(counter);
        return counter;
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::depth(int &counter) const
    {
        int max = counter + 1;
        for (const auto &q : m_quads) {
            if (!q) continue;
            q->depth(max);
            if (max > counter) counter = max;
        }
    }

    template<typename NodeT>
    int ldQuadTree<NodeT>::count() const
    {
        int counter = 0;
        count(counter);
        return counter;
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::count(int &counter) const
    {
        counter += m_nodes.size();
        for (const auto &q : m_quads) {
            if (!q) continue;
            q->count(counter);
        }
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::queryBound(const AABB2f_t &query, std::vector<NodeT *> &result, bool strict) const
    {
        // Return the contents of any quad that intersects with the query
        if (!strict) std::copy(m_nodes.begin(), m_nodes.end(), std::back_inserter(result));
        else {
            for (const auto &b : m_nodes) {
                if (ldRazer::intersection(query, b->pos)) {
                    result.emplace_back(b);
                }
            }
        }

        for (const auto &q : m_quads) {
            if (!q) continue;
            if (q->intersection(query)) {
                q->queryBound(query, result);
            }
        }
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::computeDims(const std::vector<NodeT *> &lst, vec2f &centre, float &width)
    {
        centre = vec2f{0.f, 0.f};
        float xmin = std::numeric_limits<float>::max(), xmax = -xmin;
        float ymin = std::numeric_limits<float>::max(), ymax = -ymin;
        for (auto &b : lst) {
            centre += b->pos;
            if (b->pos.x < xmin) xmin = b->pos.x;
            if (b->pos.x > xmax) xmax = b->pos.x;
            if (b->pos.y < ymin) ymin = b->pos.y;
            if (b->pos.y > ymax) ymax = b->pos.y;
        }

        centre /= lst.size();
        auto xdiff = xmax - xmin, ydiff = ymax - ymin;
        width = sqrt(xdiff * xdiff + ydiff * ydiff);
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::createTree(const std::vector<NodeT *> &lst)
    {
        vec2f centre;
        float width;
        computeDims(lst, centre, width);
        createTree(lst, centre, width, 0);
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::createTree(const std::vector<NodeT *> &lst, const vec2f &centre, float width)
    {
        return createTree(lst, centre, width, 0);
    }

    template<typename NodeT>
    void
    ldQuadTree<NodeT>::createTree(const std::vector<NodeT *> &lst, const vec2f &centre, float width, uint16_t level)
    {
        m_bound = AABB2f_t{centre, width};

        if (lst.size() < m_nodeMax) {
            m_nodes = lst;
        } else {
            auto childWidth = width / 2.f;
            const vec2f centres[4] = {
                    vec2f{centre.x - childWidth, centre.y - childWidth},
                    vec2f{centre.x + childWidth, centre.y - childWidth},
                    vec2f{centre.x + childWidth, centre.y + childWidth},
                    vec2f{centre.x - childWidth, centre.y + childWidth}
            };

            for (auto i = 0; i != 4; ++i) {
                //qDebug() << "New Quad:" << centres[i].x << centres[i].y;
                m_quads[i] = std::unique_ptr<ldQuadTree>(new ldQuadTree(AABB2f_t{centres[i], childWidth}));
            }

            std::vector<NodeT *> newNodes[4];
            for (const auto &node : lst) {
                for (auto i = 0; i != 4; ++i) {
                    if (m_quads[i]->intersection(node->pos)) {
                        //qDebug() << "Quad[" << i << "]" << node->pos.x << node->pos.y;
                        newNodes[i].emplace_back(node);
                        break;
                    }
                }
            }

            for (auto i = 0; i != 4; ++i) {
                m_quads[i]->createTree(newNodes[i], centres[i], childWidth, ++level);
            }
        }
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::createChildren(const vec2f &centre, float width)
    {
        auto childWidth = width / 2.f;
        const vec2f centres[4] = {
                vec2f{centre.x - childWidth, centre.y - childWidth},
                vec2f{centre.x + childWidth, centre.y - childWidth},
                vec2f{centre.x + childWidth, centre.y + childWidth},
                vec2f{centre.x - childWidth, centre.y + childWidth}
        };

        for (auto i = 0; i != 4; ++i) {
            //qDebug() << "New Quad:" << centres[i].x << centres[i].y;
            m_quads[i] = std::unique_ptr<ldQuadTree<NodeT>>(new ldQuadTree<NodeT>(AABB2f_t{centres[i], childWidth}));
        }
    }

    template<typename NodeT>
    bool ldQuadTree<NodeT>::addObject(NodeT *nodePtr)
    {
        if (!intersection(nodePtr->pos)) {
            qDebug() << "Node out of bounds";
            return false;
        }

        if (!m_quads[0] && m_nodes.size() < m_nodeMax) {
            m_nodes.emplace_back(nodePtr);
        } else if (!m_quads[0]) {
            createChildren(m_bound.centre(), std::max(m_bound.width(), m_bound.height()));
            m_nodes.emplace_back(nodePtr);
            updateTree(this);
        } else {
            for (auto &q : m_quads) q->addObject(nodePtr);
        }

        return true;
    }

    template<typename NodeT>
    ldQuadTree<NodeT> *ldQuadTree<NodeT>::quadIntersection(const vec2f &P)
    {
        if (!m_quads[0] && intersection(P)) return this;
        else if (!m_quads[0]) return nullptr;
        else {
            ldQuadTree *ptr = nullptr;
            for (auto &q : m_quads) {
                ptr = q->quadIntersection(P);
                if (ptr) break;
            }
            return ptr;
        }
    }

    template<typename NodeT>
    void ldQuadTree<NodeT>::updateTree(ldQuadTree *root)
    {
        if (!root) return;

        if (!m_quads[0]) {
            auto it = std::remove_if(m_nodes.begin(), m_nodes.end(), [&](NodeT *ptr)
            {
                auto q = root->quadIntersection(ptr->pos);
                if (q == this) return false;
                else if (q == nullptr) return true;
                else {
                    q->m_nodes.emplace_back(ptr);
                    return true;
                }
            });

            if (it != m_nodes.end()) m_nodes.erase(it, m_nodes.end());
        } else {
            for (auto &q : m_quads) {
                q->updateTree(root);
            }
        }
    }

}

#endif //LASERDOCKCORE_LDQUADTREE_H
