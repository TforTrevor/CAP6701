#pragma once

#include "render_object.hpp"

#include <random>

struct Body
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float mass;
	float drag;

	RenderObject renderObject;
};

class ParticleSystem
{
public:
	ParticleSystem(std::shared_ptr<Model> model, std::shared_ptr<Material> material, int count);

	void update(float deltaTime);
	void setPosition(glm::vec3 newPos) { position = newPos; }
	void setSpawnRate(float newRate) { spawnRate = newRate; }
	std::vector<RenderObject> getRenderObjects();
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	float spawnRate = 1.0f;

	std::vector<Body> particleObjects;

	float currentTime = 0.0f;
	int poolIndex = 0;

	std::default_random_engine randEngine;

	glm::vec3 applyForces(Body body);
};