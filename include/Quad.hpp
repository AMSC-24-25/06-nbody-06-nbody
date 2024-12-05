#ifndef QUAD_HPP
#define QUAD_HPP

#include "Vector.hpp"

template <typename T>
class Quad
{
private:
    Vector<T, 2> origin; // left-bottom point as the coordinate origin of quad
    T length;            // length of the quad

public:
    Quad(const Vector<T, 2> &origin, T length)
        : origin(origin), length(length) {}

    T getLength() const { return length; }

    // Check if a point is contained in this quad
    bool contains(const Vector<T, 2> &point) const
    {
        return (point[0] >= origin[0] && point[0] <= origin[0] + length &&
                point[1] >= origin[1] && point[1] <= origin[1] + length);
    }

    // Quadrant splitting for four corners
    Quad NW() const
    {
        Vector<T, 2> nwOrigin{origin[0], origin[1] + length / 2};
        return Quad(nwOrigin, length / 2);
    }

    Quad NE() const
    {
        Vector<T, 2> neOrigin{origin[0] + length / 2, origin[1] + length / 2};
        return Quad(neOrigin, length / 2);
    }

    Quad SW() const
    {
        return Quad(origin, length / 2);
    }

    Quad SE() const
    {
        Vector<T, 2> seOrigin{origin[0] + length / 2, origin[1]};
        return Quad(seOrigin, length / 2);
    }
};

#endif // QUAD_HPP