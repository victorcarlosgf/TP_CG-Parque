// Parque
/*
 * Feito por:
 *       Felipe Fontenele de Ávila Magalhães 15.1.4331
 *       Matheus Gonçalves Ferreira          15.1.4341
 *       Victor Freitas                      17.2.4254
*/
/** COMANDOS DE CONTROLE
 *       'w': deslocamento para frente de uma pessoa no parque.
 *       's': deslocamento para tras de uma pessoa no parque.
 *       'p': modo de visualizacao de uma camera situada em posicao elevada 
 *       'i': deslocamento para cima.
 *       'k': deslocamento para baixo.
 *       'j': deslocamento lateral para esquerda.
 *       'l': deslocamento lateral para direito.
 */
/*
  Link do video para detalhamento do carrossel:
    https://www.youtube.com/watch?v=xG19jKZ9agI
*/
//glm: biblioteca matem�tica para OpenGL
#include <glm/glm.hpp> //vec2, vec3, mat4, etc
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
// arquivo com recursos auxiliares (do livro texto)

using namespace std;
#include "Angel_util.h"
#include "Cilindro.cpp"
#include "Cubo.cpp"
#include "cone.cpp"
#include "Sphere.cpp"

typedef glm::vec4  color4;
typedef glm::vec4  point4;

GLuint Model, View, Projection, isLightSource;
GLuint program;

Cubo cubo;
Cilindro cilindro;
Cone cone;
Sphere esfera;

static float angulo=0.0;    // Angulo rotacao do skate
static float movSkate=0.0;  // Controlar a ida e volta do skate
static float altSkate=1.0;  // Controlar a altura do skate
static int topo = 0;        // Controlar o momento certo da rotacao do skate

glm::vec4 light_position1(  50.0f, 12.0f, 0.0f, 1.0f );
glm::vec4 light_position2( -50.0f, 12.0f, 0.0f, 1.0f );

//----------------------------------------------------------------------------

void desenhaFonteDeLuz(glm::vec4 pos, float r)
{   // desenha uma esfera de raio r, representando a fonte de luz
    glm::mat4 luz;
    luz = glm::translate(luz, glm::vec3(pos.x,pos.y,pos.z));
    luz = glm::scale(luz, glm::vec3(r,r,r));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(luz));
    glUniform1i(isLightSource, true);
    esfera.draw();
    glUniform1i(isLightSource, false);
}

//----------------------------------------------------------------------------

void cavalo(glm::mat4 M) {
    // Modelo com parte inferior do corpo (barriga) situada
    // em y=0.0, ou seja, no ch�o
    // Matriz M: transforma��es para posicionar o cavalo

    //Dimens�es das partes do cavalo
    float xCorpo=1.5, yCorpo=1.0, zCorpo=0.5;
    float xPesc=0.3,  yPesc=0.6,  zPesc=0.2;  //pesco�o
    float xCab=0.5,   yCab=0.2,   zCab=0.2;   //cabe�a
    M = glm::translate(M, glm::vec3(0.0, yCorpo/2.0, 0.0));

   // corpo (orientado segundo o eixo X)
   glm::mat4 matCorpo = M;
   matCorpo = glm::scale(matCorpo, glm::vec3(xCorpo, yCorpo, zCorpo));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCorpo));
   cubo.desenhar();

   // pescoco
   glm::mat4 matPesc = M;
   matPesc = glm::translate(matPesc, glm::vec3(xCorpo/2-xPesc/2, yCorpo/2+yPesc/2, 0.0));
   matPesc = glm::scale(matPesc, glm::vec3(xPesc, yPesc, zPesc));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPesc));
   cubo.desenhar();

   // cabeca
   glm::mat4 matCab = M;
   matCab = glm::translate(matCab, glm::vec3(xCorpo/2.0+xCab/2.0-xPesc, yCorpo/2.0+yPesc+yCab/2.0, 0.0));
   matCab = glm::scale(matCab, glm::vec3(xCab, yCab, zCab));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCab));
   cubo.desenhar();
}

void sustentacao(glm::mat4 posicao)
{
    // plataforma (placa de dimens�es 40x0.8x22, com face inferior em y=0.0)
    float altPlat=0.8;
    glm::mat4 matPlat(posicao);
    matPlat = glm::translate(matPlat, glm::vec3(0.0, altPlat/2.0, 0.0));
    matPlat = glm::scale(matPlat, glm::vec3(40.0, 0.8, 28.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPlat));
    cubo.desenhar();

    // mastros sustentacao skate
    float altMastro = 15.0;
    float diamMastro = 0.3;  //diametro do mastro
    float posX = -20.0, posZ = -4.0;

    glm::mat4 matMastro, rotMastro(posicao);
    for (int i = 0; i < 4; i++){
       matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,posZ));
       matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
       matMastro = glm::scale(matMastro, glm::vec3(diamMastro,diamMastro,altMastro));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
       cilindro.desenhar();

        if(i == 0)
          posZ = 4.0;
        else if(i == 1)
          posX = 20.0, posZ = -4.0;
        else if(i == 2)
          posX = 20.0, posZ = 4.0;
    }
    int i = 1;
    posX = -19.5;
    float aux;
    while(posX < 0.0){
        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();

        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();
        

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i+0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i+0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();
        posX += 0.75;
        i++;
    }
    i = 1;
    posX = 19.5;
    while(posX > 0.0){
        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();

        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i-0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i-0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        cilindro.desenhar();
        posX -= 0.75;
        i++;
    }

    float aux1 = 8.0, aux2 = 7.0;
    glm::mat4 matDegrau, rotDegrau(posicao);
    for (int i = 0; i < 3; i++){
       matDegrau = glm::translate(rotDegrau, glm::vec3(0.0, altPlat/2.0 + (i+1)*0.8, aux1));
       matDegrau = glm::scale(matDegrau, glm::vec3(15.0, 0.8, aux2));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matDegrau));
       cubo.desenhar();

       matDegrau = glm::translate(rotDegrau, glm::vec3(0.0, altPlat/2.0 + (i+1)*0.8, -aux1));
       matDegrau = glm::scale(matDegrau, glm::vec3(15.0, 0.8, -aux2));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matDegrau));
       cubo.desenhar();

       aux1 -= 1.0;
       aux2 -= ((i+1)*0.8);
    }
}

void movimentoSkate(void)
{
    bool flag[2] = {true, false};
    static bool aux1 = false, aux2 = false;
   
    static float deslocamento = 0.015;
    static float deslocamento2 = 0.020;
    static int contador = 0, cont = 0, cont1 = 0;

    if(!aux1 && !aux2){
        if(altSkate >= 4.0 || altSkate <= 0.9){
            deslocamento *= -1;
            if(flag[contador]){
                deslocamento2 *= -1;
            }
            cont++;
            contador++;
            if(contador == 2){
                contador = 0;
            }
            if(cont == 4){
                aux1 = true;
                cont = 0;
                cont1 = 1;
            }
        }
    }
    if(!aux2 && aux1){
        if((altSkate >= 6.5 || altSkate <= 0.9) && cont1 != 1){
            deslocamento *= -1;
            if(flag[contador]){
                deslocamento2 *= -1;
            }
            cont++;
            contador++;
            if(contador == 2){
                contador = 0;
            }
            if(cont == 4){
                aux2 = true;
                cont = 0;
                cont1 = 1;
            }
        }
        else
            cont1 = 0;
    }
    if(aux1 && aux2){
        if((altSkate >= 11.0 || altSkate <= 0.9) && cont1 != 1){
            deslocamento *= -1;
            if(flag[contador])
              deslocamento2 *= -1;
            
            contador++;
            if(contador == 2)
              contador = 0;
            topo = 1;
        }
        else
            cont1 = 0;
    }

    altSkate += deslocamento;
    movSkate += deslocamento2;
        
    angulo += 0.2;
    if(angulo > 360)
        angulo = 0.0;
}

void skate(glm::mat4 posicao)
{
    glm::mat4 matSkate(posicao);
    matSkate = glm::translate(matSkate, glm::vec3(0.0, 4.0, 0.0));
    matSkate = glm::translate(matSkate, glm::vec3(movSkate,altSkate,0.0));
    if(topo){
        matSkate = glm::rotate(matSkate, glm::radians(angulo), glm::vec3(0.0f,1.0f,0.0f));
    }
    matSkate = glm::scale(matSkate, glm::vec3(10.0, 0.25, 7.9));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matSkate));
    cubo.desenhar();

    float posX = -2.0, posZ = -1.5;
    glm::mat4 matCadeira, rotCadeira(posicao);
    for (int i = 0; i < 4; i++){
       matCadeira = glm::translate(rotCadeira, glm::vec3(posX,4.25,posZ));
       matCadeira = glm::translate(matCadeira, glm::vec3(movSkate,altSkate,0.0));
       if(topo){
          matCadeira = glm::rotate(matCadeira, glm::radians(angulo), glm::vec3(0.0f,1.0f,0.0f));
       }
       matCadeira = glm::scale(matCadeira, glm::vec3(2.5,0.5,1.5));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCadeira));
       cubo.desenhar();

        if(i == 0)
          posZ = 1.5;
        else if(i == 1)
          posX = 2.0, posZ = -1.5;
        else if(i == 2)
          posX = 2.0, posZ = 1.5;
    }
    movimentoSkate();
}

void carrossel(glm::mat4 posicao)
{
    static float angCar=0.0; // rotacao do carrossel
    float altMedia=4.0;      // altura media dos cavalos
    float amplitude=2.0;     // amplitude da variacao da altura
    float raioCar=9.0;       // raio da circunferencia do carrossel

    angCar += 0.001;
    if( angCar > 2.0*M_PI ) angCar -= 2.0*M_PI;

    // plataforma (placa de dimens�es 21x0.8x21, com face inferior em y=0.0)
    float altPlat=0.8;
    glm::mat4 matPlat(posicao);
    matPlat = glm::translate(matPlat, glm::vec3(0.0, altPlat/2.0, 0.0));
    matPlat = glm::scale(matPlat, glm::vec3(21.0, 0.8, 21.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPlat));
    cubo.desenhar();

    // mastro (eixo) do carrossel
    float altMastro = 10.0;
    float diamMastro = 0.3;  //diametro do mastro
    glm::mat4 matMastro(posicao);
    matMastro = glm::rotate(matMastro, angCar, glm::vec3(0.0f,1.0f,0.0f));
    matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matMastro = glm::scale(matMastro, glm::vec3(diamMastro,diamMastro,altMastro));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
    cilindro.desenhar();

    // cobertura
    glm::mat4 matCob(posicao);
    matCob = glm::translate(matCob, glm::vec3(0.0, 10.0, 0.0));
    matCob = glm::rotate(matCob, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matCob = glm::scale(matCob, glm::vec3(10.0, 10.0, 2.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCob));
    cone.desenhar();

    // 8 cavalos com hastes
    float altCav = altMedia + amplitude*sin(2.0*angCar);
    float angDefasagem = 2.0*M_PI/8.0; // defasagem entre os cavalos
    float diamHaste = 0.1;
    glm::mat4 matCavalo, rotCavalo(posicao), matHaste;
    rotCavalo = glm::rotate(rotCavalo, angCar, glm::vec3(0.0f,1.0f,0.0f));
    for (int i = 0; i < 8; i++){
       rotCavalo = glm::rotate(rotCavalo, angDefasagem, glm::vec3(0.0f,1.0f,0.0f));
       matCavalo = glm::translate(rotCavalo, glm::vec3(0.0f,altCav,raioCar));
       cavalo(matCavalo);

       matHaste = glm::translate(rotCavalo, glm::vec3(0.0f,0.0f,raioCar));
       matHaste = glm::rotate(matHaste, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
       matHaste = glm::scale(matHaste, glm::vec3(diamHaste,diamHaste,altMastro));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matHaste));
       cilindro.desenhar();
    }
}

void chao()
{
    // chao (placa de dimens�es 80x0.3x80, com face superior em y=0.0)
    glm::mat4 matChao;
    matChao = glm::translate(matChao, glm::vec3(0.0, -0.15, 0.0));
    matChao = glm::scale(matChao, glm::vec3(80.0, 0.3, 80.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matChao));
    cubo.desenhar();
}

void exibe( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    chao();
    glm::mat4 posicaoCarrosel;
    posicaoCarrosel = glm::translate(posicaoCarrosel, glm::vec3(0.0,0.0,-30.0));
    carrossel(posicaoCarrosel);

    glm::mat4 posicaoSkate;
    posicaoSkate = glm::translate(posicaoSkate, glm::vec3(0.0,0.0,10.0)); // -30, 0, 0
    sustentacao(posicaoSkate);
    skate(posicaoSkate);

    desenhaFonteDeLuz(light_position1, 0.5);
    desenhaFonteDeLuz(light_position2, 0.5);

    glutSwapBuffers();
}

// ============================================= Camera ======================================================
bool troca_camera;
static glm::vec3 eye;

void camera(bool troca){
    if(troca)
        eye = glm::vec3(0.0,50.0,50.0);
    else
        eye = glm::vec3(0.0,10.0,50.0);
}

void posicionaCamera(unsigned char tecla) {
   static float angCam = 0.0;   // giro da camera em torno da origem
   float raioCam = 50.0;        // raio da circunferencia do movimento da camera
   static float transCam = 1.0;
   
   glm::vec3 center(0.0,0.0,0.0);
   glm::vec3 up(0.0,1.0,0.0);
   switch( tecla ) {
     case 'd': // camera em posicao default
        eye = glm::vec3(0.0,10.0,raioCam);
      break;
     case 'j': // camera gira para a esquerda
        angCam -= 1.0;
      break;
     case 'l':  // camera gira para a direita
        angCam += 1.0;
      break;
     case 'i': // camera sobe
        eye.y += 0.5;
      break;
     case 'k':  // camera desce
        eye.y -= 0.5;
      break;
    case 'p': // troca posicao da camera
        troca_camera = !troca_camera;
        camera(troca_camera);
        break;
    case 'w': // camera anda para frente
        transCam -= 2.5;
        break;
    case 's': // camera anda para tras
        transCam += 2.5;
        break;
   }
   // calcular posicao da camera no plano XZ
   float angCamRad = glm::radians(angCam);
   eye.x = sin(angCamRad)*raioCam;
   eye.z = cos(angCamRad)*raioCam;

   // calcular translacao da camera para frente e para tras
   eye.x += transCam;
   eye.z += transCam;   

   glm::mat4 matVis = glm::lookAt(eye, center, up);
   glUniformMatrix4fv(View,1,GL_FALSE, glm::value_ptr(matVis));
   glutPostRedisplay();
}


void teclado( unsigned char tecla, int x, int y )
{
    if (tecla==033)   // Escape Key
      exit( EXIT_SUCCESS );
    else
        posicionaCamera(tecla);
}

//----------------------------------------------------------------------------

void idle( void )
{
   exibe();
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init()
{
    // Load shaders and use the resulting shader program
    program = InitShader( "genericoVshader.glsl",
                          "genericoFshader.glsl" );
    glUseProgram( program );

    cubo.criaCubo(program, "vPosition", "vNormal");
    cilindro.criaCilindro(program, "vPosition", "vNormal");
    cone.criaCone(program, "vPosition", "vNormal");
    esfera.init(program, "vPosition", "vNormal");

//**************************************************
    // Initialize shader lighting parameters
    glm::vec4 light_ambient( 0.2f, 0.2f, 0.2f, 1.0f );
    glm::vec4 light_diffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    glm::vec4 light_specular( 1.0f, 1.0f, 1.0f, 1.0f );

    glm::vec4 material_ambient( 1.0f, 0.0f, 1.0f, 1.0f );
    glm::vec4 material_diffuse( 0.8f, 0.6f, 0.0f, 1.0f );
    glm::vec4 material_specular( 0.9f, 0.7f, 0.0f, 1.0f );
    float  material_shininess = 100.0f;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
      1, glm::value_ptr(ambient_product ));
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
      1, glm::value_ptr(diffuse_product ));
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
      1, glm::value_ptr(specular_product ));

    glUniform4fv( glGetUniformLocation(program, "LightPosition1"),
      1, glm::value_ptr(light_position1 ));
    glUniform4fv( glGetUniformLocation(program, "LightPosition2"),
      1, glm::value_ptr(light_position2 ));

    glUniform1f( glGetUniformLocation(program, "Shininess"),
     material_shininess );

    // Retrieve transformation uniform variable locations
    Model = glGetUniformLocation( program, "Model" );
    Projection = glGetUniformLocation( program, "Projection" );
    View = glGetUniformLocation( program, "View" );

    isLightSource = glGetUniformLocation( program, "isLightSource" );

//**************************************************

    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 200.0f);
    glUniformMatrix4fv(Projection,1,GL_FALSE, glm::value_ptr(proj));
    glUniform1i(isLightSource, false);

    posicionaCamera('d'); //posi��o default
} //init

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Parque" );

    glewInit();

    init();

    glutDisplayFunc( exibe );
    glutKeyboardFunc( teclado );
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
