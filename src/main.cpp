#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace std;

void welcomeScreen();

int main() {

    welcomeScreen();

    return 0;
}

void welcomeScreen() {
    // create a fixed-size window titled "Minesweeper"
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper", sf::Style::Close);
    window.clear(sf::Color::Blue);
    window.display();

    // load the font
    sf::Font font;
    font.loadFromFile("files/font.ttf");

    // create the text
    sf::Text text;
    text.setFont(font);

    // create a string to hold the player's name
    string name = "|";
    text.setString(name);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    window.draw(text);
    window.display();

    // game loop - run while the window is still open
    // allows the window to process internal events
    while (window.isOpen()) {

        // event handling
        sf::Event event;
        // while loop iterates until no more events (.pollEvent() returns False)
        while (window.pollEvent(event)) {

            // user attempts to close the window
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // user enters text
            if (event.type == sf::Event::TextEntered) {
                char character = static_cast<char>(event.text.unicode);

                if (isalpha(character) && name.size() - 1 < 10) {
                    name.pop_back();
                    if (name.size() == 0) {
                        name += toupper(character);
                    }
                    else {
                        name += tolower(character);
                    }
                    name += "|";
                    text.setString(name);
                    window.clear(sf::Color::Blue);
                    window.draw(text);
                    window.display();
                }
            }

            // user presses a non-alpha key
            // only works if user has entered a name
            if (event.type == sf::Event::KeyPressed  && name.size() - 1 > 0) {

                // user presses the 'backspace' key
                // (mulitple keypress is enabled to allow press & hold backspacing)
                if (event.key.code == sf::Keyboard::Key::Backspace) {
                    name.pop_back();
                    name.pop_back();
                    name += "|";
                    text.setString(name);
                    window.clear(sf::Color::Blue);
                    window.draw(text);
                    window.display();
                }                

                // user presses the 'enter' key
                if (event.key.code == sf::Keyboard::Key::Enter) {
                    name.pop_back();
                    cout << name << endl;
                    window.close();
                }
            }
        }
        // update the window with any changes
        // window.display();
    }
}