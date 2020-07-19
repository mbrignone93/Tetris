/* TETRIS CLONE VERSION 1.0 */
/* PROGRAMADO POR MAXIMILIANO BRIGNONE © 2017 */
/* DESARROLLADO CON LIBRERIA MiniWin */
/* MiniWin ES PROPIEDAD DE PAU FERNANDEZ(c) */

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
#include "miniwin.h"
#include <windows.h>

using namespace std;
using namespace miniwin;

const int TAM = 25;
const int FILAS = 20;
const int COLUMNAS = 10;
const int MARGEN = 20;

typedef int Tablero[COLUMNAS][FILAS];

struct Coord { int x, y; };

struct Pieza {
    Coord orig;     // bloque central, posicion absoluta de la pieza
    Coord perif[3]; // bloques perifericos, posicion relativa al origen
    int color;

    Coord posicion(int n) const; // n entre 0 y 3, 0 = centro, 1, 3 perifericos
};

Coord Pieza::posicion(int n) const {
    Coord ret { orig.x, orig.y };
    if (n != 0) {
        ret.x += perif[n - 1].x;
        ret.y += perif[n - 1].y;
    }
    return ret;
}

void cuadrado(int x, int y){
    rectangulo_lleno(MARGEN + 1 + x * TAM,
                     MARGEN + 1 + y * TAM,
                     MARGEN + x * TAM + TAM,
                     MARGEN + y * TAM + TAM);
}

void pinta_pieza (const Pieza& P){
    color(P.color);
    for (int i = 0; i < 4; i++) {
       Coord c = P.posicion(i);
       cuadrado(c.x, c.y);
    }
}

Coord rotar(const Coord& c) {
    Coord ret = { -c.y , c.x };
    return ret;
}

void rotar(Pieza& P) {
    for (int i = 0; i < 3; i++) {
        P.perif[i] = rotar(P.perif[i]);
    }
}

void tablero_vacia(Tablero& T) {
   for (int i = 0; i < COLUMNAS; i++) {
       for (int j = 0; j < FILAS; j++) {
           T[i][j] = NEGRO; // esto significa casilla vacia
       }
   }
}

void tablero_pinta(const Tablero& T) {
   for (int i = 0; i < COLUMNAS; i++) {
       for (int j = 0; j < FILAS; j++) {
           color(T[i][j]);
           cuadrado(i, j);
       }
   }
}

void tablero_incrusta_pieza(Tablero& T, const Pieza& P) {
   for (int i = 0; i < 4; i++) {
      Coord c = P.posicion(i);
      T[c.x][c.y] = P.color;
   }
}

bool tablero_colision(const Tablero& T, const Pieza& P) {
   for (int i = 0; i < 4; i++){
       Coord c = P.posicion(i);
       // Comprobar limites
       if (c.x < 0 || c.x >= COLUMNAS) {
           return true;
       }
       if (c.y < 0 || c.y >= FILAS) {
           return true;
       }
       // mirar rejunte de piezas
       if (T[c.x][c.y] != NEGRO) {
           return true;
       }
   }
   return false;
}

const Coord perifs[7][3] = {
   { { 1, 0 }, { 0, 1 }, { 1, 1 } }, // cuadrado
   { { 1, 0 }, { -1, 1 }, { 0, 1 } }, // S
   { { 0, 1 }, { 1, 1 }, { -1, 0 } }, // 2
   { { 0, 1 }, { 0, -1 }, { 1, 1 } }, // L
   { { 0, 1 }, { 0, -1 }, { -1, 1 } }, // Lr
   { { -1, 0 }, { 1, 0 }, { 0, 1 } }, // T
   { { 0, 1 }, { 0, -1 }, { 0, 2 } }, // Palo
};

void pieza_nueva(Pieza& P) {
   P.orig.x = 12;
   P.orig.y = 2;
   P.color = 1 + rand() % 7;
   // pieza al azar
   int r = rand() % 7;
   for (int i = 0; i < 3; i++) {
       P.perif[i] = perifs[r][i];
   }
}

bool tablero_fila_llena(const Tablero& T, int fila) {
   for (int i = 0; i< COLUMNAS; i++) {
       if (T[i][fila] == NEGRO) return false;
   }
   return true;
}

void tablero_colapsa(Tablero& T, int fila) {
   // copiar de abajo a arriba
   for (int j = fila; j > 0; j--) {
       for (int i = 0; i < COLUMNAS; i++) {
           T[i][j] = T[i][j-1];
       }
   }
   //vaciar la de arriba
   for (int i = 0; i < COLUMNAS; i++) {
       T[i][0] = NEGRO;
   }
}

int tablero_cuenta_lineas(Tablero& T) {
   int fila = FILAS - 1, cont = 0;
   while (fila >= 0) {
       if (tablero_fila_llena(T, fila)) {
           tablero_colapsa(T, fila);
           cont++;
       } else {
           fila--;
       }
   }
   return cont;
}

string a_string(int num) {
   stringstream sout;
   sout << num;
   return sout.str();
}

void repinta (const Tablero& T, Pieza& p, const Pieza& sig, int puntos, int nivel) {
   const int ancho = TAM * COLUMNAS;
   const int alto = TAM * FILAS;
   borra();
   tablero_pinta(T);
   color_rgb(128,128,128);
   linea(MARGEN, MARGEN, MARGEN, MARGEN + alto);
   linea(MARGEN, MARGEN + alto, MARGEN + ancho, MARGEN + alto);
   linea(MARGEN + ancho, MARGEN + alto, MARGEN + ancho, MARGEN);
   // texto
   texto(MARGEN * 2 + ancho, MARGEN, "SIGUIENTE");
   texto(MARGEN * 2 + ancho, MARGEN + 140, "NIVEL");
   texto(MARGEN * 2 + ancho, MARGEN + 250, "PUNTOS");
   color_rgb(128,128,128);
   texto(MARGEN * 2 + ancho, MARGEN + 270, a_string(puntos));
   texto(MARGEN * 2 + ancho, MARGEN + 160, a_string(nivel));

   texto(60, 550, "Programando en C++ © 2017 Por Maximiliano Brignone");

   pinta_pieza(p);
   pinta_pieza(sig);
   refresca();
}

const int niveles[10] = {
   1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 15000
};

const int tics_nivel[10] = {
   33, 30, 27, 24, 21, 18, 15, 12, 9
};

void iniciar_juego(){
   srand(time(0)); // inicializar numeros al azar

   char musica[] = "theme.wav";
   PlaySound((LPCSTR)musica, NULL, SND_FILENAME | SND_ASYNC);

   int tic = 0, puntos = 0, nivel = 1;

   Tablero T;
   tablero_vacia(T);
   Pieza c, sig;
   pieza_nueva(c);
   pieza_nueva(sig);
   c.orig.x = 5;
   repinta(T, c, sig, puntos, nivel);

   pinta_pieza(c);

   int t = tecla();

   while (t != ESCAPE) {
       // copiar pieza actual
       Pieza copia = c;

       // lograr que la piez abaje sola cada 30 milisegundos
       if (t == NINGUNA && tic > 33){
           tic = 0;
           t = ABAJO;
       }
       // pausar juego
       if (t == int('P')){
          // PAUSA...
          mensaje("Juego en Pausa. Presiona Aceptar Para Reanudar");
          //system("pause");
       }

       // mover en el papel
       if (t == ABAJO) {
         c.orig.y++;
         puntos += 2;
       } else if (t == ARRIBA) {
            rotar(c);
       } else if (t == DERECHA) {
            c.orig.x++;
       } else if (t == IZQUIERDA) {
            c.orig.x--;
       }
       // mirar si hay colision
       if (tablero_colision(T, c)) {
           c = copia;
           if (t == ABAJO) {
               tablero_incrusta_pieza(T, c);
               int cont = tablero_cuenta_lineas(T);
               puntos += puntos * cont;
               if (puntos >= niveles[nivel])
                  nivel++;
               c = sig;
               pieza_nueva(sig);
               /*char line[] = "line.wav";
               PlaySound((LPCSTR)line, NULL, SND_FILENAME | SND_ASYNC);*/
               c.orig.x = 5;
               if (tablero_colision(T, c)){
                  char game_over[] = "game_over.wav";
                  PlaySound((LPCSTR)game_over, NULL, SND_FILENAME | SND_ASYNC);
                  mensaje("GAME OVER...");
                  iniciar_juego();
               }
           }
       }
       // repintar
       if (t != NINGUNA) {
           repinta(T, c, sig, puntos, nivel);
       }

       espera(30);
       tic++;
       t = tecla();
   }
}

int main() {
   vredimensiona(TAM * COLUMNAS + 220, TAM * FILAS + 100);
   mensaje("Para Pausar el Juego Presione la Tecla P");
   iniciar_juego();
   vcierra();
   return 0;
}
