#include <SFML/Graphics.hpp>

#include "camera3d.hpp"

using namespace sf::_3D;
static std::string update_fps(sf::Clock& clock, std::vector<sf::Time>& frame_times, int& current_frame, sf::Time& total_time) {
    const int num_samples = frame_times.size();
    sf::Time delta_time = clock.restart();
    total_time -= frame_times[current_frame];
    frame_times[current_frame] = delta_time;
    total_time += delta_time;
    current_frame = (current_frame + 1) % num_samples;

    float average_frame_time = total_time.asSeconds() / num_samples;
    float smoothed_fps = 1.0f / average_frame_time;

    std::string fps_string = std::to_string(static_cast<int>(smoothed_fps));
    return fps_string;
}

std::vector<Voxel> voxels_from_image(sf::Image& image, double z_offset) {
    std::vector<Voxel> voxels;
    for (unsigned int x = 0; x < image.getSize().x; x++) {
        for (unsigned int y = 0; y < image.getSize().y; y++) {
            sf::Color color = image.getPixel({ x, y });
            if (color == sf::Color::Transparent) {
                continue;
            }
            Voxel voxel(sf::Vector3<double>{(double)x * 16.f, (double)y * 16.f, z_offset});
            voxel.set_face_color(Voxel::FACE::FRONT, color);
            voxels.emplace_back(voxel);
        }
    }
    return voxels;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().size), "My window", sf::State::Fullscreen);
    window.setFramerateLimit(60);

    Voxel voxel(sf::Vector3<double>(8, 8, 8));
    //voxel.set_face_color(Voxel::FACE::FRONT, sf::Color::Blue);
    voxel.set_face_color(Voxel::FACE::BACK, sf::Color::Red);

    std::vector<Voxel> voxels;
    voxels.emplace_back(voxel);

    Camera3D camera(sf::Vector3f(0, 0, 0), sf::Vector3<sf::Angle>(), sf::VideoMode::getDesktopMode().size);

    sf::Font font;
    if (not font.openFromFile("fonts/Iosevka.ttc")) {
        throw std::runtime_error("cant find fonts/Iosevka.ttc");
    }

    sf::Clock clock;
    const int num_samples = 60;
    std::vector<sf::Time> frame_times(num_samples);
    int current_frame = 0;
    sf::Time total_time = sf::Time::Zero;
    sf::Text fps_text(font, update_fps(clock, frame_times, current_frame, total_time), 30U);
    fps_text.setFillColor(sf::Color::White);
    fps_text.setPosition({ 0, 0 });

    while (window.isOpen()) {
        fps_text.setString(update_fps(clock, frame_times, current_frame, total_time));
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }

        // camera controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            camera.move({ -1, 0, 0 });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            camera.move({ 1, 0, 0 });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            camera.move({ 0, -1, 0 });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            camera.move({ 0, 1, 0 });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            camera.rotate({ sf::degrees(0), sf::degrees(1), sf::degrees(0) });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            camera.rotate({ sf::degrees(0), sf::degrees(-1), sf::degrees(0) });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            camera.rotate({ sf::degrees(1), sf::degrees(0), sf::degrees(0) });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            camera.rotate({ sf::degrees(-1), sf::degrees(0), sf::degrees(0) });
        }

        camera.update(voxels);

        window.clear(sf::Color::Black);

        window.draw(camera);
        window.draw(fps_text);

        window.display();
    }
}