#pragma once
#include "pch.h"
#include "dms_state.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "multibase.h"
namespace gui {
	using namespace std;
	using namespace dms;
	/// <summary>
	/// This is the number of gui elements that exist! It isn't constant, but set as a read only variable.
	/// </summary>
	const int GuiElementCount = 0;
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
		Dim() : x(0), y(0) {}
		Dim(const Dim &d) : x(d.x),y(d.y) {}
		Dim(float x, float y) : x(x),y(y) {}
		inline void Set(Dim d) { const_cast<float&>(x) = d.x; const_cast<float&>(y) = d.y; OnValueChanged.fire(this); }
		inline void Set(float x, float y) { const_cast<float&>(this->x) = x; const_cast<float&>(this->y) = y; OnValueChanged.fire(this);}
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
			Position.OnValueChanged += [&](Dim* d) {
				OnValueChanged.fire(this);
			};
			Size.OnValueChanged += [&](Dim* d) {
				OnValueChanged.fire(this);
			};
		}
		DualDim() : Position(0,0),Size(0,0) { init(); }
		DualDim(const DualDim& d) : Position(d.Position.x,d.Position.y), Size(d.Size.x,d.Size.y) {init(); }
		DualDim(Dim pos, Dim size) : Position(pos.x,pos.y), Size(size.x,size.y) { init(); }
		DualDim(float x, float y, float x2, float y2) : Position(x,y),Size(x2,y2) { init(); }
		inline void Set(DualDim d){ Position.Set(d.Position); Size.Set(d.Size);OnValueChanged.fire(this);}
		inline void Set(Dim pos, Dim size) { Position.Set(pos); Size.Set(size); OnValueChanged.fire(this);}
		inline void Set(float x, float y, float x2, float y2) { Position.Set(x, y), Size.Set(x2, y2); OnValueChanged.fire(this);}
		DualDim operator=(const DualDim& dd) {
			Set(dd);
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

		//map<string, gui*> Named;

		gui() {
			const_cast<int&>(GuiElementCount) += 1;
		
		}
		gui(DualDim offset, DualDim scale) : gui() { Offset.Set(offset); Scale.Set(scale);}
		gui(float x,float y,float w,float h,float sx,float sy,float sw,float sh) : gui() {
			Offset.Set(x, y, w, h);
			Scale.Set(sx, sy, sw, sh);
		}
		void SetDualDim(DualDim offset,DualDim scale) {Offset = offset;Scale.Set(scale);}
		
		virtual void Draw(sf::RenderWindow* window) {
			return; // Override this.
		}

		inline vector<gui*>& GetChildren() {return Children;}
		inline void GetAllChildren(vector<gui*>& ref) {
			auto temp = GetChildren();
			for (int i = 0; i < temp.size(); i++) {
				ref.push_back(temp[i]);
				temp[i]->GetAllChildren(ref);
			}
		}
		inline vector<gui*> GetAllChildren() {
			auto temp = GetChildren();
			vector<gui*> everything;
			everything.reserve(GuiElementCount); // Reserve Space for all the current elements that exist
			for (int i = 0; i < temp.size(); i++) {
				everything.push_back(temp[i]);
				temp[i]->GetAllChildren(everything);
			}
			return everything;
		}

		gui& newFrame(DualDim offset, DualDim scale);

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

	

	struct framebase : public gui {
		sf::RectangleShape rect;
		framebase(DualDim offset, DualDim scale) : gui(offset,scale) {
			Type = gui_types::frame;
			rect.setFillColor(sf::Color(120, 120, 120, 255));
			rect.setOutlineColor(sf::Color::Red);
			rect.setSize(offset.Size);
			rect.setPosition(offset.Position);
			Offset.OnValueChanged += [&](DualDim* o) {
				rect.setSize(o->Size);
				rect.setPosition(o->Position);
			};
			Scale.OnValueChanged += [](DualDim* s) {

			};
		}
		void Draw(sf::RenderWindow* window) override {
			window->draw(rect);
		}
	};
	struct imagebase : public framebase {
		sf::Texture texture;
		imagebase(std::string imagepath, DualDim offset, DualDim scale) : framebase(offset,scale) {
			Type = gui_types::image;
			texture.loadFromFile(imagepath);
			sf::Sprite sprite;
			sf::Vector2u size = texture.getSize();
			sprite.setTexture(texture);
			sprite.setOrigin(0, 0);
		}
	};
	struct textbase : public framebase {
		textbase(std::string text, sf::Font font, DualDim offset, DualDim scale) : framebase(offset, scale) {
			Type = gui_types::text;
		}
	};
	gui& Root();
	void Draw(sf::RenderWindow* window);
}