#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "pch.h"
#include "dms.h"
#include "actors.h"

using namespace dms;
int main()
{
    // TODO fix disp cmd to handle the standard
    
    multi::runner run;

    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    // Load audio stuff
    dms::audio::init(state);
    state->dump();
    memory* mem = state->getMem();

    sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "Background Test");

    sf::Texture backgroundIMG;
    backgroundIMG.loadFromFile("background.jpg");
    sf::Sprite sprite;
    sf::Vector2u size = backgroundIMG.getSize();
    sprite.setTexture(backgroundIMG);
    sprite.setOrigin(0, 0);

    state->invoker.registerFunction("setBG", [&](void* self, dms::dms_state* state, dms::dms_args* args) -> dms::value {
        if (args->size() > 0 && (*args).args[0].type == dms::datatypes::string) {
            try {
                backgroundIMG.loadFromFile((*args).args[0].getString());
            }
            catch (std::exception e) {
                return dms::value(dms::utils::concat("Cannot load texture: \"", (*args).args[0].getString(), "\" from file!"), dms::datatypes::error);
            }
            return dms::nil;
        }
        return dms::value("Invalid argument, string expected for first argument!", dms::datatypes::error);
    });

    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::Text text("Hello this is a test|!", font);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Black);
    auto test = text.getGlobalBounds();
    text.setPosition(11, 768 - 110 - (test.height-test.top));
    ////std::cout << test << std::endl;

    state->OnText += [&](dms::message msg) {
        text.setString(dms::utils::concat(msg.chara->getName(),": ",msg.text));
    };
    state->OnAppendText += [&](dms::message msg) {
        text.setString(dms::utils::concat(text.getString().toAnsiString(), msg.text));
    };

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(1004, 100));
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(1);
    rectangle.setPosition(10, 768-110);


    //// run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            /*else if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }*/
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        if (!state->next(mem)) {
            // We should clean up some stuff here!
            // We should exit and show an error if one exists
            //std::cout << state->err.err_msg;
            break;
        }

        // Update the runner
        run.update();

        // draw everything here...

        window.draw(sprite);
        window.draw(rectangle);
        window.draw(text);

        // end the current frame
        window.display();
    }
}

/*
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include "pch.h"
#include <iostream>



struct Box
{
    int x1;
    int y1;
    int x2;
    int y2;
};

int win_get_num_video_adapters(void)
{
    DISPLAY_DEVICE dd;
    int count = 0;
    int c = 0;

    memset(&dd, 0, sizeof(dd));
    dd.cb = sizeof(dd);

    while (EnumDisplayDevices(NULL, count, &dd, 0) != false) {
        if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
            c++;
        count++;
    }

    return c;
}

bool win_get_monitor_info(int adapter, Box* info)
{
    DISPLAY_DEVICE dd;
    DEVMODE dm;

    memset(&dd, 0, sizeof(dd));
    dd.cb = sizeof(dd);
    if (!EnumDisplayDevices(NULL, adapter, &dd, 0)) {
        return false;
    }

    memset(&dm, 0, sizeof(dm));
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
        return false;
    }

    //   ASSERT(dm.dmFields & DM_PELSHEIGHT);
    //   ASSERT(dm.dmFields & DM_PELSWIDTH);
       // Disabled this assertion for now as it fails under Wine 1.2. //
       // ASSERT(dm.dmFields & DM_POSITION); //

info->x1 = dm.dmPosition.x;
info->y1 = dm.dmPosition.y;
info->x2 = info->x1 + dm.dmPelsWidth;
info->y2 = info->y1 + dm.dmPelsHeight;
return true;
}

struct Rect
{
    int _x;
    int _y;
    int _w;
    int _h;
};

BOOL CALLBACK MyInfoEnumProc(
    HMONITOR hMonitor,
    HDC hdcMonitor,
    LPRECT lprcMonitor,
    LPARAM dwData
)
{
    MONITORINFOEX mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);

    GetMonitorInfo(hMonitor, &mi);
    wprintf(L"DisplayDevice: %s\n", mi.szDevice);

    return TRUE;
}

sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {

    // Compares the aspect ratio of the window to the aspect ratio of the view,
    // and sets the view's viewport accordingly in order to archieve a letterbox effect.
    // A new view (with a new viewport set) is returned.

    float windowRatio = windowWidth / (float)windowHeight;
    float viewRatio = view.getSize().x / (float)view.getSize().y;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    // If horizontalSpacing is true, the black bars will appear on the left and right side.
    // Otherwise, the black bars will appear on the top and bottom.

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

    return view;
}

void getRectMonitor(HWND hwnd, Rect& rect)
{
    HMONITOR hMonitor;
    MONITORINFO mInfo;

    hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    mInfo.cbSize = sizeof(mInfo);

    GetMonitorInfo(hMonitor, &mInfo);

    rect._x = mInfo.rcMonitor.left;
    rect._y = mInfo.rcMonitor.top;
    rect._w = mInfo.rcMonitor.right - mInfo.rcMonitor.left;
    rect._h = mInfo.rcMonitor.bottom - mInfo.rcMonitor.top;

}

void showMonitorInfo(HWND hwnd)
{
    Rect rectMonitor;
    getRectMonitor(hwnd, rectMonitor);
    std::cout << "--- MONITOR INFOS ---\n";
    std::cout << " Position : " << rectMonitor._x << " , " << rectMonitor._y << " \n";
    std::cout << " Size     : " << rectMonitor._w << " , " << rectMonitor._h << " \n";
    std::cout << "----------------------\n";

}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    int* Count = (int*)dwData;
    (*Count)++;
    return TRUE;
}

int MonitorCount()
{
    int Count = 0;
    if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count))
        return Count;
    return -1;//signals an error
}


int main()
{
    unsigned screenW = 960;
    unsigned screenH = 540;

    sf::RenderWindow window;
    window.create(sf::VideoMode(screenW, screenH), "My window", sf::Style::Default);
    HWND hwnd = window.getSystemHandle();

    sf::Event event;

    sf::Texture texture;
    texture.loadFromFile("bg.png");
    sf::Sprite sprite(texture);

    sf::View view;
    view.setSize(screenW, screenH);
    view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
    view = getLetterboxView(view, screenW, screenH);

    sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color::Green);

    sf::RectangleShape rectView(sf::Vector2f(screenW, screenH));
    rectView.setFillColor(sf::Color(40, 60, 80));

    bool isFullScreen = false;
    bool keyFull = false;
    bool keyTab = false;
    bool keyShow = false;

    DISPLAY_DEVICE dd;
    dd.cb = sizeof(dd);
    int deviceIndex = 0;
    int monitorIndex = 0;

    while (EnumDisplayDevices(0, deviceIndex, &dd, 0))
    {
        while (EnumDisplayDevices(dd.DeviceName, monitorIndex, &dd, 0))
        {
            std::cout << dd.DeviceName << ", " << dd.DeviceString << "\n";

            ++monitorIndex;
        }
        ++deviceIndex;
    }


    std::cout << "nb Monitor = " << MonitorCount() << "\n";

    float x = 10;
    window.setFramerateLimit(60);


    sf::Vector2i pos = sf::Vector2i(screenW / 2, screenH / 2);


    showMonitorInfo(hwnd);

    sf::Vector2i savePos;
    savePos.x = 0;
    savePos.y = 0;

    int currentMonitor = 0;

    while (window.isOpen())
    {
        if (!isFullScreen)
        {
            sf::Vector2i winPos = window.getPosition();
            savePos.x = winPos.x;
            savePos.y = winPos.y;
        }


        // on inspecte tous les évènements de la fenêtre qui ont été émis depuis la précédente itération
        while (window.pollEvent(event))
        {
            // évènement "fermeture demandée" : on ferme la fenêtre
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
                view = getLetterboxView(view, event.size.width, event.size.height);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) keyFull = false;


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !keyFull)
        {
            keyFull = true;

            isFullScreen = !isFullScreen;

            if (isFullScreen)
            {
                std::cout << "FULLSCREEN \n";

                Rect rectM;
                getRectMonitor(hwnd, rectM);

                //showMonitorInfo(hwnd);

                std::cout << "rectM = " << rectM._x << " , " << rectM._y;
                std::cout << " | " << rectM._w << " , " << rectM._h << "\n";

                window.create(sf::VideoMode(rectM._w, rectM._h), "My window", sf::Style::None);
                hwnd = window.getSystemHandle();

                window.setPosition(sf::Vector2i(rectM._x, rectM._y));
                window.setSize(sf::Vector2u(rectM._w, rectM._h));

                SetWindowPos(hwnd, HWND_TOPMOST,
                    0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
                    SWP_NOMOVE | SWP_NOSIZE);

                //window.setFramerateLimit(60);
                window.setVerticalSyncEnabled(true);
                view = getLetterboxView(view, rectM._w, rectM._h);
                window.setView(sf::View(sf::FloatRect(0, 0, rectM._w, rectM._h)));

            }
            else
            {
                std::cout << "WINDOWED \n";

                window.create(sf::VideoMode(screenW, screenH), "My window", sf::Style::Default);
                hwnd = window.getSystemHandle();
                window.setPosition(savePos);

                window.setFramerateLimit(60);
                view = getLetterboxView(view, screenW, screenH);
            }


        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) keyTab = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && !keyTab)
        {
            keyTab = true;

            std::cout << "Switch Monitor !\n";

            ++currentMonitor;

            if (currentMonitor > win_get_num_video_adapters() - 1)
                currentMonitor = 0;

            std::cout << " Current Monitor = " << currentMonitor << "\n";

        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S)) keyShow = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !keyShow)
        {
            keyShow = true;
            showMonitorInfo(hwnd);
        }

        float v = 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    pos.y += -v;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  pos.y += v;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  pos.x += -v;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) pos.x += v;


        x += 1;

        if (x > screenW)
            x = 0;

        //pos = sf::Mouse::getPosition(window);



        shape.setPosition(pos.x, pos.y);

        sprite.setPosition(x, 0);

        window.setView(view);

        window.clear();

        window.draw(rectView);
        window.draw(sprite);
        window.draw(shape);

        window.display();

    }

    return 0;
}
*/