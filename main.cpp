/// TODO:
/// + add pause mode
/// + increased height of the screen
/// 3 car-enemies mode?
/// + add music and sound of collision
/// + increase speed with increasing score
/// + make more lives
/// +save/load hi-scores to/from log-file
/// + collision with 2nd car is not ok
/// make trees/shrubs moving along the road?

#include <iostream>
#include <conio.h>
#include <dos.h>
#include <windows.h>
#include <time.h>
#include <mmsystem.h>
#include <fstream>

# define SCREEN_WIDTH 90
# define SCREEN_HEIGHT 45
# define WIN_WIDTH 70

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

int enemyY[3];
int enemyX[3];
int enemyFlag[3];
char car[4][4] = {' ','±','±',' ','±','*','*','±',' ','±','±',' ','±','*','*','±'};
int carPos = WIN_WIDTH/2;
int score = 0;
int speed = 50;
int damage = 3;
bool pause = false;
int hi_score = 0;
std::string champion = "Foobar";
bool flag_hi_score = false;

int logToFile(int hi_score, std::string s){
    //setlocale(LC_ALL, "rus");
    std::string outputFileName {"hi_score.txt"};
    std::ofstream output(outputFileName);
    if (!output.is_open()) std::cout<<"Faled to open WRITE-file: "<< outputFileName << std::endl;
    else{
        output << hi_score << '\t' << s << '\n';
    }
    output.close();
    return 0;
}

int loadFromFile(int &hi_score, std::string &s){
    //setlocale(LC_ALL, "rus");
    std::string inputFileName {"hi_score.txt"};
    std::ifstream input(inputFileName);
    if (!input.is_open()) std::cout<<"Faled to open READ-file: "<< inputFileName << std::endl;
    else{
        input >> hi_score >> s;
    }
    input.close();
    return 0;
}

void gotoxy(int x, int y){
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

void setcursor(bool visible, DWORD size){
    if (size == 0) size = 20;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console,&lpCursor);
}

void drawBorder(){
    for (int i=0; i<SCREEN_HEIGHT; i++){
        for (int j=0; j<17; j++){
            gotoxy(0+j,i);          cout<<"±";
            gotoxy(WIN_WIDTH-j,i);  cout<<"±";
        }
    }
}

void genEnemy(int ind){
    enemyX[ind] = 17 + rand()%(33);
}

void drawEnemy(int ind){
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]+0); cout<<"****";
        gotoxy(enemyX[ind], enemyY[ind]+1); cout<<" ** ";
        gotoxy(enemyX[ind], enemyY[ind]+2); cout<<"****";
        gotoxy(enemyX[ind], enemyY[ind]+3); cout<<" ** ";
    }
}

void eraseEnemy(int ind){
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]+0); cout<<"    ";
        gotoxy(enemyX[ind], enemyY[ind]+1); cout<<"    ";
        gotoxy(enemyX[ind], enemyY[ind]+2); cout<<"    ";
        gotoxy(enemyX[ind], enemyY[ind]+3); cout<<"    ";
    }
}

void resetEnemy(int ind){
    eraseEnemy(ind);
    enemyY[ind] = 1;
    genEnemy(ind);
}

void drawCar(){
    for (int i=0; i<4; ++i){
        for (int j=0; j<4; ++j){
            gotoxy(j+carPos, i+SCREEN_HEIGHT-4); cout<<car[i][j]; ///gotoxy(j+carPos, i+22)
        }
    }
}

void eraseCar(){
    for (int i=0; i<4; ++i){
        for (int j=0; j<4; ++j){
            gotoxy(j+carPos, i+SCREEN_HEIGHT-4); cout<<" "; ///gotoxy(j+carPos, i+22)
        }
    }
}

int collision(){
    if ( enemyY[0]+4 >= SCREEN_HEIGHT-3){///enemyY[0]+4 >= 23
        if ( enemyX[0]+4-carPos >= 0 && enemyX[0]+4-carPos < 9 ){
            return 1;
        }
    }
    if ( enemyY[1]+4 >= SCREEN_HEIGHT-3){///enemyY[0]+4 >= 23
        if ( enemyX[1]+4-carPos >= 0 && enemyX[1]+4-carPos < 9 ){
            return 1;
        }
    }
    return 0;
}

void gameOver(){
    system("cls");
    if (score > 10) {score -= 10; return;}
    PlaySound(TEXT("gameover_said.wav"), NULL, SND_FILENAME | SND_ASYNC);
    Sleep(2500);
    PlaySound(TEXT("gameover.wav"), NULL, SND_FILENAME | SND_ASYNC);

    gotoxy(WIN_WIDTH-10, 10);
    cout<<endl;
    cout<<"\t\t--------------------------------"<<endl;
    cout<<"\t\t---------- Game Over -----------"<<endl;
    cout<<"\t\t--------------------------------"<<endl;

    if (flag_hi_score) {
        cout<<endl<<endl;
        cout<<"\t\tWe've got a NEW RECORD: "<<hi_score<<endl;
        cout<<"\t\tPlease input your name champion:"<<endl;
        gotoxy(25, 18);
        cin >> champion;
        logToFile(hi_score, champion); /// save hi_score and champion name to the log file
        flag_hi_score = false;
    }
    cout<<"\t\tPress any key to go back to menu";
    PlaySound(TEXT("background_sound.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
    getch();
}

void updateScoreAndSpeed(){
    gotoxy(WIN_WIDTH+7, 5); cout<<"Score: "<<score<<"   "<<endl;
    if (score > hi_score) {hi_score = score; flag_hi_score = true;};
    speed = 50 - score/3;
}

void updatePause(){
    gotoxy(WIN_WIDTH+6, 9);
    if (pause)  cout<<"--PAUSE--";
    else        cout<<"         ";
}


void instructions(){
    system("cls");
    cout<<"Instructions";
    cout<<"\n--------------------\n";
    cout<<"Avoid Cars by moving left or right.";
    cout<<"\n\nPress <- to move left";
    cout<<"\n\nPress -> to move right";
    cout<<"\n\nPress 'ESC' to exit";
    cout<<"\n\nPress any key to go back to menu";
    getch();
}

void play(){
    PlaySound(TEXT("F1race.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
    carPos = -1 + WIN_WIDTH/2;
    score = 0;
    enemyFlag[0] = 1; enemyFlag[1] = 0;
    enemyY[0] = enemyY[1] = 1;

    system("cls");
    drawBorder();
    updateScoreAndSpeed();
    genEnemy(0); genEnemy(1);

    gotoxy(WIN_WIDTH+7, 2);     cout<<"Car Game";
    gotoxy(WIN_WIDTH+6, 4);     cout<<"----------";
    gotoxy(WIN_WIDTH+6, 6);     cout<<"----------";
    gotoxy(WIN_WIDTH+7, 12);    cout<<"Control ";
    gotoxy(WIN_WIDTH+7, 13);    cout<<"----------";
    gotoxy(WIN_WIDTH+2, 14);    cout<<" <- Key: Left";
    gotoxy(WIN_WIDTH+2, 15);    cout<<" -> Key: Right";
    gotoxy(18, 5);              cout<<"Press any key to start";
    getch();
    gotoxy(18, 5);              cout<<"                      ";

    while (1){
        if (kbhit()){
            char ch = getch();
            if (!pause && ch == 75){
                if (carPos > 18)    carPos -= 2;
            }
            if (!pause && ch == 77){
                if (carPos < 50)    carPos += 2;
            }
            if (ch == ' '){
                pause = !pause; /// PAUSE
                if (pause)  PlaySound(TEXT("background_sound.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
                if (!pause) PlaySound(TEXT("F1race.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
            }
            if (ch == 27)           break;
        }

        updatePause();
        drawCar();
        drawEnemy(0); drawEnemy(1);

        if ( score < 0 ) {gameOver();  return;}

        if (!pause){
            bool collisionFlag = false;
            if ( collision() == 1 ) {collisionFlag = true; score -= damage;}
            if (collisionFlag){
                PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(100);
            }

            Sleep(speed);
            eraseCar(); eraseEnemy(0); eraseEnemy(1);

            if (collisionFlag) PlaySound(TEXT("F1race.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

            if (enemyY[0] == 10 && enemyFlag[1] == 0)   enemyFlag[1] = 1;
            if (enemyFlag[0] == 1)                      enemyY[0] += 1;
            if (enemyFlag[1] == 1)                      enemyY[1] += 1;
            if (enemyY[0] > SCREEN_HEIGHT - 4)         {resetEnemy(0); ++score; updateScoreAndSpeed();}
            if (enemyY[1] > SCREEN_HEIGHT - 4)         {resetEnemy(1); ++score; updateScoreAndSpeed();}
        }
    }
}

int main(){

    setcursor(0,0);
    srand((unsigned)time(NULL));
    loadFromFile(hi_score, champion);   /// load hi_score and champion name from the log file
    PlaySound(TEXT("background_sound.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

    do{

        system ("cls");
        gotoxy(10, 5);      cout<<"------------------------------";
        gotoxy(10, 6);      cout<<"|         Car Game           |";
        gotoxy(10, 7);      cout<<"------------------------------";
        gotoxy(10, 9);      cout<<"1. Start Game";
        gotoxy(10, 10);     cout<<"2. Instructions";
        gotoxy(10, 11);     cout<<"ESC: Quit";
        gotoxy(10, 14);     cout<<"Current champion is "<<champion<<" with hi-score "<<hi_score<<"!";
        gotoxy(10, 17);     cout<<"Select option: ";
        char op = getche();

        if      (op == '1')     play();
        else if (op == '2')     instructions();
        else if (op == 27)      exit(0);
    }while(1);


    return 0;
}
