/////////////////////
//    voxel.hpp    //
/////////////////////

#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Angle.hpp>

#include <unordered_map>

namespace sf {
    namespace _3D {

        ////////////////////////////
        //    Hidden Namespace    //
        ////////////////////////////

        namespace {
            Vector3<double> rotate3D(const Vector3<double>& point, const Vector3<double>& center, const Vector3<double>& axis, Angle angle);

            class OutlineRect {
            public:
                Vector2<float> point1;
                Vector2<float> point2;
                Vector2<float> point3;
                Vector2<float> point4;

                Color color;

                void draw(RenderTarget& target) const;
            };
        }

        /////////////////
        //    Voxel    //
        /////////////////

        class Voxel {
        public:
            Voxel& operator=(const Voxel& other) {
                if (this != &other) {
                    points = other.points;
                    center = other.center;
                }
                return *this;
            }

            Voxel() = default;

            Voxel(Color color);

            Voxel(Vector3<double> position);

            Voxel(Vector3f position, Vector3<Angle> rotation);

            Voxel(Vector3f position, Vector3<Angle> rotation, Color color);

            void move(Vector3<double> offset);

            void set_position(Vector3<double> position);

            void rotate(Vector3<Angle> rotation);

            Vector3<double> get_center() const;

            void update(Vector3<double> position_offset, Vector3<Angle> rotation_offset, sf::Vector3 < double > rotation_axis);

            void draw(RenderTarget& target) const;

            void debug_draw(RenderTarget& target, Vector3<double> position_offset, Vector3<Angle> rotation_offset, sf::Vector3 < double > rotation_axis);

            std::optional<Color> color = Color::Transparent;

            enum class FACE {
                FRONT,
                BACK,
                LEFT,
                RIGHT,
                TOP,
                BOTTOM,
                END
            };

            double perceived_z = 0;

            void set_face_color(FACE face, Color color);

        private:
            void shrink(double amount) {
                for (sf::Vector3<double>& point : points) {
                    sf::Vector3<double> direction = center - point;

                    double distance = direction.length();

                    if (distance <= amount || distance == 0) {
                        point = center;
                        return;
                    }

                    sf::Vector3<double> normalized = direction.normalized();

                    sf::Vector3<double> movement = normalized * amount;

                    point += movement;
                }
            }

            double dist_scale_dropoff(double z);

            std::vector < Vector3 < double > > points;
            Vector3<double> center;

            std::vector<std::pair<double, OutlineRect>> i_depth_sorted_shapes;

            const std::unordered_map < FACE, std::tuple<int, int, int, int> > i_FACES = {
                {FACE::FRONT, std::make_tuple(0, 1, 2, 3)},
                {FACE::BACK, std::make_tuple(4, 5, 6, 7)},
                {FACE::TOP, std::make_tuple(0, 1, 6, 7)},
                {FACE::BOTTOM, std::make_tuple(2, 3, 4, 5)},
                {FACE::RIGHT, std::make_tuple(1, 2, 5, 6)},
                {FACE::LEFT, std::make_tuple(0, 3, 4, 7)}
            };

            std::unordered_map<FACE, Color> color_by_face = {};

            int voxel_size = 128;

        };
    }
}