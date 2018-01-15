// Parque

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

   // pesco�o
   glm::mat4 matPesc = M;
   matPesc = glm::translate(matPesc, glm::vec3(xCorpo/2-xPesc/2, yCorpo/2+yPesc/2, 0.0));
   matPesc = glm::scale(matPesc, glm::vec3(xPesc, yPesc, zPesc));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPesc));
   cubo.desenhar();

   // cabe�a
   glm::mat4 matCab = M;
   matCab = glm::translate(matCab, glm::vec3(xCorpo/2.0+xCab/2.0-xPesc, yCorpo/2.0+yPesc+yCab/2.0, 0.0));
   matCab = glm::scale(matCab, glm::vec3(xCab, yCab, zCab));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCab));
   cubo.desenhar();
}

void carrossel(glm::mat4 posicao)
{
    static float angCar=0.0; // rotacao do carrossel
    float altMedia=4.0;      // altura m�dia dos cavalos
    float amplitude=2.0;     // amplitude da varia��o da altura
    float raioCar=9.0;       // raio da circunfer�ncia do carrossel

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
    float diamMastro = 0.3;  //di�metro do mastro
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
    // ch�o (placa de dimens�es 80x0.3x80, com face superior em y=0.0)
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
    glm::mat4 posicao;
    posicao = glm::translate(posicao, glm::vec3(0.0,0.0,-30.0));
    carrossel(posicao);

    desenhaFonteDeLuz(light_position1, 0.5);
    desenhaFonteDeLuz(light_position2, 0.5);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void posicionaCamera(unsigned char tecla) {
   static float angCam = 0.0;  // giro da c�mera em torno da origem
   float raioCam = 50.0; // raio da circunfer�ncia do movimento da c�mera
   static glm::vec3 eye;
   glm::vec3 center(0.0,0.0,0.0);
   glm::vec3 up(0.0,1.0,0.0);
   switch( tecla ) {
     case 'd': // camera em posi��o default
        eye = glm::vec3(0.0,10.0,raioCam);
	    break;
     case 'j': // camera gira para a esquerda
        angCam -= 0.5;
	    break;
     case 'l':  // camera gira para a direita
        angCam += 0.5;
	    break;
     case 'i': // camera sobe
        eye.y += 0.2;
	    break;
     case 'k':  // camera desce
        eye.y -= 0.2;
	    break;
   }
   // calcular posicao da camera no plano XZ
   float angCamRad = glm::radians(angCam);
   eye.x = sin(angCamRad)*raioCam;
   eye.z = cos(angCamRad)*raioCam;
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
