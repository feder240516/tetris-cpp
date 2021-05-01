#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<iostream>
#include<cstring>
#include<sstream>
#include"main.h"


/*###############################################################
BLOCK METHODS
###############################################################*/

bool Block::move(int nx, int ny){
        block_sprite.move(sf::Vector2f(nx * BLOCK_MOVE,ny * BLOCK_MOVE));
        x+=nx;
        y+=ny;
        return true;
}

bool Block::canmove(int nx, int ny, Board *board){
    //std::cout << board << std::endl;
    return x+nx >= 0 && x+nx < BOARD_WIDTH && y+ny >= 0 && y+ny < BOARD_HEIGHT && board->getBlockAt(x+nx,y+ny)==nullptr;
}

void Block::drawTo (sf::RenderWindow &window){
    //std::cout<<&window << " ventana "<<std::endl;
    window.draw(block_sprite);
}

bool Block::setTexture(std::string filepath){
    if(!Block::block_texture.loadFromFile(filepath)) throw "Could not find " + filepath + ". Shutting down";
    return true;
}

sf::Texture Block::block_texture;
int Block::block_count;

/*###############################################################
SHAPE METHODS
###############################################################*/

void Shape::drawTo (sf::RenderWindow &window){
    for(int i = 0;i<4;i++){
        blocks[i]->drawTo(window);
    }
}

bool Shape::move(int nx,int ny){
    if (!canmove(nx,ny)) return false;
    for(int i = 0;i<4;i++){
        blocks[i]->move(nx,ny);
    }
    return true;
}

void Shape::forceMove(int nx, int ny){
    for(int i = 0;i<4;i++){
        blocks[i]->move(nx,ny);
    }
}

bool Shape::canmove(int nx, int ny){
    for(int i = 0;i<4;i++){
        if (!blocks[i]->canmove(nx,ny,board)){
            return false;
        }
    }
    return true;
}

void Shape::setBoard(Board *nboard){
    board = nboard;
}

bool Shape::rotate(){
    if (rotate_mode == RotateModes::NOROTATE) return true;
    int nx[4];
    int ny[4];
    bool can_move = false;
    for(int j = 0; j<4 && !can_move; j++){
        for (int i = 0; i<4;i++){ //Rotacion normal
            can_move = true;
            nx[i] = blocks[j]->getX() - blocks[i]->getX() -(-blocks[i]->getY() + blocks[j]->getY());
            ny[i] = blocks[j]->getY() - blocks[i]->getY() -( blocks[i]->getX() - blocks[j]->getX());
            if (!blocks[i]->canmove(nx[i],ny[i],board)){
                can_move = false;
                break;
            }
        }
    }
    if(!can_move) return false;
    for (int i = 0; i<4;i++){
        blocks[i]->move(nx[i],ny[i]);
    }
    return true;
}

void Shape::deleteBlocks(){
    for(int i = 0;i < SHAPE_SIZE;i++){
        delete blocks[i];
    }
}

Shape* Shape::createShape(){
    int select_shape = rand() % NUM_SHAPES;
    switch(select_shape){
    case Shape::ShapeTypes::ISHAPE:
        return new IShape(-5);
        break;
    case Shape::ShapeTypes::JSHAPE:
        return new JShape(-5);
        break;
    case Shape::ShapeTypes::LSHAPE:
        return new LShape(-5);
        break;
    case Shape::ShapeTypes::OSHAPE:
        return new OShape(-5);
        break;
    case Shape::ShapeTypes::SSHAPE:
        return new SShape(-5);
        break;
    case Shape::ShapeTypes::TSHAPE:
        return new TShape(-5);
        break;
    case Shape::ShapeTypes::ZSHAPE:
        return new ZShape(-5);
        break;
    default:
        throw "No shape selected";
        break;
    }
}

int Shape::shape_count;

/*###############################################################
BOARD METHODS
###############################################################*/

void Board::drawTo (sf::RenderWindow &window){
    window.draw(frame);
    for(int i = 0;i<BOARD_HEIGHT;i++){
        for(int j = 0;j<BOARD_WIDTH;j++){
            //std::cout << *board[i][j] << std::endl;
            if(board[i][j]!=nullptr){
                board[i][j]->drawTo(window);
            }
        }
    }
    next_shape->drawTo(window);
}

bool Board::moveShape(int nx, int ny){
    if(!shape->canmove(nx,ny)){
        return false;
    } else{
        shape->move(nx,ny);
        return true;
    }
}

void Board::fixShape(){
    if(!shape->canmove(0,0)){
        ptrgame->setGameStatus(Game::GameStatus::MENU);
        shape->deleteBlocks();
        delete shape;
        delete next_shape;
        shape = nullptr;
        clearBoard();
        this->ptrgame->resetGame();
        return;
    }
    for(int i = 0;i<4;i++){
        int posx = shape->blocks[i]->getX();
        int posy = shape->blocks[i]->getY();
        if(board[posy][posx] != nullptr){

        }
        board[posy][posx] = shape->blocks[i];
    }
    checkFullLines();
    getNextShape();
}

Block* Board::getBlockAt(int x, int y){
    return board[y][x];
}

bool Board::rotateShape(){
    return shape->rotate();
}

Shape* Board::createShape(){

    Shape *shape = Shape::createShape();
    shape->setBoard(this);
    return shape;
}

void Board::clearLine(int line){
    for(int i = line; i >= 0; i--){
        for(int j = 0; j < BOARD_WIDTH; j++){
            delete board[i][j];
            if(i>0){
                if (board[i-1][j] != nullptr) board[i-1][j]->move(0,1);
                board[i][j] = board[i-1][j];
            }
            board[i-1][j] = nullptr;
        }
    }
}



void Board::checkFullLines(){
    for(int i = BOARD_HEIGHT-1; i >= 0; i--){
        bool line_complete = true;
        for(int j = 0; j < BOARD_WIDTH; j++){
            if (board[i][j] == nullptr){
                line_complete = false;
                break;
            }
        }
        if (line_complete) {
            clearLine(i);
            i++;
            score++;
        }
    }
}

void Board::clearBoard(){
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            if(board[i][j] != nullptr){
                delete board[i][j];
                board[i][j] = nullptr;
            }
        }
    }
}

void Board::startGame(){
    score = 0;
    setNextShapeObj(new NextShape);
    getNextShape();
}

void Board::execMove(int code_evt){
    int movex = 0;
    int movey = 0;
    switch(code_evt){
    case sf::Keyboard::Right:
        movex = 1;
        movey = 0;
        break;
    case sf::Keyboard::Left:
        movex = -1;
        movey = 0;
        break;
    case sf::Keyboard::Down:
        movex = 0;
        movey = 1;
        break;
    default:
        break;
    }
    switch(code_evt){
    case sf::Keyboard::Right:
    case sf::Keyboard::Left:
    case sf::Keyboard::Down:
        if(!moveShape(movex,movey)){
            if(code_evt == sf::Keyboard::Down){
                fixShape();
            }
        }
        break;
    case sf::Keyboard::Up:
        rotateShape();
        break;
    }

}

void Board::getNextShape(){
    if(shape!=nullptr) delete shape;
    shape = next_shape->leaveShape();
    shape->setBoard(this);
    shape->forceMove(5 + (int)(BOARD_WIDTH/2),0);
}

void Board::setNextShapeObj(NextShape* nnext_shape){
    next_shape = nnext_shape;
}

/*###############################################################
SIMPLE BUTTON
###############################################################*/

void SimpleButton::drawTo(sf::RenderWindow &window){
    if(visible){
        window.draw(frame);
        window.draw(text);
    }
}

bool SimpleButton::checkClick(const sf::Vector2f &mousepos){
    return clickable && area.contains(mousepos.x,mousepos.y);
}

void SimpleButton::onClick(){
    if (clickFunctionptr != nullptr){
        clickFunctionptr(this);
    } else{
        std::cout << "No se ha asignado ninguna funcion a este boton\n";
    }
}

void SimpleButton::makeClickable(bool nclickable){
    clickable = nclickable;
}

void SimpleButton::makeVisible(bool nvisible){
    visible = nvisible;
}

void SimpleButton::enable(bool nenable){
    clickable = nenable;
    visible = nenable;
}

void SimpleButton::centerText(){
    text.setPosition(area.left+(area.width-text.getGlobalBounds().width)/2,area.top+(area.height-2*text.getGlobalBounds().height)/2);
}

sf::Font SimpleButton::default_font;
int SimpleButton::button_counter;

/*###############################################################
NEXTSHAPE METHODS
###############################################################*/

Shape* NextShape::createShape(){
    next_shape = Shape::createShape();
    return next_shape;
}

Shape* NextShape::leaveShape(){
    Shape *giving_shape = next_shape;
    createShape();
    //giving_shape->move(5,0);
    return giving_shape;
}

void NextShape::drawTo(sf::RenderWindow &window){
    next_shape->drawTo(window);
}


/*###############################################################
GAME METHODS
###############################################################*/

void Game::resetGame(){
    start_button->enable(true);

}

void Game::startApp(){
    srand(time(0)); //random init
    int down_count = 0; //frame count events
    this->game_status = GameStatus::MENU;

    Block::setTexture("assets/block.png");
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH,SCREEN_HEIGHT),"Tetris",sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);
    //window.setKeyRepeatEnabled(false);
    sf::Texture texture_background;
    if (!texture_background.loadFromFile("assets/windowsxp.jpg")) throw "Could not find assets/windowsxp.jpg. Shutting down.";
    sf::Sprite sprite_background(texture_background);
    Board board(this);

    sf::Event event;
    if(!SimpleButton::default_font.loadFromFile("assets/Electron Plain.ttf")) throw "Could not find assets/Electron Plain.ttf. Shutting down.";

    //SimpleButton start_button = *ptr_start_button;
    start_button = new SimpleButton(sf::Vector2f(SCREEN_WIDTH/4,SCREEN_HEIGHT/2),sf::Vector2f(SCREEN_WIDTH/2,SCREEN_HEIGHT/4),"START",&board);
    start_button->setCallback([this](SimpleButton* btn){
        this->game_status = Game::GameStatus::PLAY;
        btn->getBoard()->startGame();
        btn->enable(false);
    });
    score_text = new sf::Text("",SimpleButton::default_font);
    //score_text.move(const sf::Vector2f(0,0));
    /*###############################################################
    START MAIN LOOP
    ###############################################################*/

    while(window.isOpen()){
        window.clear();
        window.draw(sprite_background);

        switch(this->game_status){
        case GameStatus::MENU:
            start_button->drawTo(window);
            break;
        case GameStatus::PLAY:{
            if (down_count % board.getDownInterval() == 0){
                board.execMove(sf::Keyboard::Down);
                down_count = 0;
            }
            std::ostringstream texto_a_poner;
            texto_a_poner << "Score: " << board.getScore();
            score_text->setString(texto_a_poner.str());
            window.draw(*score_text);
            board.drawTo(window);
            break;
        }
        case GameStatus::GAMEOVER:
            break;
        }
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && this->game_status == GameStatus::PLAY){
                board.execMove(event.key.code);
                if (event.key.code == sf::Keyboard::Down) down_count = 0;
            }
            if (event.type == sf::Event::MouseButtonPressed){
                for(auto i = board.clickable_elements.begin();i!=board.clickable_elements.end();i++){
                    if((*i)->checkClick(window.mapPixelToCoords(sf::Mouse::getPosition(window)))){
                        (*i)->onClick();
                    }
                    std::cout << "clickable:" << (*i)->checkClick(window.mapPixelToCoords(sf::Mouse::getPosition(window))) << std::endl;
                }
            }
        }
        down_count++;

        /*std::cout << "Block count: " << Block::block_count << std::endl;
        std::cout << "Shape count: " << Shape::shape_count << std::endl;
        std::cout << "Down interv: " << board.getDownInterval() << std::endl;
        std::cout << "Score:       " << board.getScore() << std::endl;
*/

        if(this->game_status == GameStatus::PLAY || this->game_status == GameStatus::GAMEOVER) board.shape->drawTo(window);
        window.display();
        //std::cout << "Block count:" << Block::block_count << std::endl;
    }
    std::cout << SimpleButton::button_counter << std::endl;
    setStartButton(nullptr);
}


void Game::activateButton(bool nactivate){
    start_button->enable(nactivate);
}

/*###############################################################
MAIN
###############################################################*/

int main()
{
    Game game;
    game->startApp();
    return EXIT_SUCCESS;
}
