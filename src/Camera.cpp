#include "Camera.hpp"

#include <cmath>

#include "Vector.hpp"
#include "Matrix.hpp"

Camera::Camera(float fovy,
               float aspect_ratio,
               float near,
               float far) :
    _look_at({ 0.0, 0.0, -1.0 }),
    _view_up({ 0.0, 1.0, 0.0 })
{
    _computeWorldToCamera();
    _computePerspectiveProjection(fovy, aspect_ratio, near, far);
}

void Camera::setPosition(const vec3 &position)
{
    _position = position;
}

void Camera::setSphericalPosition(const vec3 &position)
{
    _spherical_position = position;
}

void Camera::setLookAt(const vec3 &point)
{
    _look_at = point;
}

void Camera::update(double dt)
{
    if (_orbit_mode) {
        _updatePosition(_spherical_position,
                        _spherical_velocity,
                        _spherical_acceleration,
                        _ORBIT_FRICTION,
                        _ORBIT_MAX_SPEED,
                        dt);
        _sphericalToCartesian();
    } else {
        _updatePosition(_position,
                        _velocity,
                        _acceleration,
                        _MOVE_FRICTION,
                        _MOVE_MAX_SPEED,
                        dt);
    }

    _computeWorldToCamera();
}

void Camera::_computeWorldToCamera()
{
    // x, y, z axes in camera frame of reference
    vec3 u, v, n;

    n = _position - _look_at;
    u = vec3::cross(_view_up, n);
    v = vec3::cross(n, u);

    n.normalize();
    u.normalize();
    v.normalize();

    mat4 rotation = mat4::identity();
    mat4 translation = mat4::identity();

    // TODO: write a more elegant way
    for (int i = 0; i < 3; ++i) {
        rotation(0, i) = u[i];
        rotation(1, i) = v[i];
        rotation(2, i) = n[i];
        translation(i, 3) = -_position[i];
    }

    _worldToCamera = rotation * translation;
}

void Camera::_computePerspectiveProjection(float fovy,
                                           float aspect_ratio,
                                           float far,
                                           float near)
{
    mat4 &mat = _perspectiveProjection;

    mat(0, 0) = 1 / (aspect_ratio * std::tan(fovy / 2));
    mat(1, 1) = 1 / std::tan(fovy / 2);
    mat(2, 2) = - (near + far) / (far - near);
    mat(2, 3) = 2 * near * far / (far - near);
    mat(3, 2) = -1.0;
}

void Camera::_updatePosition(vec3 &position,
                             vec3 &velocity,
                             vec3 &acceleration,
                             double friction,
                             double max_speed,
                             double dt)
{
    velocity += dt * friction * acceleration;
    double speed = velocity.norm();
    if (speed > max_speed) {
        // Limit velocity norm
        velocity *= (max_speed / speed);
    }

    position += dt * velocity;
    acceleration = vec3::ZERO;
}

void Camera::_sphericalToCartesian()
{
    double theta = _spherical_position[1];
    double phi = _spherical_position[2];

    _position[0] = std::cos(theta) * std::cos(phi);
    _position[1] = std::sin(theta);
    _position[2] = std::cos(theta) * std::sin(phi);
    // Multiply by radius
    _position *= _spherical_position[0];
}
