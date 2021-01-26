#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"


//Global Times
const float timeBeforeCollision = 4.f;
const float coastingTime = 2.f;

//Ball is an Object that will collide
class Ball
{
public:

	Ball(sf::Color color)
	{
		circle.setFillColor(color);
	}

	void initializeBall(sf::Vector2f position, float acceleration, float finalVelocity, float mass) {
		circle.setPosition(position);
		this->acceleration = acceleration;
		this->finalVelocity = finalVelocity;
		this->mass = mass;
		radius = (std::sqrt(mass)) * 10;
		circle.setRadius(radius);
		circle.setOrigin(radius, radius);

		clock.restart();
		velocity = 0;
		
	}

	void update(float deltatime)
	{
		if (clock.getElapsedTime().asSeconds() < (timeBeforeCollision - coastingTime))
		{
			velocity += acceleration * deltatime;
		}
		else {
			if (clock.getElapsedTime().asSeconds() < (timeBeforeCollision - 1)) {
				velocity = finalVelocity;
			}
		}
		circle.move(velocity * deltatime, 0.f);
	}

	void draw(sf::RenderWindow& window) 
	{
		window.draw(circle);
	}

	bool detectCollision(Ball other)
	{
		return circle.getGlobalBounds().intersects(other.circle.getGlobalBounds());
	}

	void resolveCollision(Ball &other)
	{
		sf::FloatRect intersection;
		circle.getGlobalBounds().intersects(other.circle.getGlobalBounds(), intersection);
		if (circle.getPosition().x > other.circle.getPosition().x)
		{
			circle.move(intersection.width, 0.f);
		} 
		else
		{
			other.circle.move(intersection.width, 0.f);
		}
	}

	float velocity = 0;

private:

	sf::CircleShape circle;
	sf::Clock clock;
	float finalVelocity;
	float mass;
	float acceleration;
	float radius;

};


int main() 
{
	float WindowResolution[2] = { 1280, 800 };

	float velocity1 = 0;
	float velocity2 = 0;
	float saveVelocity = 0;
	float mass1 = 1;
	float mass2 = 1;
	float vf1;
	float vf2;
	bool submit = false;
	bool renderingResult = false;
	bool hesADumDum = false;

	Ball ball1(sf::Color::Red);
	Ball ball2(sf::Color::Blue);

	sf::Font font;
	font.loadFromFile("Arial.ttf");
	sf::Text timer("0.00", font);
	timer.setFillColor(sf::Color::Black);

	//set Window
	sf::RenderWindow window(sf::VideoMode(WindowResolution[0], WindowResolution[1]), "Collision");
	sf::View viewPort(sf::FloatRect({}, { WindowResolution[0], WindowResolution[1] }));
	window.setView(viewPort);

	
	//Boing Sound
	sf::SoundBuffer buffer;
	buffer.loadFromFile("Cartoon Sound Effect - Bounce.wav");
	sf::Sound soundEffect;
	soundEffect.setBuffer(buffer);

	ImGui::SFML::Init(window);
	ImGui::SetNextWindowPos({ 0.f,0.f });

	sf::Clock clock;
	sf::Clock stopwatch;


	while (window.isOpen())
	{
		window.clear(sf::Color::White);
		sf::Event evnt;

		while (window.pollEvent(evnt))
		{
			ImGui::SFML::ProcessEvent(evnt);

			switch (evnt.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::Resized:
				WindowResolution[0] = evnt.size.width;
				WindowResolution[1] = evnt.size.height;
				viewPort.setSize(WindowResolution[0], WindowResolution[1]);
				viewPort.setCenter(WindowResolution[0] / 2, WindowResolution[1] / 2);
				window.setView(viewPort);
				break;
			}
		}

		float reset = clock.restart().asSeconds();

		ImGui::SFML::Update(window, sf::seconds(reset));

		ImGui::Begin("Perfectly Elastic Collision", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(1);

		ImGui::InputFloat("Ball 1 velocity", &velocity1, 1.0f, 1.0f, "%.1f");
		ImGui::InputFloat("Ball 1 mass", &mass1, 1.0f, 1.0f, "%.1f");
		ImGui::InputFloat("Ball 2 velocity", &velocity2, 1.0f, 1.0f, "%.1f");
		ImGui::InputFloat("Ball 2 mass", &mass2, 1.0f, 1.0f, "%.1f");

		if (ImGui::Button("ClicK ME!")) {


			if ((mass1 < 0) || (mass2 < 0) || (velocity1 == velocity2)) 
			{	
				hesADumDum = true;
			}
			else 
			{
				hesADumDum = false;
				renderingResult = false;
				submit = true;
				stopwatch.restart();

				float accelerationBall1 = velocity1 / (timeBeforeCollision - coastingTime);
				float accelerationBall2 = velocity2 / (timeBeforeCollision - coastingTime);
				float PositionBall1 = ((1.f / 2) * accelerationBall1 * std::pow((timeBeforeCollision - coastingTime), 2) + velocity1 * (coastingTime));
				float PositionBall2 = ((1.f / 2) * accelerationBall2 * std::pow((timeBeforeCollision - coastingTime), 2) + velocity2 * (coastingTime));

				//figureout where the balls should be created depending on velocity
				if (velocity1 >= velocity2)
				{
					ball1.initializeBall({ (-PositionBall1) + (WindowResolution[0]/2.f) - ((std::sqrt(mass1)) * 10), WindowResolution[1] / 2.f }, accelerationBall1, velocity1, mass1);
					ball2.initializeBall({ (-PositionBall2) + (WindowResolution[0]/2.f) + ((std::sqrt(mass2)) * 10), WindowResolution[1] / 2.f }, accelerationBall2, velocity2, mass2);
				}
				else 
				{
					ball1.initializeBall({ (-PositionBall1) + 512 + ((std::sqrt(mass1)) * 10), 400.f }, accelerationBall1, velocity1, mass1);
					ball2.initializeBall({ (-PositionBall2) + 512 - ((std::sqrt(mass2)) * 10), 400.f }, accelerationBall2, velocity2, mass2);
				}

				//quadratic formula to figureout velocity after collision;
				float a = ((mass2 / 2.f) + ((std::pow((mass2), 2) / (2.f * mass1))));
				float b = (-(2.f * (mass1 * velocity1 + mass2 * velocity2) * mass2 / (2.f * mass1)));
				float c = ((std::pow((mass1 * velocity1 + mass2 * velocity2), 2) / (2.f * mass1)) - ((mass1 / 2.f) * std::pow(velocity1, 
					2) + (mass2 / 2.f) * std::pow(velocity2, 2)));
				
				//quadratic formula's +- depends on which variable is bigger
				if (velocity1 >= velocity2)
				{
					vf2 = ((-b + std::sqrt(std::pow(b, 2) - 4.f * a * c)) / (2.f * a));
					vf1 = ((mass1 * velocity1 + mass2 * velocity2) - mass2 * vf2) / mass1;
				}
				else {
					vf2 = ((-b - std::sqrt(std::pow(b, 2) - 4.f * a * c)) / (2.f * a));
					vf1 = ((mass1 * velocity1 + mass2 * velocity2) - mass2 * vf2) / mass1;
				}
				std::cout << vf1 << " " << vf2 << std::endl;
			}

		}

		ImGui::End();
		if (hesADumDum)
		{
			ImGui::SetNextWindowPos({ 200.f,400.f });
			ImGui::Begin("DUMMY!", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(3);
			ImGui::Text("Thats the wrong number you dumdum");
			ImGui::End();
		}
		
		//Happens after user hits the submit button
		if (submit)
		{	
			ball1.update(reset);
			ball2.update(reset);

			if (ball1.detectCollision(ball2))
			{
				
				soundEffect.play();
				soundEffect.setPlayingOffset(sf::seconds(4.f));


				ball1.resolveCollision(ball2);
				ball1.velocity = vf1;
				ball2.velocity = vf2;
				renderingResult = true;
			}

		}

		if (soundEffect.getPlayingOffset().asSeconds() > 4.5f)
		{
			soundEffect.stop();
		}
		
		if (renderingResult) {
			ImGui::SetNextWindowPos({ WindowResolution[0] - 300.f ,100.f });
			ImGui::Begin("Final Result", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(1);
			ImGui::Text("Final Velocity of First Ball: %.2f m/s", vf1);
			ImGui::Text("Final Velocity of Second Ball: %.2f m/s", vf2);
			ImGui::End();
		}

		//Credits
		ImGui::SetNextWindowPos({ WindowResolution[0] - 400.f, WindowResolution[1] - 100.f });
		ImGui::Begin("Created By", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(2);
		ImGui::Text("Ahmed Raiyan & Sarah Yuniar");
		ImGui::End();
	

		//timer.setString(std::to_string(stopwatch.getElapsedTime().asSeconds()));
		//window.draw(timer);
		ball1.draw(window);
		ball2.draw(window);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}