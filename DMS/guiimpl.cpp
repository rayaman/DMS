#include "gui.h"
namespace gui {
	gui& gui::newFrame(DualDim offset, DualDim scale) {
		gui* fb = new framebase(offset, scale);
		fb->Parent = this;
		Children.push_back(fb);
		return *fb;
	}
	// Draw Everything
	gui _Root;
	std::vector<gui*> childs; // This tends to always grow, rarely strinking. So make it once so we arent allocating meme each loop!
	void Draw(sf::RenderWindow* window) {
		_Root.AbsoluteSize = window->getSize();
		childs = _Root.GetAllChildren();
		for (int i = 0; i < childs.size(); i++) {
			childs[i]->Draw(window);
		}
	}
	gui& Root() {
		return _Root;
	}
}