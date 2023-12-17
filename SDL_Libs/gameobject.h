
#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "hitbox.h"
#include <vector>

typedef void(*wrap_f_t)(void*);//primitive draw function

class GameObject2D{//virtual class, should only be inherited

protected:

	double x=0, y=0;//double for correct positioning
	double w=0, h=0;//for width and height
	
	std::vector<Hitbox*> hitboxes;//pointer to hitboxes

	bool update_on_move = true;

	wrap_f_t draw_f;




public:

	void moveUp(double);
	void moveDown(double);
	void moveRight(double);
	void moveLeft(double);

	void changeX(double);
	void changeY(double);
	void changeW(double);
	void changeH(double);

	//gives back x and y positions
	int X() const;
	int Y() const;
	int W() const;
	int H() const;

	bool hits(GameObject2D*);//checks if it hits other game object
	void draw(void*) const;
	void set_draw_f(wrap_f_t f);

	Hitbox* add_hitbox(int x, int y, int wr=0, int h=0, HitboxType ht = HitboxType::RECTANGULAR);//wr stands for width and radius, for both
	void remove_hitbox(Hitbox*);
	void set_hitboxes(std::vector<Hitbox*>);//sets all given hitboxes to the new pointer
	std::vector<Hitbox*>* get_hitboxes();

	void update_hitboxes(int xdelta=0, int ydelta=0, int wdelta=0, int hdelta=0);// updates all hitboxes accordingly, for circular hitboxes wdelta is radius change

	GameObject2D(double, double, double, double, wrap_f_t draw, bool update=true);//for initialization
	GameObject2D(const GameObject2D&);
	GameObject2D(const GameObject2D&&);
	virtual ~GameObject2D();

	GameObject2D& operator=(GameObject2D&& other);
	GameObject2D& operator=(GameObject2D& other);

};


GameObject2D::GameObject2D(double x_, double y_, double w_, double h_, wrap_f_t draw, bool update): x(x_), y(y_), w(w_), h(h_), draw_f(draw), update_on_move(update){}
GameObject2D::~GameObject2D(){

	for(Hitbox* ht : (hitboxes)) delete ht;

}

void GameObject2D::moveUp(double speed){y -= speed; if(update_on_move) update_hitboxes(0, -speed);}
void GameObject2D::moveDown(double speed){y += speed; if(update_on_move) update_hitboxes(0, speed);}
void GameObject2D::moveRight(double speed){x += speed; if(update_on_move) update_hitboxes(speed, 0);}
void GameObject2D::moveLeft(double speed){x -= speed; if(update_on_move) update_hitboxes(-speed, 0);}

int GameObject2D::X() const {return static_cast<int>(x);}
int GameObject2D::Y() const {return static_cast<int>(y);}
int GameObject2D::W() const {return static_cast<int>(w);}
int GameObject2D::H() const {return static_cast<int>(h);}

void GameObject2D::changeX(double x_){
	if(update_on_move){
		update_hitboxes(x_ - x, 0);
	}
	x = x_;

}
void GameObject2D::changeY(double y_){
	if(update_on_move){
		update_hitboxes(y_ - y, 0);
	}
	y = y_;
}
void GameObject2D::changeW(double w_){
	if(update_on_move){
		update_hitboxes(0, w_-w);
	}
	w = w_;
}
void GameObject2D::changeH(double h_){
	if(update_on_move){
		update_hitboxes(0, h_-h);
	}
	h = h_;
}


bool GameObject2D::hits(GameObject2D* other){
	
	for(Hitbox* h : (hitboxes)){

		for(Hitbox* oh : ((other->hitboxes))){

			if (h->hits(oh)) return true;
		}

	}

	return false;

}

void GameObject2D::set_hitboxes(std::vector<Hitbox*> ht){
	
	for (Hitbox* h : (hitboxes)) delete h;
	hitboxes = ht;
}

std::vector<Hitbox*>* GameObject2D::get_hitboxes(){
	return &hitboxes;
}

Hitbox* GameObject2D::add_hitbox(int x, int y, int wr, int h, HitboxType ht){


	switch (ht){

		case HitboxType::RECTANGULAR: 	{
											RectangularHitbox* rh = new RectangularHitbox(x, y, wr, h);
											hitboxes.push_back(dynamic_cast<Hitbox*>(rh));
											return dynamic_cast<Hitbox*>(rh);
											break;
										}	  
		case HitboxType::CIRCULAR:		{
											CircularHitbox* ch = new CircularHitbox(x, y, wr);
											hitboxes.push_back(dynamic_cast<Hitbox*>(ch));
											return dynamic_cast<Hitbox*>(ch);
											break;
										}

	}

	return nullptr;

}

void GameObject2D::remove_hitbox(Hitbox* hb){
	std::vector<Hitbox*>::iterator it = hitboxes.begin();
	while (it != hitboxes.end() && (*it) != hb){
		it++;
	}
	if (it != hitboxes.end()) hitboxes.erase(it);

}

void GameObject2D::update_hitboxes(int xdelta, int ydelta, int wdelta, int hdelta){

	for(Hitbox* h : (hitboxes)){

		switch(h->type){

			case HitboxType::RECTANGULAR: 	{
											RectangularHitbox* rh = dynamic_cast<RectangularHitbox*>(h);
											rh->x += xdelta;
											rh->y += ydelta;
											rh->w += wdelta;
											rh->h += hdelta; 
											break;
											}
			case HitboxType::CIRCULAR:		{
											CircularHitbox* ch = dynamic_cast<CircularHitbox*>(h);
											ch->x += xdelta;
											ch->y += ydelta;
											ch->r += wdelta;
											break;
											}
		}

	}

}


void GameObject2D::draw(void* args) const{
	draw_f(args);
}

void GameObject2D::set_draw_f(wrap_f_t f){
	draw_f = f;
}

GameObject2D::GameObject2D(const GameObject2D& other){
	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	draw_f = other.draw_f;

	for(Hitbox* h : hitboxes) delete h;
	std::vector<Hitbox*> empty;
	hitboxes = empty;

	for(Hitbox* h : other.hitboxes){
		if (h->type == HitboxType::RECTANGULAR){
			RectangularHitbox* hr = dynamic_cast<RectangularHitbox*>(h);
			add_hitbox(hr->x, hr->y, hr->w, hr->h, HitboxType::RECTANGULAR);
		}
		else if(h->type == HitboxType::CIRCULAR){
			CircularHitbox *cr = dynamic_cast<CircularHitbox*>(h);
			add_hitbox(cr->x, cr->y, cr->r, 0, HitboxType::CIRCULAR);

		}
	}

}

GameObject2D::GameObject2D(const GameObject2D&& other){
	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	draw_f = other.draw_f;

	for(Hitbox* h : hitboxes) delete h;
	std::vector<Hitbox*> empty;
	hitboxes = empty;

	for(Hitbox* h : other.hitboxes){
		if (h->type == HitboxType::RECTANGULAR){
			RectangularHitbox* hr = dynamic_cast<RectangularHitbox*>(h);
			add_hitbox(hr->x, hr->y, hr->w, hr->h, HitboxType::RECTANGULAR);
		}
		else if(h->type == HitboxType::CIRCULAR){
			CircularHitbox *cr = dynamic_cast<CircularHitbox*>(h);
			add_hitbox(cr->x, cr->y, cr->r, 0, HitboxType::CIRCULAR);

		}
	}

}

GameObject2D& GameObject2D::operator=(GameObject2D&& other){

	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	draw_f = other.draw_f;

	for(Hitbox* h : hitboxes) delete h;
	std::vector<Hitbox*> empty;
	hitboxes = empty;

	for(Hitbox* h : other.hitboxes){
		if (h->type == HitboxType::RECTANGULAR){
			RectangularHitbox* hr = dynamic_cast<RectangularHitbox*>(h);
			add_hitbox(hr->x, hr->y, hr->w, hr->h, HitboxType::RECTANGULAR);
		}
		else if(h->type == HitboxType::CIRCULAR){
			CircularHitbox *cr = dynamic_cast<CircularHitbox*>(h);
			add_hitbox(cr->x, cr->y, cr->r, 0, HitboxType::CIRCULAR);

		}
	}
	return (*this);

}

GameObject2D& GameObject2D::operator=(GameObject2D& other){

	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	draw_f = other.draw_f;

	for(Hitbox* h : hitboxes) delete h;
	std::vector<Hitbox*> empty;
	hitboxes = empty;

	for(Hitbox* h : other.hitboxes){
		if (h->type == HitboxType::RECTANGULAR){
			RectangularHitbox* hr = dynamic_cast<RectangularHitbox*>(h);
			add_hitbox(hr->x, hr->y, hr->w, hr->h, HitboxType::RECTANGULAR);
		}
		else if(h->type == HitboxType::CIRCULAR){
			CircularHitbox *cr = dynamic_cast<CircularHitbox*>(h);
			add_hitbox(cr->x, cr->y, cr->r, 0, HitboxType::CIRCULAR);

		}
	}
	return (*this);

}

#endif