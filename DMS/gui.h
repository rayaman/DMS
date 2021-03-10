#pragma once
#include "window.h"
#include "pch.h"
#include "dms_state.h"
namespace gui {
	using namespace std;
	using namespace dms;
	int __ElementCount = 0;
	void init(dms_state* state) {
		// While very unlikely it is possible multiple windows to be created! It might be a good idea to support it sometime down the line
		window::OnWindowCreated += [](sf::RenderWindow* window) {

		};
	}
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
	enum class gui_types{frame,text,text,image,sprite,video};
	struct Dim {
		float x;
		float y;
		Dim() {x = 0;y = 0;}
		Dim(const Dim &d) { x = d.x; y = d.y;}
		Dim(float x, float y) { this->x = x; this->y = y;}
		inline void Set(Dim d) {x = d.x; y = d.y;}
		inline void Set(float x, float y) { this->x = x; this->y = y;}
	};
	struct DualDim {
		Dim Position;
		Dim Size;
		DualDim() {Position.x = 0;Position.y = 0;Size.x = 0;Size.y = 0;}
		DualDim(const DualDim& d) { Position = d.Position; Size = d.Size;}
		DualDim(Dim pos, Dim size) { Position = pos; Size = size;}
		DualDim(float x, float y, float x2, float y2) { Position.x = x; Position.y = y; Size.x = x2; Size.y = y2; }
		inline void Set(DualDim d){ Position = d.Position; Size = d.Size;}
		inline void Set(Dim pos, Dim size) {Position.x = pos.x;Position.y = pos.y;Size.x = size.x;Size.y = size.y;}
		inline void Set(float x, float y, float x2, float y2) { Position.x = x; Position.y = y; Size.x = x2; Size.y = y2;}
	};
	struct MouseStats {
		float x;
		float y;
		float dx;
		float dy;
		char button;
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
	struct guibase {
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
	class gui : guibase {

	public:
		gui_types Type;
		DualDim Offset;
		DualDim Scale;
		DualDim Absolute;
		vector<gui> Children;
		bool Visible = true;

		//map<string, gui*> Named;

		gui() {__ElementCount++;}
		gui(DualDim offset, DualDim scale) { Offset = offset; Scale = scale;}
		gui(float x,float y,float w,float h,float sx,float sy,float sw,float sh){Offset.Position.x = x;Offset.Position.y = y;Offset.Size.x = w;Offset.Size.y = h;Scale.Position.x = sx;Scale.Position.y = sy;Scale.Size.x = sw;Scale.Size.y = sh;}
		void SetDualDim(DualDim offset,DualDim scale) {Offset = offset;Scale = scale;}
		
		virtual void Draw() {
			return; // Override this.
		}

		inline vector<gui>& GetChildren() {return Children;}
		inline void GetAllChildren(vector<gui*>& ref) {
			auto temp = GetChildren();
			for (int i = 0; i < temp.size(); i++) {
				ref.push_back(&temp[i]);
				temp[i].GetAllChildren(ref);
			}
		}
		inline vector<gui*> GetAllChildren() {
			auto temp = GetChildren();
			vector<gui*> everything;
			everything.reserve(__ElementCount); // Reserve all the current elements that exist
			for (int i = 0; i < temp.size(); i++) {
				everything.push_back(&temp[i]);
				temp[i].GetAllChildren(everything);
			}
			return everything;
		}
	};
	// Draw Everything
	gui _Root;
	void Draw() {
		auto childs = _Root.GetAllChildren();
		for (int i = 0; i < childs.size(); i++) {
			childs[i]->Draw();
		}
	}
}