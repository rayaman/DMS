#include "pch.h"
#include "dms.h"

//#include <chrono>
using namespace dms;
//typedef void(*FNPTR)();
//using namespace std::chrono;
//value invokeTest(void* self, dms_state* state, dms_args* args) {
//    utils::print(args->args[0].getPrintable());
//    return "I work!";
//}
//value concat(void* self, dms_state* state, dms_args* args) {
//    std::stringstream str;
//    for (size_t i = 0; i < args->size() - 1; i++)
//        str << args->args[i].getPrintable();
//    return value(str.str());
//}
//void draw(sf::RenderWindow& window) {
//    sf::CircleShape shape(60.f);
//    shape.setFillColor(sf::Color(150, 50, 250));
//
//    // set a 10-pixel wide orange outline
//    shape.setOutlineThickness(10.f);
//    shape.setOutlineColor(sf::Color(250, 150, 100));
//    window.draw(shape);
//}
int main()
{
    // TODO fix disp cmd to handle the standard


    /*milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    );
    utils::print(ms.count());*/
    
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    // Load audio stuff
    dms::audio::init(state);
    state->dump();
    state->run();
    
    // create the window
    //sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    //// run the program as long as the window is open
    //while (window.isOpen())
    //{
    //    // check all the window's events that were triggered since the last iteration of the loop
    //    sf::Event event;
    //    while (window.pollEvent(event))
    //    {
    //        // "close requested" event: we close the window
    //        if (event.type == sf::Event::Closed)
    //            window.close();
    //    }

    //    // clear the window with black color
    //    window.clear(sf::Color::Black);

    //    // draw everything here...
    //    draw(window);

    //    // end the current frame
    //    window.display();
    //}

    /*HINSTANCE hInst = LoadLibrary(L"C:\\Users\\rayam\\Desktop\\test.dll");
    if (!hInst) {
        std::cout << "\nCould not load the library!";
        return EXIT_FAILURE;
    }

    FNPTR fn = (FNPTR)GetProcAddress(hInst, "_init");
    if (!fn) {
        std::cout << "\nCould not load function from library!";
        return EXIT_FAILURE;
    }

    fn();*/
}