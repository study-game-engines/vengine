#pragma once
#include <vector>
#include "IActorSystem.h"
#include "Actor.h"
#include "ActorSystemCache.h"
#include "Serialiser.h"
#include "Components/Component.h"

template <typename T>
struct ActorSystem : IActorSystem
{
	std::vector<T*> actors;

	ActorSystem()
	{
		std::string typeName = typeid(T).name();
		std::string delimiter = " ";
		std::string token = typeName.substr(typeName.find(delimiter) + 1);
		name = token;

		actorSystemCache.Add(typeid(T), this);
	}

	T* Add(T newActor = T(), Transform transform = Transform())
	{
		T* actor = new T(newActor);
		actors.push_back(actor);

		actor->actorSystem = this;
		actor->index = actors.size() - 1;
		actor->SetTransform(transform);
		actor->name = this->name + std::to_string(actor->index);

		return actor;
	}

	void Remove(int index)
	{
		for (Component* component : actors[index]->components)
		{
			component->Destroy();
		}

		delete actors[index];
		std::swap(actors[index], actors.back());
		actors[index]->index = index;
		actors[index]->name = this->name + std::to_string(index);
		actors.pop_back();
	}

	virtual void Tick(double deltaTime) override
	{
		for (T* actor : actors)
		{
			actor->Tick(deltaTime);
		}
	}

	virtual void Serialise(Serialiser& s) override
	{
		s.os << name << "\n"; //Use actorsystem name to create again from ActorSystemCache on Deserialise
		s.os << actors.size() << "\n"; //Write out num of actors to load the same amount on Deserialise

		for (T* actor : actors)
		{
			s.Serialise(actor->GetProps());
		}
	}

	virtual void Deserialise(Serialiser& s) override
	{
		for (Actor* actor : actors)
		{
			s.Deserialise(actor->GetProps());
		}
	}

	virtual void SerialiseActorTemplate(Serialiser& s, Actor* actor) override
	{
		auto props = actor->GetProps();
		s.os << this->name << "\n";
		s.Serialise(props);
	}

	virtual Actor* DeserialiseActorTemplate(Serialiser& s) override
	{
		T* actor = Add();
		s.Deserialise(actor->GetProps());
		actor->name = this->name + std::to_string(actor->index);
		return (Actor*)actor;
	}

	virtual Actor* FindActorByName(std::string actorName) override
	{
		for (T* actor : actors)
		{
			if (actor->name == actorName)
			{
				return (Actor*)actor;
			}
		}

		return nullptr;
	}

	virtual Actor* SpawnActor(Transform transform) override
	{
		Actor* actor = (Actor*)Add(T(), transform);
		return actor;
	}

	virtual Actor* SpawnActorTemplate() override
	{
		return (Actor*)Add();
	}

	virtual void GetActors(std::vector<Actor*>& outActors) override
	{
		for (T* actor : actors)
		{
			outActors.push_back(actor);
		}
	}

	virtual void Cleanup() override
	{
		for (T* actor : actors)
		{
			delete actor;
		}

		actors.clear();
	}
};

#define ACTOR_SYSTEM(type) inline static ActorSystem<type> system; \
virtual void Destroy() override { system.Remove(index); } \
