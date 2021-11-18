#include "particle_system.hpp"

ParticleSystem::ParticleSystem(std::shared_ptr<Model> model , std::shared_ptr<Material> material, int count)
{
	for (int i = 0; i < count; i++)
	{
		RenderObject object = { model, material };
		object.setEnabled(false);

		Body body{
			{0, 0, 0}, {0, 0, 0}, {0, 0, 0},
			1.0f, 0.1f,
			object
		};

		particleObjects.push_back(std::move(body));
	}
}

void ParticleSystem::update(float deltaTime)
{
	if (currentTime > spawnRate)
	{
		std::uniform_real_distribution<float> xDistro(-5.0f, 5.0f);
		std::uniform_real_distribution<float> yDistro(2.0f, 5.0f);
		std::uniform_real_distribution<float> zDistro(-6.0f, -3.0f);

		Body& body = particleObjects[poolIndex];
		body.renderObject.setEnabled(true);
		body.renderObject.setPosition(position);
		body.position = position;
		body.velocity = glm::vec3(xDistro(randEngine), yDistro(randEngine), zDistro(randEngine));

		currentTime = 0;
		poolIndex++;
		if (poolIndex >= particleObjects.size())
		{
			poolIndex = 0;
		}
	}

	for (Body& body : particleObjects)
	{
		if (body.renderObject.isEnabled())
		{
			glm::vec3 newPos = body.position + body.velocity * deltaTime + body.acceleration * (deltaTime * deltaTime * 0.5f);
			glm::vec3 newAccel = applyForces(body);
			glm::vec3 newVel = body.velocity + (body.acceleration + newAccel) * (deltaTime * 0.5f);

			body.position = newPos;
			body.velocity = newVel;
			body.acceleration = newAccel;

			body.renderObject.setPosition(position + body.position);
		}		
	}

	currentTime += deltaTime;
}

glm::vec3 ParticleSystem::applyForces(Body body)
{
	glm::vec3 gravity = glm::vec3(0, -9.81f, 0);
	glm::vec3 dragForce = 0.5f * body.drag * (body.velocity * abs(body.velocity));
	glm::vec3 dragAccel = dragForce / body.mass;
	return gravity - dragAccel;
}

std::vector<RenderObject> ParticleSystem::getRenderObjects()
{
	std::vector<RenderObject> objects;

	for (Body& body : particleObjects)
	{
		objects.push_back(body.renderObject);
	}

	return objects;
}