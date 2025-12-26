/////////////////////
//    voxel.cpp    //
/////////////////////

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>

#include "voxel.hpp"

namespace sf {

    namespace _3D {
        ////////////////////////////
        //    Hidden Namespace    //
        ////////////////////////////

        namespace {
            Vector3<double> rotate3D(const Vector3<double>& point, const Vector3<double>& center, const Vector3<double>& axis, Angle angle) {
                Vector3<double> p = { point.x - center.x, point.y - center.y, point.z - center.z };

                Vector3<double> k = axis.normalized();

                Vector3<double> term1 = {
                    p.x * std::cos(angle.asRadians()),
                    p.y * std::cos(angle.asRadians()),
                    p.z * std::cos(angle.asRadians())
                };

                Vector3<double> term2 = k.cross(p);

                term2 = {
                    term2.x * std::sin(angle.asRadians()),
                    term2.y * std::sin(angle.asRadians()),
                    term2.z * std::sin(angle.asRadians())
                };

                double kDotP = k.dot(p);

                Vector3<double> term3 = {
                    k.x * kDotP * (1 - std::cos(angle.asRadians())),
                    k.y * kDotP * (1 - std::cos(angle.asRadians())),
                    k.z * kDotP * (1 - std::cos(angle.asRadians()))
                };

                Vector3<double> rotated = {
                    term1.x + term2.x + term3.x,
                    term1.y + term2.y + term3.y,
                    term1.z + term2.z + term3.z
                };

                rotated.x += center.x;
                rotated.y += center.y;
                rotated.z += center.z;

                return rotated;
            }
        }

        Voxel::Voxel(Color color)
            : color(color) {
            points.reserve(8);
        }

        Voxel::Voxel(Vector3<double> position) {
            points.reserve(8);
            points.push_back(position); // top front left
            points.push_back({ position.x + voxel_size, position.y, position.z }); // top front right
            points.push_back({ position.x + voxel_size, position.y + voxel_size, position.z }); // bottom front right
            points.push_back({ position.x, position.y + voxel_size, position.z }); // bottom front left
            points.push_back({ position.x, position.y + voxel_size, position.z + voxel_size }); // bottom back left
            points.push_back({ position.x + voxel_size, position.y + voxel_size, position.z + voxel_size }); // bottom back right
            points.push_back({ position.x + voxel_size, position.y, position.z + voxel_size }); // top back right
            points.push_back({ position.x, position.y, position.z + voxel_size }); // top back left
            center = { position.x + voxel_size / 2, position.y + voxel_size / 2 , position.z + voxel_size / 2 };
        }

        /////////////////
        //    Voxel    //
        /////////////////

        Voxel::Voxel(Vector3f position, Vector3<Angle> rotation) {
            points.reserve(8);
        }

        Voxel::Voxel(Vector3f position, Vector3<Angle> rotation, Color color) {
            points.reserve(8);
        }

        void Voxel::move(Vector3<double> offset) {
            center += offset;
            for (auto& point : points) {
                point += offset;
            }
        }

        void Voxel::set_position(sf::Vector3<double> position) {
            // Move the voxel so its center becomes `position`.
            // Compute the delta from the current center to the new position
            // and apply it to all corner points.
            sf::Vector3<double> offset = position - center;
            center = position;

            for (auto& point : points) {
                point += offset;
            }
        }

        void Voxel::rotate(Vector3<Angle> rotation) {
            for (auto& point : points) {
                if (rotation.x.asDegrees() != 0) {
                    point = rotate3D(point, center, { 1, 0, 0 }, rotation.x);
                }
                if (rotation.y.asDegrees() != 0) {
                    point = rotate3D(point, center, { 0, 1, 0 }, rotation.y);
                }
                if (rotation.z.asDegrees() != 0) {
                    point = rotate3D(point, center, { 0, 0, 1 }, rotation.z);
                }
            }
        }

        Vector3<double> Voxel::get_center() const {
            return center;
        }

        void Voxel::update(Vector3<double> position_offset, Vector3<Angle> rotation_offset, sf::Vector3 < double > rotation_axis) {
            Voxel copy = *this;
            copy.move(-position_offset);
            copy.rotate(-rotation_offset);
            if (rotation_offset.x != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 1, 0, 0 }, rotation_offset.x));
            }
            if (rotation_offset.y != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 0, 1, 0 }, rotation_offset.y));
            }
            if (rotation_offset.z != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 0, 0, 1 }, rotation_offset.z));
            }

            double dropoff = dist_scale_dropoff(copy.center.z);
            copy.shrink(4 - (4 * dropoff));

            perceived_z = copy.center.z;

            std::vector<std::pair<double, Rect2D>> depth_sorted_shapes;
            depth_sorted_shapes.reserve(6);

            for (int i = 0; i < static_cast<int>(FACE::END); i++) {
                std::tuple<int, int, int, int> face = i_FACES.at(static_cast<FACE>(i));

                int one = std::get<0>(face);
                int two = std::get<1>(face);
                int three = std::get<2>(face);
                int four = std::get<3>(face);


                
                Rect2D rect();

                

                if (color_by_face.find(static_cast<FACE>(i)) != color_by_face.end()) {
                    rect.color = color_by_face[static_cast<FACE>(i)];
                }
                else {
                    rect.color = color.value();
                }

                double averageZ = (copy.points[one].z + copy.points[two].z + copy.points[three].z + copy.points[four].z) / 4.0;

                depth_sorted_shapes.emplace_back(averageZ, shape);
            }

            std::sort(depth_sorted_shapes.begin(), depth_sorted_shapes.end(),
                [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

            i_depth_sorted_shapes = depth_sorted_shapes;
        }

        void Voxel::draw(RenderTarget& target) const {
            int celing = 8;
            for (int i = 0; i < celing; i++) {
                if (i > 5) {
                    return;
                }
                if (i_depth_sorted_shapes[i].second.color == sf::Color::Transparent) {
                    //celing += 1;
                    //continue;
                }

                i_depth_sorted_shapes[i].second.draw(target);
            }
        }

        void Voxel::set_face_color(FACE face, Color color) {
            color_by_face[face] = color;
        }

        double Voxel::dist_scale_dropoff(double z) {
            return 1 - 0.002 * z;
        }
         
        void Voxel::debug_draw(RenderTarget& target, Vector3<double> position_offset, Vector3<Angle> rotation_offset, sf::Vector3 < double > rotation_axis) {
            Voxel copy = *this;
            copy.move(-position_offset);
            copy.rotate(-rotation_offset);
            if (rotation_offset.x != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 1, 0, 0 }, rotation_offset.x));
            }
            if (rotation_offset.y != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 0, 1, 0 }, rotation_offset.y));
            }
            if (rotation_offset.z != sf::degrees(0)) {
                copy.set_position(rotate3D(copy.center, rotation_axis, { 0, 0, 1 }, rotation_offset.z));
            }


            for (const auto& point : copy.points) {
                CircleShape point_circle(5);
                point_circle.setPosition(Vector2f(point.x - 5, point.y - 5));
                point_circle.setFillColor(Color::White);
                target.draw(point_circle);
            }

            CircleShape center_circle(5);
            center_circle.setPosition(Vector2f(copy.center.x - 5, copy.center.y - 5));
            center_circle.setFillColor(Color::Red);
            target.draw(center_circle);

            CircleShape rotation_circle(5);
            rotation_circle.setPosition(Vector2f(rotation_axis.x - 5, rotation_axis.y - 5));
            rotation_circle.setFillColor(Color::Blue);
            target.draw(rotation_circle);
        }
    }
}
