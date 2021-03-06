#include "Vanisher.h"






Vanisher::Vanisher(color r_c, int id, int Arraival_Time, double health, double Power, REGION r_region, int d, int reload) : Enemy(r_c, r_region, d, reload)
{
	ID = id;
	arraival_time = Arraival_Time;
	Health = health;
	power = Power;
	type = 5;
	vanishing_time = arraival_time;
}


Vanisher::~Vanisher(void)
{
}

void Vanisher::Act()
{
	// defining the constant k of the equation depending on the health of the Vanisher
	double k = 0;
	if (Health > vanisher_health / 2)  k = 1;
	else   k = 0.5;

	double healt_deducted = target->GetHealth() - (k / Distance)*get_power();    // this will be the new health of the tower
	target->SetHealth(healt_deducted);			// calling set health function of the target tower
	 
	// managing visibility graphically

	if (!visible)
		Clr = DARKGREY;
	else
		Clr = PURPLE;
}


bool Vanisher::is_frozen()
{
	return frozen;
}

void Vanisher::Move()
{
	if (freeze_period == 0)
	{
		frozen = false;
	}

	// decrementing the distance depending on the frozen state
	if (!is_frozen())
	{
		DecrementDist(); // The frozern cannot shoot only but can move.
	}
	else // if it is frozen
	{
		freeze_period--;
	}
}

void Vanisher::set_target(Tower* Tower) // defining the target tower
{
	target = Tower;
}

void Vanisher::set_visible(bool visible_bool) {
	visible = visible_bool;
}

bool Vanisher::get_visible() {
	return visible;
}

void Vanisher::set_vanishing_time(int vt)
{
	vanishing_time = vt;
}

void Vanisher::increment_vanishing_time()
{
	vanishing_time++;
}

int Vanisher::get_vanishing_time()
{
	return vanishing_time;
}
