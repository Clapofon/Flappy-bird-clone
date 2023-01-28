#pragma once

#include <Box2D/Box2D.h>

#include "EntityData.h"


class ContactListener : public b2ContactListener
{
public:
	ContactListener() {}
	~ContactListener() {}

	virtual void BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		EntityData* fixtureAData = static_cast<EntityData*>(fixtureA->GetBody()->GetUserData());
		EntityData* fixtureBData = static_cast<EntityData*>(fixtureB->GetBody()->GetUserData());

		if (fixtureAData && fixtureBData)
		{
			if (fixtureAData->type == "floor" && fixtureBData->type == "player")
			{
				fixtureBData->contact = true;
				fixtureBData->with = "floor";
				fixtureB->GetBody()->SetUserData(fixtureBData);
			}

			if (fixtureAData->type == "pipe" && fixtureBData->type == "player")
			{
				fixtureBData->contact = true;
				fixtureBData->with = "pipe";
				fixtureB->GetBody()->SetUserData(fixtureBData);
			}

			if (fixtureAData->type == "pipe_score_collider" && fixtureBData->type == "player")
			{
				fixtureAData->contact = true;
				fixtureAData->with = "player";
				fixtureA->GetBody()->SetUserData(fixtureAData);
			}
		}
	}

	virtual void EndContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		EntityData* fixtureAData = static_cast<EntityData*>(fixtureA->GetBody()->GetUserData());
		EntityData* fixtureBData = static_cast<EntityData*>(fixtureB->GetBody()->GetUserData());

		if (fixtureAData && fixtureBData)
		{
			if (fixtureAData->type == "floor" && fixtureBData->type == "player")
			{
				fixtureBData->contact = false;
				fixtureBData->with = "floor";
				fixtureB->GetBody()->SetUserData(fixtureBData);
			}

			if (fixtureAData->type == "pipe" && fixtureBData->type == "player")
			{
				fixtureBData->contact = false;
				fixtureBData->with = "pipe";
				fixtureB->GetBody()->SetUserData(fixtureBData);
			}

			if (fixtureAData->type == "pipe_score_collider" && fixtureBData->type == "player")
			{
				fixtureAData->contact = false;
				fixtureAData->with = "player";
				fixtureA->GetBody()->SetUserData(fixtureAData);
			}
		}
	}
};