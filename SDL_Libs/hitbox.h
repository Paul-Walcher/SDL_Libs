
#ifndef __HITBOX__
#define __HITBOX__

#include <cmath>
#include <iostream>

class Hitbox;
class RectangularHitbox;
class CircularHitbox;

static bool circle_hits_rect(CircularHitbox*, RectangularHitbox*);

enum HitboxType{
					NO_HITBOX,
					RECTANGULAR,
					CIRCULAR
				};


//superclass
class Hitbox{

public:

	HitboxType type = HitboxType::NO_HITBOX;//for virtual class

	virtual bool hits(Hitbox* other)=0;
	virtual ~Hitbox(){}

};

class RectangularHitbox : public Hitbox{

public:

	int x=0, y=0, w=0, h=0;

	RectangularHitbox(int x, int y, int w, int h);
	bool hits(Hitbox* other);

};

class CircularHitbox : public Hitbox{

public:

	int x=0, y=0, r=0;

	CircularHitbox(int x, int y, int r);
	bool hits(Hitbox* other);

};

CircularHitbox::CircularHitbox(int x_, int y_, int r_): x(x_), y(y_), r(r_){
	type = HitboxType::CIRCULAR;
}
bool CircularHitbox::hits(Hitbox* o){
	

	switch(o->type){

		case HitboxType::CIRCULAR: 	{
									
										CircularHitbox* ch = dynamic_cast<CircularHitbox*>(o);
										int xdist = (x-ch->x)*(x-ch->x), ydist = (y-ch->y)*(y-ch->y);
										double distance = sqrt(xdist+ydist);
										if (distance < (r+ch->r)) return true;
										else return false;
										break;
									}
		case HitboxType::RECTANGULAR:	{
											RectangularHitbox* other = dynamic_cast<RectangularHitbox*>(o);
											return circle_hits_rect(this, other);
										}

	}

	return false;
}

RectangularHitbox::RectangularHitbox(int x_, int y_, int w_, int h_): x(x_), y(y_), w(w_), h(h_){
	type = HitboxType::RECTANGULAR;
}
bool RectangularHitbox::hits(Hitbox* o){

	switch(o->type){

		case HitboxType::RECTANGULAR: 	{
										RectangularHitbox* other = dynamic_cast<RectangularHitbox*>(o);

										int leftA=x, rightA=x+w, topA=y, bottomA=y+h, leftB=other->x, rightB=other->x+other->w, topB=other->y, bottomB=other->y+other->h; 
										if (rightA <= leftB || leftA >= rightB || topA >= bottomB || bottomA <= topB) return false;
										return true;
										break;
									 	}		 
		case HitboxType::CIRCULAR:    	{
										CircularHitbox* other = dynamic_cast<CircularHitbox*>(o);
										return circle_hits_rect(other, this);
										break;
										}


	}

	return false;

}

static bool circle_hits_rect(CircularHitbox* ch, RectangularHitbox* rh){

	int cx, cy;//closest x and y

	if (ch->x < rh->x) cx = rh->x;
	else if (ch->x > rh->x+rh->w) cx = rh->x+rh->w;
	else cx = ch->x;

	if(ch->y < rh->y) cy = rh->y;
	else if(ch->y > rh->y+rh->h) cy = rh->y+rh->h;
	else cy = ch->y;

	int cdx = ch->x-cx;
	int cdy = ch->y-cy;

	if ((cdx*cdx + cdy*cdy) < ch->r*ch->r) return true;
	else return false;


}

#endif