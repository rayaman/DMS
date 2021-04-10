#include "pch.h"
#include "gui.h"
#include "actors.h"
#include <thread>
namespace gui {
	int GuiElementCount = 0;
	const size_t ANChilds = 128;
	gui::gui() {
		GuiElementCount += 1;
		Offset.OnValueChanged.connect([&](DualDim* d) {
			std::cout << "hey hey hey" << std::endl;
			updateValues(Offset, Scale);
		});
		Scale.OnValueChanged.connect([&](DualDim* d) {
			std::cout << "hey hey hey" << std::endl;
			updateValues(Offset, Scale);
		});
	}
	gframe& gui::newFrame(DualDim offset, DualDim scale) {
		gframe* fb = new gframe(offset, scale,this);
		Children.push_back(fb);
		return *fb;
	}
	// Draw Everything
	gui _Root;
	int lastcount = -1;
	std::vector<gui*>* childs = new std::vector<gui*>(ANChilds); // This tends to always grow, rarely strinking. So make it once so we arent allocating memes each loop!
	void Draw(sf::RenderWindow* window) {
		_Root.AbsoluteSize = window->getSize();
		if (lastcount != GuiElementCount) {
			std::cout << "New Element Added/Removed!" << std::endl;
			lastcount = GuiElementCount;
			childs->clear();
			_Root.GetAllChildren(childs);
		}
		for (int i = 0; i < childs->size(); i++) {
			if ((*childs)[i]->Visible);
				(*childs)[i]->Draw(window);
		}
	}
	/*multi::runner runner;
	multi::loop updateloop(&runner, [](multi::loop* loop) {
		_Root.GetAllChildren(childs);
		for (int i = 0; i < childs.size(); i++) {
			if (childs[i]->Active)
				childs[i]->OnUpdate.fire(childs[i]);
		}
	});
	std::thread thread([]() {
		runner.mainloop();
	});*/
	gui& Root() {
		return _Root;
	}
}