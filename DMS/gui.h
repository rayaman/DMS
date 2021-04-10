#pragma once
#include "pch.h"
#include "dms_state.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "multibase.h"
namespace gui {
	struct gframe;
	struct gtext;
	struct gimage;
	using namespace std;
	using namespace dms;
	/*
		textbox
		textbutton
		textlabel

		frame

		imagelabel
		imagebutton
		
		anim/sprite
		video


	*/
	enum class gui_types{root, frame,text,image,sprite,video};
	struct Dim {
		const float x = 0;
		const float y = 0;
		void init() {
			OnValueChanged = multi::connection<Dim*>("DimConnection");
		}
		Dim() : x(0), y(0) { init(); }
		Dim(const Dim &d) : x(d.x),y(d.y) { init(); }
		Dim(float x, float y) : x(x),y(y) { init(); }
		inline void Set(Dim d) { if (x == d.x && y == d.y) return; const_cast<float&>(x) = d.x; const_cast<float&>(y) = d.y; OnValueChanged.fire(this); }
		inline void Set(float x, float y) { if (this->x == x && this->y == y) return; const_cast<float&>(this->x) = x; const_cast<float&>(this->y) = y; OnValueChanged.fire(this);}
		void operator=(const Dim& dd) {
			Set(dd);
		}
		void operator=(const sf::Vector2u& v) {
			Set(v.x, v.y);
		}
		operator sf::Vector2u () {
			return sf::Vector2u((int)x, (int)y);
		}
		operator sf::Vector2f() {
			return sf::Vector2f(x, y);
		}
		multi::connection<Dim*> OnValueChanged;
	};
	struct DualDim {
		Dim Position;
		Dim Size;
		void init() {
			OnValueChanged = multi::connection<DualDim*>("DualDimConnection");
			Position.OnValueChanged += [&](Dim* d) {
				std::cout << "Hi ddp" << std::endl;
				OnValueChanged.fire(this);
			};
			Size.OnValueChanged += [&](Dim* d) {
				std::cout << "Hi dds" << std::endl;
				OnValueChanged.fire(this);
			};
		}
		DualDim() : Position(0,0),Size(0,0) { init(); }
		DualDim(const DualDim& d) : Position(d.Position.x,d.Position.y), Size(d.Size.x,d.Size.y) {init(); }
		DualDim(Dim pos, Dim size) : Position(pos.x,pos.y), Size(size.x,size.y) { init(); }
		DualDim(float x, float y, float x2, float y2) : Position(x,y),Size(x2,y2) { init(); }
		inline void Set(DualDim d){ if (d.Position.x == this->Position.x && d.Position.y == this->Position.y && d.Size.x == this->Size.x && d.Size.y == this->Size.y) return; Position.Set(d.Position); Size.Set(d.Size);}
		inline void Set(Dim pos, Dim size) { Position.Set(pos); Size.Set(size);}
		inline void Set(float x, float y, float x2, float y2) { Position.Set(x, y), Size.Set(x2, y2); }
		DualDim operator=(const DualDim& dd) {
			Set(dd);
			return *this;
		}
		multi::connection<DualDim*> OnValueChanged;
	};
	struct MouseStats {
		float x = 0;
		float y = 0;
		float dx = 0;
		float dy = 0;
		char button = 0;
		MouseStats(Dim pos, char b) {
			x = pos.x;
			y = pos.y;
			button = b;
		}
		MouseStats(float xx, float yy, char b) {
			x = xx;
			y = yy;
			button = b;
		}
		MouseStats(float xx, float yy, float dxx, float dyy) {
			x = xx;
			y = yy;
			dx = dxx;
			dy = dyy;
			button = 0;
		}
	};
	struct Keyboard {
		//
	};
	
	class gui {

	public:
		gui_types Type = gui_types::root;
		DualDim Offset;
		DualDim Scale;
		Dim AbsolutePosition;
		Dim AbsoluteSize;
		gui* Parent = nullptr;
		vector<gui*> Children;
		bool Visible = true;
		bool Active = true;

		//map<string, gui*> Named;

		gui();
		gui(DualDim offset, DualDim scale) : gui() { Offset.Set(offset); Scale.Set(scale);}
		gui(float x,float y,float w,float h,float sx,float sy,float sw,float sh) : gui() {
			Offset.Set(x, y, w, h);
			Scale.Set(sx, sy, sw, sh);
		}
		
		virtual void Draw(sf::RenderWindow* window) {
			return; // Override this.
		}
		virtual void updateValues(DualDim offset, DualDim scale) {
			return; // Override this.
		}
		inline vector<gui*>* GetChildren() {return &Children;}
		inline void GetAllChildren(vector<gui*>* ref) {
			vector<gui*>* temp = GetChildren();
			for (size_t i = 0; i < temp->size(); i++) {
				ref->push_back((*temp)[i]);
				(*temp)[i]->GetAllChildren(ref);
			}
		}
		// Constructors
		gframe& newFrame(DualDim offset, DualDim scale = { 0,0,0,0 });

		//Standard mathods
		virtual void setColor(sf::Color c) {};
		void SetDualDim(DualDim offset, DualDim scale) { Offset.Set(offset); Scale.Set(scale); }

		//Connections and objects to inheret
		multi::connection<MouseStats> OnPressed;
		multi::connection<MouseStats> OnReleased;
		multi::connection<MouseStats> OnPressedOuter;
		multi::connection<MouseStats> OnReleasedOuter;
		multi::connection<MouseStats> OnMousedMoved;
		multi::connection<MouseStats> WhileHovering;

		multi::connection<MouseStats> OnMouseEnter;
		multi::connection<MouseStats> OnMouseExit;

		multi::connection<gui*> OnUpdate;
	};

	

	struct gframe : public gui {
		sf::RectangleShape rect;
		gframe(DualDim offset, DualDim scale, gui* parent) : gui(offset, scale) {
			Parent = parent;
			Type = gui_types::frame;
			updateValues(Offset, Scale);
			//rect.setSize(offset.Size);
			//rect.setPosition(offset.Position);
		}
		inline void SetColor(sf::Color& c) {
			rect.setFillColor(c);
		}
		inline void SetBorderColor(sf::Color& c) {
			rect.setOutlineColor(c);
		}
		inline void SetBorderSize(float bs) {
			rect.setOutlineThickness(bs);
		}
		inline void SetAlpha(float alpha) {
			auto c = rect.getFillColor();
			c.a = alpha;
			SetColor(c);
			c = rect.getOutlineColor();
			c.a = alpha;
			SetBorderColor(c);
		}
		inline void Draw(sf::RenderWindow* window) override {
			window->draw(this->rect);
		}
	private:
		void updateValues(DualDim offset, DualDim scale) override {
			AbsolutePosition = Dim(Parent->AbsolutePosition.x*scale.Position.x+offset.Position.x,Parent->AbsolutePosition.y*scale.Position.y+offset.Position.y);
			AbsoluteSize = Dim(Parent->AbsoluteSize.x * scale.Size.x + offset.Size.x, Parent->AbsoluteSize.y * scale.Size.y + offset.Size.y);
			rect.setPosition(AbsolutePosition);
			rect.setSize(AbsoluteSize);
		}
	};
	struct gimage : public gframe {
		sf::Texture texture;
		gimage(std::string imagepath, DualDim offset, DualDim scale, gui* parent) : gframe(offset,scale,parent) {
			Type = gui_types::image;
			texture.loadFromFile(imagepath);
			sf::Sprite sprite;
			sf::Vector2u size = texture.getSize();
			sprite.setTexture(texture);
			sprite.setOrigin(0, 0);
		}
	};
	struct gtext : public gframe {
		gtext(std::string text, sf::Font font, DualDim offset, DualDim scale,gui* parent) : gframe(offset, scale, parent) {
			Type = gui_types::text;
		}
	};
	gui& Root();
	void Draw(sf::RenderWindow* window);
}