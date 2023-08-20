#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cctype>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "board.h"
using namespace std;

void welcomeScreen(sf::Vector2i location);
void GameScreen(sf::Vector2i location, string name);
int UpdateLeaderboard(string name, int gameMins, int gameSecs, ifstream& input, ofstream& output);
void Leaderboard(sf::Vector2i location, int cols, int rows, int newStat);

int main() {

    // create a vector to center the window
    sf::VideoMode vm = sf::VideoMode::getDesktopMode();
    auto w = (vm.width  / 2) - 400;
    auto h = (vm.height / 2) - 300;
    sf::Vector2i center(w, h);

    welcomeScreen(center);

    return 0;
}



void welcomeScreen(sf::Vector2i location) {

    // use fstream to read game dimensions from config.cfg
    ifstream in("files/board_config.cfg");
    int cols, rows, mines;
    in >> cols >> rows >> mines;
    in.close();
    int width = cols * 32;
    int height = (rows * 32) + 100;


    // create a fixed-size window titled "Minesweeper"
    sf::RenderWindow window;
    window.create(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
    window.setVisible(false);
    window.setPosition(location);
    window.setVisible(true);
    window.clear(sf::Color::Blue);
    window.display();
    window.setVerticalSyncEnabled(true);


    // load the font
    sf::Font font;
    font.loadFromFile("files/font.ttf");


    // create text
    sf::Text title;
    sf::Text prompt;
    sf::Text input;
    title.setFont(font);
    prompt.setFont(font);
    input.setFont(font);

    
    // title text
    title.setString("WELCOME TO MINESWEEPER!");
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(24);
    sf::FloatRect r1 = title.getLocalBounds();
    title.setOrigin(r1.left + (r1.width / 2.0f), r1.top + (r1.height / 2.0f));
    title.setPosition(sf::Vector2f(width / 2.0f, (height / 2.0f) - 150));


    // prompt text
    prompt.setString("Enter your name:");
    prompt.setStyle(sf::Text::Bold);
    prompt.setCharacterSize(20);
    sf::FloatRect r2 = prompt.getLocalBounds();
    prompt.setOrigin(r2.left + (r2.width / 2.0f), r2.top + (r2.height / 2.0f));
    prompt.setPosition(sf::Vector2f(width / 2.0f, (height / 2.0f) - 75));


    // name text
    string name = "|";
    input.setString(name);
    input.setCharacterSize(18);
    input.setStyle(sf::Text::Bold);
    input.setFillColor(sf::Color::Yellow);
    sf::FloatRect r3 = input.getLocalBounds();
    input.setOrigin(r3.left + (r3.width / 2.0f), r3.top + (r3.height / 2.0f));
    input.setPosition(width / 2.0f, (height / 2.0f) - 45);


    /* ==================== GAME LOOP ==================== */
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
            else if (event.type == sf::Event::TextEntered) {
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
                    input.setString(name);
                    // re-center text
                    sf::FloatRect r3 = input.getLocalBounds();
                    input.setOrigin(r3.left + (r3.width / 2.0f), r3.top + (r3.height / 2.0f));
                    input.setPosition(width / 2.0f, (height / 2.0f) - 45);
                }
            }


            // user presses a non-alpha key
            // only works if user has entered a name
            else if (event.type == sf::Event::KeyPressed  && name.size() - 1 > 0) {
               
                // user presses the 'backspace' key
                // (mulitple keypress is enabled to allow press & hold backspacing)
                if (event.key.code == sf::Keyboard::Key::Backspace) {
                    name.pop_back();
                    name.pop_back();
                    name += "|";
                    input.setString(name);;
                    // re-center text
                    sf::FloatRect r3 = input.getLocalBounds();
                    input.setOrigin(r3.left + (r3.width / 2.0f), r3.top + (r3.height / 2.0f));
                    input.setPosition(width / 2.0f, (height / 2.0f) - 45);
                }                

                // user presses the 'enter' key
                else if (event.key.code == sf::Keyboard::Key::Enter) {
                    name.pop_back();
                    window.close();
                    GameScreen(location, name);
                }
            }
        }


    // update the window with any changes
    window.clear(sf::Color::Blue);
    window.draw(title);
    window.draw(prompt); 
    window.draw(input);
    window.display();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void GameScreen(sf::Vector2i location, string name) {

    // use fstream to read game dimensions from config.cfg
    fstream file("files/board_config.cfg");
    int cols, rows, mines;
    file >> cols >> rows >> mines;
    file.close();

    
    // create a fixed-size window titled "Minesweeper"
    sf::RenderWindow window(sf::VideoMode(cols * 32, (rows * 32) + 100), "Minesweeper", sf::Style::Close);
    window.setVisible(false);
    window.setPosition(location);
    window.setVisible(true);
    window.setVerticalSyncEnabled(true);
    window.clear(sf::Color::White);


    // load game textures
    sf::Texture hiddenTxt;
    hiddenTxt.loadFromFile("files/images/tile_hidden.png");
    sf::Texture revealedTxt;
    revealedTxt.loadFromFile("files/images/tile_revealed.png");
    sf::Texture flagTxt;
    flagTxt.loadFromFile("files/images/flag.png");
    sf::Texture mineTxt;
    mineTxt.loadFromFile("files/images/mine.png");
    sf::Texture happyTxt;
    happyTxt.loadFromFile("files/images/face_happy.png");
    sf::Texture loseTxt;
    loseTxt.loadFromFile("files/images/face_lose.png");
    sf::Texture winTxt;
    winTxt.loadFromFile("files/images/face_win.png");
    sf::Texture debugTxt;
    debugTxt.loadFromFile("files/images/debug.png");
    sf::Texture pauseTxt;
    pauseTxt.loadFromFile("files/images/pause.png");
    sf::Texture playTxt;
    playTxt.loadFromFile("files/images/play.png");
    sf::Texture leaderboardTxt;
    leaderboardTxt.loadFromFile("files/images/leaderboard.png");

    // load tile textures into array
    sf::Texture tileNums[8];

    sf::Texture tileOneTxt;
    tileOneTxt.loadFromFile("files/images/number_1.png");
    tileNums[0] = tileOneTxt;

    sf::Texture tileTwoTxt;
    tileTwoTxt.loadFromFile("files/images/number_2.png");
    tileNums[1] = tileTwoTxt;

    sf::Texture tileThreeTxt;
    tileThreeTxt.loadFromFile("files/images/number_3.png");
    tileNums[2] = tileThreeTxt;

    sf::Texture tileFourTxt;
    tileFourTxt.loadFromFile("files/images/number_4.png");
    tileNums[3] = tileFourTxt;

    sf::Texture tileFiveTxt;
    tileFiveTxt.loadFromFile("files/images/number_5.png");
    tileNums[4] = tileFiveTxt;

    sf::Texture tileSixTxt;
    tileSixTxt.loadFromFile("files/images/number_6.png");
    tileNums[5] = tileSixTxt;

    sf::Texture tileSevenTxt;
    tileSevenTxt.loadFromFile("files/images/number_7.png");
    tileNums[6] = tileSevenTxt;

    sf::Texture tileEightTxt;
    tileEightTxt.loadFromFile("files/images/number_8.png");
    tileNums[7] = tileEightTxt;


    // create digit sprites
    sf::Sprite digitList[10];

    sf::Texture digitsTxt;
    digitsTxt.loadFromFile("files/images/digits.png");
    sf::Sprite digits;
    digits.setTexture(digitsTxt);

    sf::IntRect r0(0, 0, 21, 32);
    digits.setTextureRect(r0);
    sf::Sprite digit0 = digits;
    digitList[0] = digit0;

    sf::IntRect r1(21, 0, 21, 32);
    digits.setTextureRect(r1);
    sf::Sprite digit1 = digits;
    digitList[1] = digit1;

    sf::IntRect r2(21 * 2, 0, 21, 32);
    digits.setTextureRect(r2);
    sf::Sprite digit2 = digits;
    digitList[2] = digit2;

    sf::IntRect r3(21 * 3, 0, 21, 32);
    digits.setTextureRect(r3);
    sf::Sprite digit3 = digits;
    digitList[3] = digit3;

    sf::IntRect r4(21 * 4, 0, 21, 32);
    digits.setTextureRect(r4);
    sf::Sprite digit4 = digits;
    digitList[4] = digit4;

    sf::IntRect r5(21 * 5, 0, 21, 32);
    digits.setTextureRect(r5);
    sf::Sprite digit5 = digits;
    digitList[5] = digit5;

    sf::IntRect r6(21 * 6, 0, 21, 32);
    digits.setTextureRect(r6);
    sf::Sprite digit6 = digits;
    digitList[6] = digit6;

    sf::IntRect r7(21 * 7, 0, 21, 32);
    digits.setTextureRect(r7);
    sf::Sprite digit7 = digits;
    digitList[7] = digit7;

    sf::IntRect r8(21 * 8, 0, 21, 32);
    digits.setTextureRect(r8);
    sf::Sprite digit8 = digits;
    digitList[8] = digit8;

    sf::IntRect r9(21 * 9, 0, 21, 32);
    digits.setTextureRect(r9);
    sf::Sprite digit9 = digits;
    digitList[9] = digit9;

    sf::IntRect n(21 * 10, 0, 21, 32);
    digits.setTextureRect(n);
    sf::Sprite negative = digits;
    negative.setPosition(12, 32 * (rows + 0.5f) +16);

    
    // create the board
    Board board(cols, rows, mines);


    // create a sprite for the tiles
    sf::Sprite ts;
    ts.setColor(sf::Color::White);
    ts.setTexture(hiddenTxt);


    // draw the tiles
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            window.draw(ts);
            ts.move(sf::Vector2f(32.f, 0));
        }
        ts.move(sf::Vector2f(- (32 * cols), 32));
    }


    // create sprites for the buttons
    sf::Sprite newGame;
    sf::Sprite toggleDebug;
    sf::Sprite togglePlay;
    sf::Sprite leaderboard;


    // texture the buttons
    newGame.setTexture(happyTxt);
    newGame.setPosition(((cols / 2.0) * 32) - 32, (rows + 0.5f) * 32);
    toggleDebug.setTexture(debugTxt);
    toggleDebug.setPosition((cols * 32) - 304, (rows + 0.5f) * 32);
    togglePlay.setTexture(pauseTxt);
    togglePlay.setPosition((cols * 32) - 240, (rows + 0.5f) * 32);
    leaderboard.setTexture(leaderboardTxt);
    leaderboard.setPosition((cols * 32) - 176, (rows + 0.5f) * 32);


    // set the game state
    enum gameState { play, pause, debug, win, lose };
    gameState state = play;
    gameState prev;
    bool score = false;

    // number of num tiles left
    // player wins when count = 0
    int count = (rows * cols) - mines;


    // timer
    auto startTime = chrono::high_resolution_clock::now();
    // ignore these for now
    auto pauseTime = chrono::high_resolution_clock::now();
    auto totalPauseTime = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - pauseTime).count(); // int
    int totalTime = 0;

    // record additions to leaderboard
    int newStat = -1;
    int iteration = -1;


    /* ==================== GAME LOOP ==================== */
    while (window.isOpen()) {

        // event handling - game
        sf::Event event;
        while (window.pollEvent(event)) {


            // user attempts to close the window
            if (event.type == sf::Event::Closed) {
                window.close();
            }


            // user clicks
            if (event.type == sf::Event::MouseButtonPressed) {

                // get position of click
                int x = event.mouseButton.x - (event.mouseButton.x % 32);
                int y = event.mouseButton.y - (event.mouseButton.y % 32);


                // check that mouse click is within board bounds                
                if (y < (rows * 32) && (state == play || state == debug)) {


                    // determine position of clicked tile
                    int c = floor(event.mouseButton.x / 32.0);
                    int r = floor(event.mouseButton.y / 32.0);
                    Tile& t = board[c + (r * cols)];

                    // left click - reveal tile
                    if ((event.mouseButton.button == sf::Mouse::Left) && (!t.IsFlagged()) && (!t.IsRevealed())) {
                        
                        // reveal all mines together
                        if (t.IsMine()) {

                            for (int i = 0; i < rows * cols; i++) {
                                
                                if (board[i].IsMine()) {
                                    board[i].Reveal(count);
                                }
                            }

                            state = lose;
                            newGame.setTexture(loseTxt);
                        }

                        // reveal numbers
                        else {

                            t.Reveal(count);
                            
                            if (count == 0) {

                                state = win;

                                // flag all mines
                                for (int i = 0; i < rows * cols; i++) {
                                    
                                    if (board[i].IsMine()) {
                                        board[i].Flag();
                                        // board.SetFlagCount(mines);
                                    }
                                }

                                newGame.setTexture(winTxt);
                            }
                        }
                    }


                    // right click - toggle flag
                    else if ((event.mouseButton.button == sf::Mouse::Right) && (!t.IsRevealed())) {

                        bool add = t.Flag();
                        if (add) {
                            board.SetFlagCount(board.GetFlagCount() + 1);
                        }
                        else {
                            board.SetFlagCount(board.GetFlagCount() - 1);
                        }
                    }
                }


                // mouse click is outside board bounds
                else {


                    // smiley button - new game
                    if ((newGame.getPosition().x <= x && x <= newGame.getPosition().x + 64) && (newGame.getPosition().y <= y && y <= newGame.getPosition().y + 64)) {
                        
                        state = play;
                        iteration = -1;
                        newGame.setTexture(happyTxt);
                        togglePlay.setTexture(pauseTxt);
                        board.Replace();
                        count = (rows * cols) - mines;
                        startTime = chrono::high_resolution_clock::now();
                        // ignore these for now
                        pauseTime = chrono::high_resolution_clock::now();
                        totalPauseTime = chrono::duration_cast<chrono::seconds>(startTime - pauseTime).count();      
                    }


                    // debug button - toggle debug mode
                    if ((toggleDebug.getPosition().x <= x && x <= toggleDebug.getPosition().x + 64) && (toggleDebug.getPosition().y <= y && y <= toggleDebug.getPosition().y + 64) && (state == play || state == debug)) {
                        
                        if (state != debug) {
                            state = debug;
                        }
                        else {
                            state = play;
                        }
                    }


                    // pause/play button - toggle pause mode
                    if ((togglePlay.getPosition().x <= x && x <= togglePlay.getPosition().x + 64) && (togglePlay.getPosition().y <= y && y <= togglePlay.getPosition().y + 64)&& (state < 3)) {
                        
                        if (state != pause) {
                            state = pause;
                            togglePlay.setTexture(playTxt);
                            pauseTime = chrono::high_resolution_clock::now();
                        }
                        else {
                            state = play;
                            togglePlay.setTexture(pauseTxt);
                            auto playTime = chrono::high_resolution_clock::now();
                            totalPauseTime += chrono::duration_cast<chrono::seconds>(playTime - pauseTime).count(); // add paused time
                        }
                    }


                    // leaderboard button - open leaderboard
                    if ((leaderboard.getPosition().x <= x && x <= leaderboard.getPosition().x + 64) && (leaderboard.getPosition().y <= y && y <= leaderboard.getPosition().y + 64)) {
                        
                        pauseTime = chrono::high_resolution_clock::now();

                        for (int i = 0; i < rows * cols; i++) {

                            int c = (i % cols) * 32;
                            int r = ((i - (i % cols)) / cols) * 32;
                            ts.setPosition(c, r);
                            ts.setTexture(revealedTxt);
                            window.draw(ts);
                        }

                        window.display();
                        Leaderboard(location, cols, rows, -1);
                        auto playTime = chrono::high_resolution_clock::now();
                        totalPauseTime += chrono::duration_cast<chrono::seconds>(playTime - pauseTime).count();                 
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        //draw timer
        // TOTAL duration of game (including paused periods)
        auto gameDuration = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - startTime);
        // convert to int
        totalTime = gameDuration.count();

        int minutes;
        int seconds;

        if (state == play || state == debug) {
            totalTime = totalTime - totalPauseTime;
            minutes = totalTime / 60;
            seconds = totalTime % 60;
        }

        int minutesL = minutes / 10 % 10;
        int minutesR = minutes % 10;
        int secondsL = seconds / 10 % 10;
        int secondsR = seconds % 10;

        // cout << minutesL << minutesR << " : " << secondsL << secondsR << endl;
        digitList[minutesL].setPosition((cols * 32) - 97, 32 * (rows + 0.5f) + 16);
        window.draw(digitList[minutesL]);
        digitList[minutesR].setPosition((cols * 32) - 97 + 21, 32 * (rows + 0.5f) + 16);
        window.draw(digitList[minutesR]);
        digitList[secondsL].setPosition((cols * 32) - 54, 32 * (rows + 0.5f) + 16);
        window.draw(digitList[secondsL]);
        digitList[secondsR].setPosition((cols * 32) - 54 + 21, 32 * (rows + 0.5f) + 16);
        window.draw(digitList[secondsR]);


        // draw mine counter
        int mineCount = board.GetRemainingMines();
        
        if (mineCount < 0) {
            window.draw(negative);
            mineCount *= -1;
        }

        int size = to_string(mineCount).size();
        
        for (int i = size - 1; i >= 0; i--) {

            int num = mineCount % 10;
            digitList[num].setPosition(33 + (i * 21), 32 * (rows + 0.5f) + 16);
            window.draw(digitList[num]);
            mineCount /= 10;
        }


        // draw the updated board
        for (int i = 0; i < rows * cols; i++) {
            int c = (i % cols) * 32;
            int r = ((i - (i % cols)) / cols) * 32;
            ts.setPosition(c, r);

            Tile t = board[i];


            // if game is paused, all tiles are revealed
            if (state == pause) {

                ts.setTexture(revealedTxt);
                window.draw(ts);
            }


            // for all other game states
            else {
                
                // mines
                if (t.IsMine()) {

                    // debug mode
                    if (state == debug) {

                        // flagged mines stay flagged
                        // cout <<
                        if (t.IsFlagged()) {
                            ts.setTexture(hiddenTxt);
                            window.draw(ts);
                            ts.setTexture(flagTxt);
                            window.draw(ts);
                        }

                        // unflagged mine becomes revealed
                        else {
                            ts.setTexture(revealedTxt);
                            window.draw(ts);
                            ts.setTexture(mineTxt);
                            window.draw(ts);
                        }
                    }

                    // not debug mode
                    else {
                        
                        // revealed mines become revealed
                        if (t.IsRevealed()) {

                            ts.setTexture(revealedTxt);
                            window.draw(ts);
                            ts.setTexture(mineTxt);
                            window.draw(ts);
                        }

                        // hidden mines stay hidden
                        else {

                            ts.setTexture(hiddenTxt);
                            window.draw(ts);

                            // add flag if tile is flagged

                            if (t.IsFlagged()) {
                                ts.setTexture(flagTxt);
                                window.draw(ts);
                            }
                        }
                    }
                }


                // number tiles
                else {

                    // revealed tiles
                    if (t.IsRevealed()) {

                        ts.setTexture(revealedTxt);

                        if (t.GetValue() == 1) {
                            window.draw(ts);
                            ts.setTexture(tileOneTxt);
                        }
                        else if (t.GetValue() == 2) {
                            window.draw(ts);
                            ts.setTexture(tileTwoTxt);
                        }
                        else if (t.GetValue() == 3) {
                            window.draw(ts);
                            ts.setTexture(tileThreeTxt);
                        }
                        else if (t.GetValue() == 4) {
                            window.draw(ts);
                            ts.setTexture(tileFourTxt);
                        }
                        else if (t.GetValue() == 5) {
                            window.draw(ts);
                            ts.setTexture(tileFiveTxt);
                        }
                        else if (t.GetValue() == 6) {
                            window.draw(ts);
                            ts.setTexture(tileSixTxt);
                        }
                        else if (t.GetValue() == 7) {
                            window.draw(ts);
                            ts.setTexture(tileSevenTxt);
                        }
                        else if (t.GetValue() == 8) {
                            window.draw(ts);
                            ts.setTexture(tileEightTxt);
                        }
                        window.draw(ts);
                    }

                    // tile is still hidden
                    else {
                        ts.setTexture(hiddenTxt);
                        window.draw(ts);

                        // add flag if tile is flagged
                        if (t.IsFlagged()) {
                            ts.setTexture(flagTxt);
                            window.draw(ts);
                        }
                    }
                }
            }
        }

        if (state == win && iteration == 0) {

            board.SetFlagCount(mines);

            int gameMins = totalTime / 60;
            int gameSecs = totalTime % 60;

            ifstream in;
            ofstream out;

            newStat = UpdateLeaderboard(name, gameMins, gameSecs, in, out);
            Leaderboard(location, cols, rows, newStat);

            iteration++;
        }
        else if (state == win) {
            iteration++;
        }


        // draw buttons
        window.draw(newGame);
        window.draw(toggleDebug);
        window.draw(togglePlay);
        window.draw(leaderboard);

        window.display();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int UpdateLeaderboard(string player, int mins, int secs, ifstream& input, ofstream& output) {

    input.open("files/leaderboard.txt");
    output.open("files/temp.txt");

    int scoreAdded = -1;
    bool added = false;

    for (int i = 0; i < 5; i++) {

        // read line
        string line;
        getline(input, line);

        // find delimeters
        unsigned int colon = line.find(':');
        unsigned int comma = line.find(',');
        
        // parse string
        string otherMins = line.substr(0, colon);
        int oMins = stoi((otherMins));
        string otherSecs = line.substr(colon + 1, comma - colon - 1);
        int oSecs = stoi((otherSecs));
        string otherName = line.substr(comma + 1, line.size() - comma);

        if ((mins < oMins) || (mins == oMins && secs < oSecs)) {
            
            output << setw(2) << setfill('0') << mins << ":" << setw(2) << setfill('0') << secs << "," << player << endl;

            // store index of new player
            if (!added) {
                scoreAdded = i;
                added = true;
            }

            player = otherName;
            mins = oMins;
            secs = oSecs;
        }
        else {
            output << line << endl;
        }
    }

    input.close();
    output.close();

    rename("files/temp.txt", "files/leaderboard.txt");

    return scoreAdded;
}



void Leaderboard(sf::Vector2i location, int cols, int rows, int newStat) {

    int width = cols * 16;
    int height = (rows * 16) + 50;

    // create a fixed-size window titled "Minesweeper"
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
    window.setVisible(false);
    sf::Vector2i shift(cols * 8, rows * 8 + 25);
    window.setPosition(location + shift);
    window.setVisible(true);
    window.setVerticalSyncEnabled(true);
    window.clear(sf::Color::Blue);


    // read scores from file
    ifstream file("files/leaderboard.txt");
    string scoreList = "";

    for (int i = 0; i < 5; i++) {
        string line;
        file >> line;
        unsigned int index = line.find(',');
        scoreList += to_string(i + 1) + ".";
        scoreList += "\t";
        scoreList += line.substr(0, index);
        scoreList += "\t";
        scoreList += line.substr(index + 1, line.size() - index);

        if (i == newStat) {
            scoreList += "*";
        }

        scoreList += "\n\n";
    }

    file.close();


    // load font
    sf::Font font;
    font.loadFromFile("files/font.ttf");

    // create text
    sf::Text title;
    title.setFont(font);
    title.setString("LEADERBOARD");
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(20);
    sf::FloatRect r1 = title.getLocalBounds();
    title.setOrigin(r1.left + (r1.width / 2.0f), r1.top + (r1.height / 2.0f));
    title.setPosition(sf::Vector2f(width / 2.0f, (height / 2.0f) - 120));

    sf::Text scores;
    scores.setFont(font);
    scores.setString(scoreList);
    scores.setStyle(sf::Text::Bold);
    scores.setFillColor(sf::Color::White);
    scores.setCharacterSize(18);
    sf::FloatRect r2 = scores.getLocalBounds();
    scores.setOrigin(r2.left + (r2.width / 2.0f), r2.top + (r2.height / 2.0f));
    scores.setPosition(sf::Vector2f(width / 2.0f, (height / 2.0f) + 20));

    
    /* ==================== GAME LOOP ==================== */
    while (window.isOpen()) {

        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {

            // user closes window
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        window.draw(title);
        window.draw(scores);

        window.display();
    }
}