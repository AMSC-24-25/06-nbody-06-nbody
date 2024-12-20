#ifndef BHTREE_HPP
#define BHTREE_HPP

#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include <memory>
#include <cmath>

const double G = 1;         // Gravitational constant
const double THETA = 0.1;   // Barnes-Hut threshold parameter
const double EPSILON = 0.0; // Softening factor for force calculation (not using now for small size bodies)

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
    bool isExternal; // External nodes are like leaves in the binary tree

    // Helper method to combine 2 bodies together
    static Body<T, 2> combineBodies(const Body<T, 2> &a, const Body<T, 2> &b)
    {
        T totalMass = a.getMass() + b.getMass();
        Vector<T, 2> centerOfMass = (a.getPosition() * a.getMass() + b.getPosition() * b.getMass()) / totalMass;

        // Velocity is weighted average of velocities
        Vector<T, 2> weightedAvgVel = (a.getVelocity() * a.getMass() + b.getVelocity() * b.getMass()) / totalMass;

        Body<T, 2> combined(totalMass, centerOfMass, weightedAvgVel);
        return combined;
    }

public:
    BHTree(const Quad<T> &q) : cluster(0, Vector<T, 2>(), Vector<T, 2>()), quad(q), isExternal(true) {}

    // Recursive method to insert a body into the tree
    void insert(const Body<T, 2> &b)
    {
        // If this node has no body, add the body
        if (isExternal && cluster.getMass() == 0)
        {
            cluster = b;
            return;
        }

        // If this is an external node (leaf for binary tree), split and rearrange the tree structure
        if (isExternal)
        {
            isExternal = false;
            Body<T, 2> oldCluster = cluster;
            cluster = combineBodies(oldCluster, b);

            // Create quadrants if not exists
            if (!NW)
                NW = std::make_unique<BHTree>(quad.NW());
            if (!NE)
                NE = std::make_unique<BHTree>(quad.NE());
            if (!SW)
                SW = std::make_unique<BHTree>(quad.SW());
            if (!SE)
                SE = std::make_unique<BHTree>(quad.SE());

            Vector<T, 2> oldPos = oldCluster.getPosition();
            Vector<T, 2> newPos = b.getPosition();

            // Insert old and new body into appropriate quadrants
            if (quad.NW().contains(oldPos))
                NW->insert(oldCluster);
            else if (quad.NE().contains(oldPos))
                NE->insert(oldCluster);
            else if (quad.SW().contains(oldPos))
                SW->insert(oldCluster);
            else
                SE->insert(oldCluster);

            if (quad.NW().contains(newPos))
                NW->insert(b);
            else if (quad.NE().contains(newPos))
                NE->insert(b);
            else if (quad.SW().contains(newPos))
                SW->insert(b);
            else
                SE->insert(b);
        }
        else
        {
            // Internal node: multiple childs update center of mass of current cluster and insert b recursively
            cluster = combineBodies(cluster, b);

            Vector<T, 2> newPos = b.getPosition();

            if (quad.NW().contains(newPos))
                NW->insert(b);
            else if (quad.NE().contains(newPos))
                NE->insert(b);
            else if (quad.SW().contains(newPos))
                SW->insert(b);
            else
                SE->insert(b);
        }
    }

    // Recursive method to update forces acting on a body
    void updateForce(Body<T, 2> &b)
    {
        if (isExternal)
        {
            // Not adding force to itself or non-existent bodies
            if (cluster.getMass() > 0 && &cluster != &b)
            {
                Vector<T, 2> forceVector = calculateForce(cluster, b);
                b.updateAcceleration(forceVector / b.getMass());
            }
            return;
        }

        // Compute distance from body to cluster's center of mass
        Vector<T, 2> r = cluster.getPosition() - b.getPosition();
        T distance = r.norm();

        // Threshold for Barnes-Hut about if we can treat a cluster as a single body when it is far away
        if (quad.getLength() / distance < THETA)
        {
            Vector<T, 2> forceVector = calculateForce(cluster, b);
            b.updateAcceleration(forceVector / b.getMass());
        }
        else
        {
            // Recursively update forces from each subnode if exists
            if (NW)
                NW->updateForce(b);
            if (NE)
                NE->updateForce(b);
            if (SW)
                SW->updateForce(b);
            if (SE)
                SE->updateForce(b);
        }
    }

    // Here we should use a gereral force function in the future
    const Vector<T, 2> calculateForce(Body<T, 2> &cluster, Body<T, 2> &b) const
    {
        Vector<T, 2> r = cluster.getPosition() - b.getPosition();
        T distance = r.norm();

        // Add check for minimum distance to prevent division by zero or instability
        if (distance < 1e-5)
            distance = 1e-5; // Avoid extremely small distances, maybe there exists a better way

        // Use softened force calculation (now using EPSILON = 0)
        T softenedDistance = std::sqrt(distance * distance + EPSILON * EPSILON);
        T force = (G * b.getMass() * cluster.getMass()) / (softenedDistance * softenedDistance);
        // Normalize r for direction
        Vector<T, 2> forceVector = r * (force / distance);
        return forceVector;
    }

    // Destroy: std::unique_ptr members automatically handle deletion of their managed objects when the Tree object is destroyed.
    ~BHTree() = default;
};

#endif // BHTREE_HPP