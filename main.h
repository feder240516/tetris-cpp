#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <time.h>
#include <functional>


const int BLOCK_SIZE = 30;
const int BLOCK_MOVE = 27;
const int BOARD_HEIGHT = 16;
const int BOARD_WIDTH = 12;
const int STARTING_X = 8 * BLOCK_MOVE;
const int STARTING_Y = 40;
const int PADDING_RIGHT = 40;
const int PADDING_DOWN = 40;

const int SCREEN_WIDTH = BOARD_WIDTH * BLOCK_MOVE + STARTING_X + PADDING_RIGHT;
const int SCREEN_HEIGHT = BOARD_HEIGHT * BLOCK_MOVE + STARTING_Y + PADDING_DOWN;
const int SHAPE_SIZE = 4;
enum RotateModes {NOROTATE,ZIGZAG,FULLROTATE};
const int NUM_SHAPES = 7;






//enum COLORES;

//Declarado para poder ser referenciado en las otras clases
class Board;
class SimpleButton;
class NextShape;
class Game;




/**
 * Esta clase se refiere a cada uno de los cubos que se mueven
 */
class Block{
protected:
    static sf::Texture block_texture; // Almacena la imagen predefinida de cada cubo
    int x;
    int y;
    bool enabled;
    sf::Sprite block_sprite;
    const std::string texture_name = "assets/block.png";
public:
    static int block_count;

    /**
     * Constructor Block recibe parametros:
     * Sus posiciones iniciales en x y y con respecto al tablero;
     * Si sera visible o no;
     * Su color.
     */
    Block(int nx=0, int ny=0, bool nenabled = false, const sf::Color &ncolor = sf::Color::White):x(nx),y(ny),enabled(nenabled){
        if(enabled){
            block_sprite.setTexture(Block::block_texture);
            block_sprite.setColor(ncolor);
            block_sprite.move(sf::Vector2f(STARTING_X + nx * BLOCK_MOVE,STARTING_Y + ny * BLOCK_MOVE));
            block_sprite.setScale(((float)BLOCK_SIZE)/Block::block_texture.getSize().x,((float)BLOCK_SIZE)/Block::block_texture.getSize().y);
        }
        block_count++;
        std::cout << "Block created" << std::endl;
    };
    Block(const Block &o):x(o.x),y(o.y),enabled(o.enabled),block_sprite(o.block_sprite){
        block_count++;

    }
    ~Block(){
        block_count--;
        std::cout << "Block destroyed" << std::endl;
    }
    const int getX(){return x;}
    const int getY(){return y;}
    /*sf::Texture* getTexture()
    {
        return &block_texture;
    }*/
    sf::Sprite* getSprite(){return &block_sprite;}

    /**
     * move definido por distancia movida, no por ubicacion nueva.
     * Llamar canmove primero.
     */
    bool move(int nx,int ny);
    bool canmove(int nx, int ny, Board* board);
    void drawTo (sf::RenderWindow &window);
    static bool setTexture(std::string filepath);
};

/**
 * Clase shape para cada Tetromino
 */
class Shape{
protected:
    int x,y;
    int rotate_mode;
    int rotate_state;
public:
    enum ShapeTypes {LSHAPE,JSHAPE,OSHAPE,TSHAPE,ISHAPE,SSHAPE,ZSHAPE};
    static int shape_count;
    Block *blocks[4];
    Board *board;
    /**
     * Parametros de shape:
     * nx y ny: posiciones de sus centros;
     * nx1,ny1,nx2,ny2,nx3,ny3: posiciones de sus otros bloques respecto a sus centros;
     * ncolor: el color de la pieza;
     * nrotate: el modo de rotacion. Solo se estan usando NOROTATE y FULLROTATE.
     * Esta clase es heredada por los diferentes tipos de piezas, y ellas son las que llaman este
     * constructor.
     */
    Shape(int nx = 0,int ny = 1, int nx1 = 1, int ny1 = 0, int nx2 = -1, int ny2 = 0, int nx3 = 1, int ny3 = 1,const sf::Color &ncolor = sf::Color::White, int nrotate_mode = RotateModes::FULLROTATE):
                blocks{new Block(nx,ny,true,ncolor),
                       new Block(nx+nx1,ny+ny1,true,ncolor),
                       new Block(nx+nx2,ny+ny2,true,ncolor),
                       new Block(nx+nx3,ny+ny3,true,ncolor)},
                rotate_mode(nrotate_mode),
                rotate_state(0){shape_count++;std::cout << "Shape created!  Shape count: " << Shape::shape_count << std::endl;}
    ~Shape(){shape_count--;std::cout << "Shape deleted!  Shape count: " << Shape::shape_count << std::endl;}
    void drawTo (sf::RenderWindow &window);
    bool move(int nx,int ny);
    bool canmove(int nx, int ny);
    void setBoard(Board* nboard);
    bool rotate();
    void deleteBlocks();
    void forceMove(int nx, int ny);
    static Shape* createShape();
};

class LShape : public Shape{
public:
    LShape(int nx = 0, int ny = 1):Shape(nx,ny,1,0,-1,0,-1,1,sf::Color(255,128,0),RotateModes::FULLROTATE){}
};

class JShape : public Shape{
public:
    JShape(int nx = 0, int ny = 1):Shape(nx,ny,1,0,-1,0,1,1,sf::Color::Magenta,RotateModes::FULLROTATE){}
};

class OShape : public Shape{
public:
    OShape(int nx = 0, int ny = 1):Shape(nx,ny,1,0,0,1,1,1,sf::Color::White,RotateModes::NOROTATE){}
};

class TShape : public Shape{
public:
    TShape(int nx = 0, int ny = 1):Shape(nx,ny,1,0,-1,0,0,1,sf::Color::Yellow,RotateModes::FULLROTATE){}
};

class IShape : public Shape{
public:
    IShape(int nx = 0, int ny = 1):Shape(nx,ny,1,0,-1,0,2,0,sf::Color::Cyan,RotateModes::ZIGZAG){}
};

class SShape : public Shape{
public:
    SShape(int nx = 0, int ny = 1):Shape(nx,ny,0,1,1,0,-1,1,sf::Color::Green,RotateModes::ZIGZAG){}
};

class ZShape : public Shape{
public:
    ZShape(int nx = 0, int ny = 1):Shape(nx,ny,-1,0,0,1,1,1,sf::Color::Red,RotateModes::ZIGZAG){}
};

/**
 * Clase Board que almacena la informacion necesaria sobre el espacio jugable.
 * Tambien referencia al Shape actualmente en movimiento.
 */
class Board{
private:
    Block *board[BOARD_HEIGHT][BOARD_WIDTH];
    sf::RectangleShape frame;
    int score;
    Game *ptrgame;
public:
    std::vector<SimpleButton*> clickable_elements;
    Shape *shape;
    NextShape *next_shape;
    /**
     * Este constructor toma el Game en el que se construyo como argumento, por si algun atributo
     * de este es necesario posteriormente.
     */
    Board(Game *ngame): score(0),shape(nullptr), next_shape(nullptr),
                        frame(sf::Vector2f(BLOCK_SIZE + (BOARD_WIDTH-1) * BLOCK_MOVE, BLOCK_SIZE + (BOARD_HEIGHT-1) * BLOCK_MOVE)),
                        ptrgame(ngame){
        for(int i = 0;i<BOARD_HEIGHT;i++){
            for(int j = 0;j<BOARD_WIDTH;j++){
                board[i][j] = nullptr;
            }
        }
        frame.move(STARTING_X,STARTING_Y);
        frame.setFillColor(sf::Color(64,64,64,64));
    };
    void drawTo (sf::RenderWindow &window);
    bool moveShape(int nx, int ny);
    bool rotateShape();
    void fixShape();
    void checkFullLines();
    void clearLine(int line);
    void clearBoard();
    int getScore(){return score;}
    int getDownInterval(){return 60/ (int)((0.1 * score)+1);}
    Shape* createShape();
    Block* getBlockAt(int x, int y);
    void startGame();
    void execMove(int code_evt);
    void getNextShape();
    void setNextShapeObj(NextShape *nnext_shape);
};

/**
 * Clase SimpleButton
 * Tiene toda la informacion logica y visual necesaria para mostrar un boton.
 * Para configurar el callback, utilice la funcion setCallback.
 */
class SimpleButton{
protected:
    sf::Rect<float> area;
    sf::RectangleShape frame;
    sf::Text text;
    Board *board;
    //void(*clickFunctionptr)(SimpleButton*);
    std::function <void(SimpleButton*)> clickFunctionptr;

    bool visible;
    bool clickable;

public:

    static int button_counter;
    static sf::Font default_font;

    /**
     * Argumentos del constructor:
     * origin: Esquina superior izquierda;
     * size: Tamaño del boton;
     * words: Texto a renderizar;
     * nboard: Puntero al Board donde se va a renderizar este boton;
     * nvisible, nclickable: autoexplicativos;
     * font: La fuente con la que se renderiza el texto.
     */
    SimpleButton(const sf::Vector2f &origin, const sf::Vector2f &size, std::string words,
                 Board *nboard, bool nvisible = true, bool nclickable = true,
                 sf::Font &font = SimpleButton::default_font,
                 const sf::Color &color = sf::Color(255,128,128,128))
                :area(origin.x,origin.y,size.x,size.y),
                frame(size),
                text(words,font),
                board(nboard),
                clickable(nclickable),
                visible(nvisible),
                clickFunctionptr(nullptr){
        frame.move(origin);
        frame.setFillColor(color);
        frame.setOutlineColor(sf::Color::Black);
        frame.setOutlineThickness(4);
        text.move(area.left,area.top);
        centerText();

        board->clickable_elements.push_back(this);
        button_counter++;
        //std::cout << "\n\n\n\n\n\n\n\n\n\nContador de botones: " << button_counter << std::endl;
    }
    SimpleButton(const SimpleButton &o):area(o.area),frame(o.frame),text(o.text),board(o.board),visible(o.visible),clickable(o.clickable) {button_counter++;std::cout << "cloneContador de botones: " << button_counter << std::endl;}
    ~SimpleButton(){button_counter--;}
    void drawTo(sf::RenderWindow &window);
    bool checkClick(const sf::Vector2f &mousepos);
    void onClick();
    void makeClickable(bool nclickable);
    void makeVisible(bool nvisible);
    void enable(bool nenable);
    bool isClickable(){return clickable;}
    bool isVisible(){return visible;}
    Board* getBoard(){return board;}
    void setCallback(auto func){clickFunctionptr = func;}
    void centerText();
};


/**
 * Clase NextShape guarda toda la informacion sobre la siguiente pieza
 *
 */
class NextShape{
protected:
    Shape *next_shape;
public:
    /**
     * Cada vez que se crea este objeto, inmediatamente crea una Shape a la cual hace referencia,
     * esto por medio del operador new para evitar que quede restringida a este objeto y para lograr
     * que se pueda pasar la Shape al tablero jugable.
     */
    NextShape(): next_shape(nullptr){
        createShape();
    }
    Shape* createShape();
    Shape* leaveShape();
    void drawTo(sf::RenderWindow &window);
};


/**
 * Clase Game que retiene informacion sobre los elementos de la ventana de juego.
 *
 */
class Game{
protected:
    SimpleButton *start_button;
    sf::Text *score_text;
    int game_status;
public:
    enum GameStatus {MENU,PLAY,GAMEOVER};
    Game():start_button(nullptr), score_text(nullptr){};
    void setStartButton(SimpleButton *nstart_button){start_button = nstart_button;}
    void setScoreText(sf::Text *nscore_text){score_text = nscore_text;}
    void activateButton(bool nactivate);
    void resetGame();
    void startApp();
    void setGameStatus(int ngame_status){game_status = ngame_status;}
};

//Game *game;

#endif // MAIN_H_INCLUDED
