////////////////////////
//    camera3d.cpp    //
////////////////////////

#include <SFML/Graphics/Sprite.hpp>

#include <iostream>
#include <algorithm>

#include "camera3d.hpp"

namespace sf {
	namespace _3D {

		namespace {
			Vector3<double> calculate_mean_center(const std::vector<Voxel>& voxels) {
				if (voxels.empty()) {
					return Vector3 <double>(0.0f, 0.0f, 0.0f);
				}
				//if (voxels.size() == 1) {
					//return voxels[0].get_center();
				//}

				Vector3 <double> sum(0.0f, 0.0f, 0.0f);

				for (int i = 0; i < voxels.size(); i++) {
					sum += voxels[i].get_center();
				}

				double count = static_cast<double>(voxels.size());
				Vector3<double> mean = sum / count;

				//std::cout << "mean: " << mean.x << ", " << mean.y << ", " << mean.z << "\n position" << voxels[0].get_center().x << ", " << voxels[0].get_center().y << ", " << voxels[0].get_center().z << "\n";

				return mean;
			}
		}

		Camera3D::Camera3D(Vector3f position, Vector3<Angle> rotation, Vector2u size)
			: position(position),
			rotation(rotation),
			size(size) {
			if (not rt.resize(size)) {
				// idk >:)
			}
		}

		Vector3f Camera3D::get_position() {
			return position;
		}

		Vector3<Angle> Camera3D::get_rotation() {
			return rotation;
		}

		void Camera3D::set_position(Vector3f position) {
			this->position = position;
		}

		void Camera3D::set_rotation(Vector3<Angle> rotation) {
			this->rotation = rotation;
		}

		void Camera3D::rotate(Vector3<Angle> rotation) {
			this->rotation += rotation;
		}

		void Camera3D::move(Vector3f offset) {
			position += offset;
		}

		void Camera3D::draw(RenderTarget& target, RenderStates states) const {
			target.draw(Sprite(rt.getTexture()), states);
		}

		void Camera3D::update(std::vector<Voxel>& voxels) {
			rt.clear(Color::Transparent);
			std::vector<std::pair<double, Voxel>> depth_sorted_voxels;
			depth_sorted_voxels.reserve(voxels.size());

			sf::Vector3<double> mean = calculate_mean_center(voxels);
			mean -= Vector3<double>(position);

			for (Voxel& voxel : voxels) {
				voxel.update(Vector3<double>(position), rotation, mean);
				depth_sorted_voxels.emplace_back(voxel.perceived_z, voxel);
			}

			std::sort(depth_sorted_voxels.begin(), depth_sorted_voxels.end(),
				[](const auto& a, const auto& b) {
					return a.first > b.first;
			});

			for (auto& voxel : depth_sorted_voxels) {
				voxel.second.draw(rt);
			}
			rt.display();
		}
	}
}