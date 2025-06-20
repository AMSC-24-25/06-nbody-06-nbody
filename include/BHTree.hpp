#ifndef BHTREE_HPP
#define BHTREE_HPP

#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include <memory>
#include <cmath>
#include <vector>

const double G = 1;         // Gravitational constant
const double THETA = 0.2;   // Barnes-Hut threshold parameter
const double EPSILON = 0.01; // Softening factor

template <typename T>
class BHTree
{
private:
    Body<T, 2> cluster;
    Quad<T> quad;
    std::unique_ptr<BHTree> NW;
    std::unique_ptr<BHTree> NE;
    std::unique_ptr<BHTree> SW;
    std::unique_ptr<BHTree> SE;
    bool isExternal;

    // Added for depth/leaves control
    int depth;
    int maxDepth;
    int maxLeaves;
    std::vector<Body<T, 2>> leafBodies;

    // Helper to combine bodies
    static Body<T, 2> combineBodies(const Body<T, 2> &a, const Body<T, 2> &b)
    {
        T totalMass = a.getMass() + b.getMass();
        Vector<T, 2> centerOfMass = (a.getPosition() * a.getMass() + b.getPosition() * b.getMass()) / totalMass;
        Vector<T, 2> weightedAvgVel = (a.getVelocity() * a.getMass() + b.getVelocity() * b.getMass()) / totalMass;
        return Body<T, 2>(totalMass, centerOfMass, weightedAvgVel);
    }

public:
    BHTree(const Quad<T> &q, int depth_ = 0, int maxDepth_ = 6, int maxLeaves_ = 4)
        : cluster(0, Vector<T, 2>(), Vector<T, 2>()),
          quad(q), isExternal(true),
          depth(depth_), maxDepth(maxDepth_), maxLeaves(maxLeaves_)
    {}

    void insert(const Body<T, 2> &b)
    {
        // If leaf and not full or at maxDepth, just store in leafBodies
        if (isExternal)
        {
            if (cluster.getMass() == 0 && leafBodies.empty())
            {
                cluster = b;
                leafBodies.push_back(b);
                return;
            }
            leafBodies.push_back(b);

            if ((int)leafBodies.size() <= maxLeaves || depth >= maxDepth)
            {
                // Combine all bodies for this node's cluster
                cluster = leafBodies[0];
                for (size_t i = 1; i < leafBodies.size(); ++i)
                    cluster = combineBodies(cluster, leafBodies[i]);
                return;
            }

            // Time to split!
            isExternal = false;
            NW = std::make_unique<BHTree>(quad.NW(), depth + 1, maxDepth, maxLeaves);
            NE = std::make_unique<BHTree>(quad.NE(), depth + 1, maxDepth, maxLeaves);
            SW = std::make_unique<BHTree>(quad.SW(), depth + 1, maxDepth, maxLeaves);
            SE = std::make_unique<BHTree>(quad.SE(), depth + 1, maxDepth, maxLeaves);

            // Re-insert all existing bodies into children
            for (const auto &body : leafBodies)
            {
                const Vector<T, 2> &pos = body.getPosition();
                if (quad.NW().contains(pos))
                    NW->insert(body);
                else if (quad.NE().contains(pos))
                    NE->insert(body);
                else if (quad.SW().contains(pos))
                    SW->insert(body);
                else
                    SE->insert(body);
            }
            leafBodies.clear(); // Not a leaf anymore!
        }
        else
        {
            // Internal node: update cluster and recurse
            cluster = combineBodies(cluster, b);
            const Vector<T, 2> &pos = b.getPosition();
            if (quad.NW().contains(pos))
                NW->insert(b);
            else if (quad.NE().contains(pos))
                NE->insert(b);
            else if (quad.SW().contains(pos))
                SW->insert(b);
            else
                SE->insert(b);
        }
    }

    void updateForce(Body<T, 2> &b)
    {
        if (isExternal)
        {
            // Add force only from other bodies (not self)
            for (const auto &other : leafBodies)
            {
                if (other.getMass() > 0 && &other != &b)
                {
                    Vector<T, 2> forceVector = calculateForce(other, b);
                    b.updateAcceleration(forceVector / b.getMass());
                }
            }
            return;
        }

        Vector<T, 2> r = cluster.getPosition() - b.getPosition();
        T distance = r.norm();

        // Barnes-Hut opening criterion
        if (quad.getLength() / distance < THETA)
        {
            Vector<T, 2> forceVector = calculateForce(cluster, b);
            b.updateAcceleration(forceVector / b.getMass());
        }
        else
        {
            if (NW) NW->updateForce(b);
            if (NE) NE->updateForce(b);
            if (SW) SW->updateForce(b);
            if (SE) SE->updateForce(b);
        }
    }

    // General force function (Newtonian gravity)
    Vector<T, 2> calculateForce(const Body<T, 2> &src, const Body<T, 2> &b) const
    {
        Vector<T, 2> r = src.getPosition() - b.getPosition();
        T distance = r.norm();
        if (distance < 1e-5)
            distance = 1e-5; // Avoid extremely small distances
        T softenedDistance = std::sqrt(distance * distance + EPSILON * EPSILON);
        T force = (G * b.getMass() * src.getMass()) / (softenedDistance * softenedDistance);
        Vector<T, 2> forceVector = r * (force / distance);
        return forceVector;
    }

    ~BHTree() = default;
};

#endif // BHTREE_HPP
