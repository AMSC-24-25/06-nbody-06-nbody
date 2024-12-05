#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.hpp"
#include "Matrix.hpp"

using vec3 = Vector<float, 3>;
using mat4 = Matrix<float, 4, 4>;

class Camera
{
public:
    Camera(float fovy, /* Vertical field of view */
           float aspect_ratio, /* Window width/height */
           float near, /* z coordinate of near clipping plane */
           float far   /* z coordinate of far clipping plane */);

    const vec3 &getPosition() const;
    // Getters for camera transformation matrices
    const mat4 &getWorldToCamera() const;
    const mat4 &getPerspectiveProjection() const;

    void setPosition(const vec3 &position);
    void setSphericalPosition(const vec3 &position);
    // Sets camera frame of reference axes
    void setFrameOfReference(const vec3 &u, /* x axis on view plane */
                             const vec3 &v, /* y axis on view plane */
                             const vec3 &n  /* normal to the view plane */);
    void setOrbitMode(bool flag);
    // Points the camera to the given point
    void lookAt(const vec3 &point,
                const vec3 &view_up = { 0.0, 1.0, 0.0 });

    // Moves the camera around the look at point
    // using spherical coordinates
    void orbit(const vec3 &delta);
    // Moves the camera using cartesian coordinates
    void move(const vec3 &delta);
    // Updates camera position and recomputes
    // world to camera transformation matrix
    void update(double dt);

private:
    // Computes the transformation matrix
    // to move from world coordinates to
    // camera coordinates
    void _computeWorldToCamera();
    // Computes the transformation matrix
    // to project points in camera coordinates
    // to the view plane
    void _computePerspectiveProjection(float fovy,
                                       float aspect_ratio,
                                       float near,
                                       float far);
    // Performs backwards Euler integration to update
    // cartesian or spherical position
    void _updatePosition(vec3 &position,
                         vec3 &velocity,
                         vec3 &acceleration,
                         float friction,
                         float max_speed,
                         float dt);
    // Converts current spherical coordinates position
    // to cartesian coordinates
    void _sphericalToCartesian();

    // Cartesian coordinates
    vec3 _position;
    vec3 _velocity;
    vec3 _acceleration;
    // Spherical coordinates used to orbit camera
    vec3 _spherical_position;
    vec3 _spherical_velocity;
    vec3 _spherical_acceleration;
    // Camera frame of reference axes
    vec3 _axis_u, _axis_v, _axis_n;

    // Transformation matrices
    mat4 _worldToCamera;
    mat4 _perspectiveProjection;

    bool _orbit_mode;

    const double _MOVE_FRICTION = 0.99f;
    const double _MOVE_MAX_SPEED = 10.0f;
    const double _ORBIT_FRICTION = 0.99f;
    const double _ORBIT_MAX_SPEED = 1.0f;
};

#endif
