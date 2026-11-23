/*---------------------------------------------------------*/
/* ----------- Proyecto Final: Panteón Oaxaca -------------*/
/*---------------     Semestre 2027-1     -----------------*/
/*---------------    Nombre del alumno:   -----------------*/
/*---------------       319097542         -----------------*/
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <Windows.h>
#include <glad/glad.h>
#include <glfw3.h>                      
#include <stdlib.h>     
#include <glm/glm.hpp>                  
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>                  
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);
void getResolution(void);


// Configuración de ventana
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;


// Cámara sintética (inicia sobre el piso)
Camera camera(glm::vec3(0.0f, 140.0f, 650.0f));
float MovementSpeed = 2.0f;
GLfloat lastX = SCR_WIDTH / 2.0f,
        lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// Control de tiempo (60 FPS)
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS;
double deltaTime = 0.0f,
lastFrame = 0.0f;


// Variables para Esqueleto 2 (junto a la ofrenda)
float esq2_saltoY = 0.0f;
float esq2_giroTorso = 0.0f;
float esq2_rotCabeza = 0.0f;
float esq2_rotBrazoDer = 0.0f;
float esq2_rotBrazoIzq = 0.0f;
float esq2_rotPiernaDer = 0.0f;
float esq2_rotPiernaIzq = 0.0f;


// Variables para Esqueleto 3 (reverencias frente a una tumba)
float esq3_inclinacion = 0.0f;
float esq3_rotCabezaX = 0.0f;
float esq3_rotBrazos = 0.0f;


// Variables para KeyFrames (Baile articulado)
float posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
giroTorso = 0.0f,
rotCabeza = 0.0f,
rotBrazoDer = 0.0f,
rotBrazoIzq = 0.0f,
rotPiernaDer = 0.0f,
rotPiernaIzq = 0.0f;

float incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
incGiroTorso = 0.0f,
incCabeza = 0.0f,
incBrazoDer = 0.0f,
incBrazoIzq = 0.0f,
incPiernaDer = 0.0f,
incPiernaIzq = 0.0f;


// Para la animación del viento
float tiempo = 0.0f, 
      anguloViento = 0.0f;
      

// Para la animación del fantasma
float alturaFantasma = 0.0f,
      movFantasma_x = 0.0f,
      movFantasma_z = 900.0f,
      orientaFantasma = 90.0f,
      pausa = 0.0f;
int estadoFantasma = 1;       


// Máquina de estados para el esqueleto central 
int estadoEsqueleto = 1;
float pausaEsqueleto = 0.0f;


#define MAX_FRAMES 9
int i_max_steps = 18;
int i_curr_steps = 0;


typedef struct _frame {
    float posX;
    float posY;
    float posZ;
    float giroTorso;
    float rotCabeza;
    float rotBrazoDer;
    float rotBrazoIzq;
    float rotPiernaDer;
    float rotPiernaIzq;
} FRAME;


FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 9;
bool play = false;
int playIndex = 0;


void resetElements(void) {
    posX = KeyFrame[0].posX;
    posY = KeyFrame[0].posY;
    posZ = KeyFrame[0].posZ;
    giroTorso = KeyFrame[0].giroTorso;
    rotCabeza = KeyFrame[0].rotCabeza;
    rotBrazoDer = KeyFrame[0].rotBrazoDer;
    rotBrazoIzq = KeyFrame[0].rotBrazoIzq;
    rotPiernaDer = KeyFrame[0].rotPiernaDer;
    rotPiernaIzq = KeyFrame[0].rotPiernaIzq;
}


void interpolation(void) {
    incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
    incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
    incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
    incGiroTorso = (KeyFrame[playIndex + 1].giroTorso - KeyFrame[playIndex].giroTorso) / i_max_steps;
    incCabeza = (KeyFrame[playIndex + 1].rotCabeza - KeyFrame[playIndex].rotCabeza) / i_max_steps;
    incBrazoDer = (KeyFrame[playIndex + 1].rotBrazoDer - KeyFrame[playIndex].rotBrazoDer) / i_max_steps;
    incBrazoIzq = (KeyFrame[playIndex + 1].rotBrazoIzq - KeyFrame[playIndex].rotBrazoIzq) / i_max_steps;
    incPiernaDer = (KeyFrame[playIndex + 1].rotPiernaDer - KeyFrame[playIndex].rotPiernaDer) / i_max_steps;
    incPiernaIzq = (KeyFrame[playIndex + 1].rotPiernaIzq - KeyFrame[playIndex].rotPiernaIzq) / i_max_steps;
}

// ------------------------------------------------------------------
// Variables para KeyFrames 2 (Abuela junto a la ofrenda)
// ------------------------------------------------------------------
float abuelaPosX = 0.0f,
abuelaPosY = 0.0f,
abuelaPosZ = 0.0f,
abuelaGiroTorso = 0.0f,
abuelaRotCabeza = 0.0f,
abuelaRotBrazoDer = 0.0f,
abuelaRotBrazoIzq = 0.0f,
abuelaRotPiernaDer = 0.0f,
abuelaRotPiernaIzq = 0.0f;

float abuelaIncX = 0.0f,
abuelaIncY = 0.0f,
abuelaIncZ = 0.0f,
abuelaIncGiroTorso = 0.0f,
abuelaIncCabeza = 0.0f,
abuelaIncBrazoDer = 0.0f,
abuelaIncBrazoIzq = 0.0f,
abuelaIncPiernaDer = 0.0f,
abuelaIncPiernaIzq = 0.0f;

#define MAX_FRAMES_ABUELA 7
int i_max_steps_abuela = 25;
int i_curr_steps_abuela = 0;

FRAME KeyFrameAbuela[MAX_FRAMES_ABUELA];
int FrameIndexAbuela = 7;
bool playAbuela = false;
int playIndexAbuela = 0;

void resetElementsAbuela(void) {
    abuelaPosX = KeyFrameAbuela[0].posX;
    abuelaPosY = KeyFrameAbuela[0].posY;
    abuelaPosZ = KeyFrameAbuela[0].posZ;
    abuelaGiroTorso = KeyFrameAbuela[0].giroTorso;
    abuelaRotCabeza = KeyFrameAbuela[0].rotCabeza;
    abuelaRotBrazoDer = KeyFrameAbuela[0].rotBrazoDer;
    abuelaRotBrazoIzq = KeyFrameAbuela[0].rotBrazoIzq;
    abuelaRotPiernaDer = KeyFrameAbuela[0].rotPiernaDer;
    abuelaRotPiernaIzq = KeyFrameAbuela[0].rotPiernaIzq;
}

void interpolationAbuela(void) {
    abuelaIncX = (KeyFrameAbuela[playIndexAbuela + 1].posX - KeyFrameAbuela[playIndexAbuela].posX) / i_max_steps_abuela;
    abuelaIncY = (KeyFrameAbuela[playIndexAbuela + 1].posY - KeyFrameAbuela[playIndexAbuela].posY) / i_max_steps_abuela;
    abuelaIncZ = (KeyFrameAbuela[playIndexAbuela + 1].posZ - KeyFrameAbuela[playIndexAbuela].posZ) / i_max_steps_abuela;
    abuelaIncGiroTorso = (KeyFrameAbuela[playIndexAbuela + 1].giroTorso - KeyFrameAbuela[playIndexAbuela].giroTorso) / i_max_steps_abuela;
    abuelaIncCabeza = (KeyFrameAbuela[playIndexAbuela + 1].rotCabeza - KeyFrameAbuela[playIndexAbuela].rotCabeza) / i_max_steps_abuela;
    abuelaIncBrazoDer = (KeyFrameAbuela[playIndexAbuela + 1].rotBrazoDer - KeyFrameAbuela[playIndexAbuela].rotBrazoDer) / i_max_steps_abuela;
    abuelaIncBrazoIzq = (KeyFrameAbuela[playIndexAbuela + 1].rotBrazoIzq - KeyFrameAbuela[playIndexAbuela].rotBrazoIzq) / i_max_steps_abuela;
    abuelaIncPiernaDer = (KeyFrameAbuela[playIndexAbuela + 1].rotPiernaDer - KeyFrameAbuela[playIndexAbuela].rotPiernaDer) / i_max_steps_abuela;
    abuelaIncPiernaIzq = (KeyFrameAbuela[playIndexAbuela + 1].rotPiernaIzq - KeyFrameAbuela[playIndexAbuela].rotPiernaIzq) / i_max_steps_abuela;
}


void animate(void) {
    // Variable de tiempo para las animaciones
    tiempo += 0.03f;


    // Movimiento del viento con una oscilación de 20 grados
    anguloViento = sin(tiempo * 2.0f) * glm::radians(20.0f);


    // Cambio de altura del fantasma de 20 unidades
    alturaFantasma = sin(tiempo * 1.5f) * 20.0f;


    // Máquina de estados para el movimiento del fastama en el escenario
    // Estado 1 - Se mueve hacia la derecha 
    if (estadoFantasma == 1) {
        if (movFantasma_x < 900.0f) {
            movFantasma_x += 2.0f;
            orientaFantasma = 90.0f;
        }
        else {
            pausa += 0.01f;
            if (pausa > 1.5f) {
                pausa = 0.0f;
                estadoFantasma = 2;
            }
        }
    }


    // Estado 2 - Se mueve hacia atrás
    else if (estadoFantasma == 2) {
        orientaFantasma = 180.0f;

        if (movFantasma_z > -1000.0f) {
            movFantasma_z -= 2.0f; 
        }

        else {
            pausa += 0.01f;
            
            if (pausa > 1.5f) {
                pausa = 0.0f;
                estadoFantasma = 3;
            }
        }
    }


    // Estado 3 - Se mueve a la izquierda
    else if (estadoFantasma == 3) {
        orientaFantasma = 270.0f; 
        
        if (movFantasma_x > -1000.0f) {
            movFantasma_x -= 2.0f;
        }
        
        else {
            pausa += 0.01f;

            if (pausa > 1.5f) {
                pausa = 0.0f;
                estadoFantasma = 4;
            }
        }
    }


    // Estado 4 - Se mueve hacia enfrente 
    else if (estadoFantasma == 4) {
        orientaFantasma = 0.0f;

        if (movFantasma_z < 900.0f) {
            movFantasma_z += 2.0f;
        }

        else {
            pausa += 0.01f;
            
            if (pausa > 1.5f) {
                pausa = 0.0f;
                estadoFantasma = 5;
            }
        }
    }


    // Estado 5 - Regresa al inicio
    else if (estadoFantasma == 5) {
        orientaFantasma = 90.0f;

        if (movFantasma_x < 0.0f) {
            movFantasma_x += 2.0f;
        }

        else {
            estadoFantasma = 1;
        }
    }


    // Máquina de estados para la caminata del esqueleto central
    // Estado 1 - Camina hacia la derecha 
    if (estadoEsqueleto == 1) {
        if (posX < 350.0f) {
            posX += 1.8f;
            giroTorso = 90.0f;
            rotPiernaDer = sin(tiempo * 4.0f) * 28.0f;
            rotPiernaIzq = -sin(tiempo * 4.0f) * 28.0f;
            rotBrazoDer = -sin(tiempo * 4.0f) * 22.0f;
            rotBrazoIzq = sin(tiempo * 4.0f) * 22.0f;
            rotCabeza = sin(tiempo * 2.0f) * 6.0f;
        }
        else {
            // Se detiene un momento antes de dar la vuelta
            rotPiernaDer = 0.0f;
            rotPiernaIzq = 0.0f;
            rotBrazoDer = 0.0f;
            rotBrazoIzq = 0.0f;
            pausaEsqueleto += 0.01f;
            if (pausaEsqueleto > 0.8f) {
                pausaEsqueleto = 0.0f;
                estadoEsqueleto = 2;
            }
        }
    }


    // Estado 2 - Camina hacia la izquierda 
    else if (estadoEsqueleto == 2) {
        if (posX > -350.0f) {
            posX -= 1.8f;
            giroTorso = -90.0f;
            rotPiernaDer = sin(tiempo * 4.0f) * 28.0f;
            rotPiernaIzq = -sin(tiempo * 4.0f) * 28.0f;
            rotBrazoDer = -sin(tiempo * 4.0f) * 22.0f;
            rotBrazoIzq = sin(tiempo * 4.0f) * 22.0f;
            rotCabeza = sin(tiempo * 2.0f) * 6.0f;
        }
        else {
            // Se detiene un momento antes de volver a la derecha
            rotPiernaDer = 0.0f;
            rotPiernaIzq = 0.0f;
            rotBrazoDer = 0.0f;
            rotBrazoIzq = 0.0f;
            pausaEsqueleto += 0.01f;
            if (pausaEsqueleto > 0.8f) {
                pausaEsqueleto = 0.0f;
                estadoEsqueleto = 1;
            }
        }
    }
        

    // Interpolación de KeyFrames 2 (Abuela)
    if (playAbuela) {
        if (i_curr_steps_abuela >= i_max_steps_abuela) {
            playIndexAbuela++;
            if (playIndexAbuela > FrameIndexAbuela - 2) {
                // En lugar de apagar playAbuela, reinicia el ciclo desde el Frame 0
                playIndexAbuela = 0;
                i_curr_steps_abuela = 0;
                resetElementsAbuela();
                interpolationAbuela();
            }
            else {
                i_curr_steps_abuela = 0;
                interpolationAbuela();
            }
        }
        else {
            abuelaPosX += abuelaIncX;
            abuelaPosY += abuelaIncY;
            abuelaPosZ += abuelaIncZ;
            abuelaGiroTorso += abuelaIncGiroTorso;
            abuelaRotCabeza += abuelaIncCabeza;
            abuelaRotBrazoDer += abuelaIncBrazoDer;
            abuelaRotBrazoIzq += abuelaIncBrazoIzq;
            abuelaRotPiernaDer += abuelaIncPiernaDer;
            abuelaRotPiernaIzq += abuelaIncPiernaIzq;
            i_curr_steps_abuela++;
        }


        // Animación automática del Esqueleto 3 (reverencias suaves frente a la tumba)
        // Va de 0° (erguido) a 32° (inclinado) de forma suave y continua
        esq3_inclinacion = (1.0f - cos(tiempo * 1.4f)) * 16.0f;
        esq3_rotCabezaX = esq3_inclinacion * 0.45f;
        esq3_rotBrazos = -esq3_inclinacion * 0.6f;
    }


    // Animación para el Esqueleto 1 (saludando junto a la ofrenda)
    esq2_saltoY = 0.0f;
    esq2_giroTorso = sin(tiempo * 1.2f) * 6.0f;           
    esq2_rotCabeza = sin(tiempo * 2.5f) * 8.0f;           
    esq2_rotBrazoDer = -135.0f;                           
    esq2_rotBrazoIzq = sin(tiempo * 2.0f) * 6.0f;         
    esq2_rotPiernaDer = 0.0f;
    esq2_rotPiernaIzq = 0.0f;
}


void getResolution() {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    SCR_WIDTH = mode->width;
    SCR_HEIGHT = (mode->height) - 80;
}


int main() {
    // Se inicializa GLFW
    glfwInit();

    monitors = glfwGetPrimaryMonitor();
    getResolution();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final CG - Panteon Oaxaca", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, my_input);

    // Captura del cursor 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Se inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Compilación de Shaders
    Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
    Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
    Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

    vector<std::string> faces{
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg"
    };

    Skybox skybox = Skybox(faces);

    skyboxShader.use();

    skyboxShader.setInt("skybox", 0);


    // ------------------------------------------------------------------
    // Carga de los modelos del escenario
    // ------------------------------------------------------------------
    Model Piso("resources/objects/Piso/Piso.obj");
    Model Pasto("resources/objects/Pasto/Pasto.obj");
    Model Tumba("resources/objects/Tumba/Tumba.obj");
    Model Tumba2("resources/objects/Tumba2/Tumba2.obj");
    Model Tumba3("resources/objects/Tumba3/Tumba3.obj");
    Model Entrada("resources/objects/Entrada_arco/Entrada_arco.obj");
    Model Muro("resources/objects/Muro/Muro.obj");
    Model Arbol("resources/objects/tree/autumn tree001-obj.obj");
    Model Postes("resources/objects/Postes_Papel_Picado/Postes.obj");
    Model Papel_picado("resources/objects/Postes_Papel_Picado/Papel_picado.obj");
    Model ArbolCentral("resources/objects/tree_center/maple-tree.obj");
    Model Banco("resources/objects/Banco/park-bench.obj");
    Model Lampara("resources/objects/Lampara/spooky-lamp-post.obj");
    Model Calavera_azucar("resources/objects/Calavera_dulce/pastel-sugar-skull.obj");
    Model Pila_Huesos("resources/objects/Pila_Huesos/bone-pile.obj");
    Model Fantasma("resources/objects/Fantasma/Floating+Cape+1.obj");
    Model Ofrenda("resources/objects/Ofrenda/Ofrenda.obj");
    Model Esqueleto("resources/objects/Esqueleto/human-skeleton.obj");
    ModelAnim Mujer1("resources/objects/Mujer1/Shoved.dae");
    Model BailarinTorso("resources/objects/Bailarin/Torso.obj");
    Model BailarinCabeza("resources/objects/Bailarin/Cabeza.obj");
    Model BailarinBrazoDer("resources/objects/Bailarin/BrazoDer.obj");
    Model BailarinBrazoIzq("resources/objects/Bailarin/BrazoIzq.obj");
    Model BailarinPiernaDer("resources/objects/Bailarin/PiernaDer.obj");
    Model BailarinPiernaIzq("resources/objects/Bailarin/PiernaIzq.obj");
    Model AbuelaTorso("resources/objects/Abuela/AbuelaTorso.obj");
    Model AbuelaCabeza("resources/objects/Abuela/AbuelaCabeza.obj");
    Model AbuelaBrazoDer("resources/objects/Abuela/AbuelaBrazoDer.obj");
    Model AbuelaBrazoIzq("resources/objects/Abuela/AbuelaBrazoIzq.obj");
    Model AbuelaPiernaDer("resources/objects/Abuela/AbuelaPiernaDer.obj");
    Model AbuelaPiernaIzq("resources/objects/Abuela/AbuelaPiernaIzq.obj");
    Mujer1.initShaders(animShader.ID);
    
    
    // ------------------------------------------------------------------
    // KeyFrames para la Abuela
    // ------------------------------------------------------------------
    // Comienza en postura de reposo viendo hacia el pasillo
    KeyFrameAbuela[0].posX = 0.0f;
    KeyFrameAbuela[0].posY = 0.0f;
    KeyFrameAbuela[0].posZ = 0.0f;
    KeyFrameAbuela[0].giroTorso = 0.0f;
    KeyFrameAbuela[0].rotCabeza = 0.0f;
    KeyFrameAbuela[0].rotBrazoDer = 0.0f;
    KeyFrameAbuela[0].rotBrazoIzq = 0.0f;
    KeyFrameAbuela[0].rotPiernaDer = 0.0f;
    KeyFrameAbuela[0].rotPiernaIzq = 0.0f;

    // Levanta el brazo derecho para empezar a saludar
    KeyFrameAbuela[1].posX = 0.0f;
    KeyFrameAbuela[1].posY = 1.0f;
    KeyFrameAbuela[1].posZ = 0.0f;
    KeyFrameAbuela[1].giroTorso = -5.0f;
    KeyFrameAbuela[1].rotCabeza = 6.0f;
    KeyFrameAbuela[1].rotBrazoDer = -40.0f;
    KeyFrameAbuela[1].rotBrazoIzq = -5.0f;
    KeyFrameAbuela[1].rotPiernaDer = 3.0f;
    KeyFrameAbuela[1].rotPiernaIzq = -3.0f;

    // Primer movimiento de saludo (baja ligeramente el brazo)
    KeyFrameAbuela[2].posX = 0.0f;
    KeyFrameAbuela[2].posY = 0.0f;
    KeyFrameAbuela[2].posZ = 0.0f;
    KeyFrameAbuela[2].giroTorso = -2.0f;
    KeyFrameAbuela[2].rotCabeza = -4.0f;
    KeyFrameAbuela[2].rotBrazoDer = -18.0f;
    KeyFrameAbuela[2].rotBrazoIzq = -8.0f;
    KeyFrameAbuela[2].rotPiernaDer = 0.0f;
    KeyFrameAbuela[2].rotPiernaIzq = 0.0f;

    // Segundo movimiento de saludo (vuelve a subir el brazo suavemente)
    KeyFrameAbuela[3].posX = 0.0f;
    KeyFrameAbuela[3].posY = 1.0f;
    KeyFrameAbuela[3].posZ = 0.0f;
    KeyFrameAbuela[3].giroTorso = -6.0f;
    KeyFrameAbuela[3].rotCabeza = 6.0f;
    KeyFrameAbuela[3].rotBrazoDer = -40.0f;
    KeyFrameAbuela[3].rotBrazoIzq = -5.0f;
    KeyFrameAbuela[3].rotPiernaDer = -3.0f;
    KeyFrameAbuela[3].rotPiernaIzq = 3.0f;

    // Tercer movimiento de saludo (baja ligeramente el brazo)
    KeyFrameAbuela[4].posX = 0.0f;
    KeyFrameAbuela[4].posY = 0.0f;
    KeyFrameAbuela[4].posZ = 0.0f;
    KeyFrameAbuela[4].giroTorso = -2.0f;
    KeyFrameAbuela[4].rotCabeza = -4.0f;
    KeyFrameAbuela[4].rotBrazoDer = -18.0f;
    KeyFrameAbuela[4].rotBrazoIzq = -8.0f;
    KeyFrameAbuela[4].rotPiernaDer = 0.0f;
    KeyFrameAbuela[4].rotPiernaIzq = 0.0f;

    // Último saludo arriba antes de descansar
    KeyFrameAbuela[5].posX = 0.0f;
    KeyFrameAbuela[5].posY = 1.0f;
    KeyFrameAbuela[5].posZ = 0.0f;
    KeyFrameAbuela[5].giroTorso = -4.0f;
    KeyFrameAbuela[5].rotCabeza = 5.0f;
    KeyFrameAbuela[5].rotBrazoDer = -38.0f;
    KeyFrameAbuela[5].rotBrazoIzq = -5.0f;
    KeyFrameAbuela[5].rotPiernaDer = 2.0f;
    KeyFrameAbuela[5].rotPiernaIzq = -2.0f;

    // Regresa a reposo para cerrar el bucle sin saltos
    KeyFrameAbuela[6].posX = 0.0f;
    KeyFrameAbuela[6].posY = 0.0f;
    KeyFrameAbuela[6].posZ = 0.0f;
    KeyFrameAbuela[6].giroTorso = 0.0f;
    KeyFrameAbuela[6].rotCabeza = 0.0f;
    KeyFrameAbuela[6].rotBrazoDer = 0.0f;
    KeyFrameAbuela[6].rotBrazoIzq = 0.0f;
    KeyFrameAbuela[6].rotPiernaDer = 0.0f;
    KeyFrameAbuela[6].rotPiernaIzq = 0.0f;


    playIndexAbuela = 0;
    i_curr_steps_abuela = 0;
    resetElementsAbuela();
    interpolationAbuela();
    playAbuela = true;

    
    glm::mat4 modelOp = glm::mat4(1.0f);
    glm::mat4 viewOp = glm::mat4(1.0f);
    glm::mat4 projectionOp = glm::mat4(1.0f);


    // ==================================================================
    // Configuración de audio (miniaudio)
    // ==================================================================
    ma_engine engine;
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "Error al iniciar el motor de audio miniaudio" << std::endl;
        return -1;
    }

    // Se carga y configura la música de fondo (mp3)
    ma_sound musicaFondo;

    // Se añade la canción para la ambientación del escenario
    ma_sound_init_from_file(&engine, "resources/music/Spanish Rose.mp3", 0, NULL, NULL, &musicaFondo);

    // La canción se mete dentro de un bucle para que nunca deje de sonar
    ma_sound_set_looping(&musicaFondo, MA_TRUE);

    // Para ajustar el volumen de la canción
    ma_sound_set_volume(&musicaFondo, 0.2f);

    // Se inicia la reproducción
    ma_sound_start(&musicaFondo);


    // ------------------------------------------------------------------
    // Ciclo de renderizado 
    // ------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        skyboxShader.setInt("skybox", 0);

        lastFrame = SDL_GetTicks();

        animate();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activar shader principal para modelos e iluminación
        staticShader.use();
        staticShader.setVec3("viewPos", camera.Position);


        // Luz Direccional (efecto de anochecer azul)
        staticShader.setVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, -0.5f));
        staticShader.setVec3("dirLight.ambient", glm::vec3(0.02f, 0.05f, 0.12f));
        staticShader.setVec3("dirLight.diffuse", glm::vec3(0.08f, 0.12f, 0.20f));
        staticShader.setVec3("dirLight.specular", glm::vec3(0.05f, 0.05f, 0.1f));


        // Veladora 1 (Lado izquierdo de la ofrenda)
        staticShader.setVec3("pointLight[0].position", glm::vec3(-105.0f, 40.0f, 5.0f));
        staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[0].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[0].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[0].constant", 1.0f);
        staticShader.setFloat("pointLight[0].linear", 0.0014f);
        staticShader.setFloat("pointLight[0].quadratic", 0.000007f);


        // Veladora 2 (Lado derecho de la ofrenda)
        staticShader.setVec3("pointLight[1].position", glm::vec3(115.0f, 15.0f, 35.0f));
        staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[1].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[1].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[1].constant", 1.0f);
        staticShader.setFloat("pointLight[1].linear", 0.0014f);
        staticShader.setFloat("pointLight[1].quadratic", 0.000007f);


        // Veladora 3 (Centro 3)
        staticShader.setVec3("pointLight[2].position", glm::vec3(250.0f, 25.0f, -250.0f));
        staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[2].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[2].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[2].constant", 1.0f);
        staticShader.setFloat("pointLight[2].linear", 0.0014f);
        staticShader.setFloat("pointLight[2].quadratic", 0.000007f);


        // Veladora 4 (Arriba Izquierda)
        staticShader.setVec3("pointLight[3].position", glm::vec3(-1000.0f, 25.0f, -1000.0f));
        staticShader.setVec3("pointLight[3].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[3].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[3].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[3].constant", 1.0f);
        staticShader.setFloat("pointLight[3].linear", 0.0014f);
        staticShader.setFloat("pointLight[3].quadratic", 0.000007f);


        // Veladora 5 (Arriba Centro)
        staticShader.setVec3("pointLight[4].position", glm::vec3(0.0f, 25.0f, -1200.0f));
        staticShader.setVec3("pointLight[4].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[4].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[4].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[4].constant", 1.0f);
        staticShader.setFloat("pointLight[4].linear", 0.0014f);
        staticShader.setFloat("pointLight[4].quadratic", 0.000007f);


        // Veladora 6 (Arriba Derecha)
        staticShader.setVec3("pointLight[5].position", glm::vec3(1000.0f, 25.0f, -1000.0f));
        staticShader.setVec3("pointLight[5].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[5].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[5].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[5].constant", 1.0f);
        staticShader.setFloat("pointLight[5].linear", 0.0014f);
        staticShader.setFloat("pointLight[5].quadratic", 0.000007f);


        // Veladora 7 (Medio Izquierda)
        staticShader.setVec3("pointLight[6].position", glm::vec3(-1100.0f, 25.0f, 0.0f));
        staticShader.setVec3("pointLight[6].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[6].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[6].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[6].constant", 1.0f);
        staticShader.setFloat("pointLight[6].linear", 0.0014f);
        staticShader.setFloat("pointLight[6].quadratic", 0.000007f);


        // Veladora 8 (Medio Derecha)
        staticShader.setVec3("pointLight[7].position", glm::vec3(1100.0f, 25.0f, 0.0f));
        staticShader.setVec3("pointLight[7].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[7].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[7].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[7].constant", 1.0f);
        staticShader.setFloat("pointLight[7].linear", 0.0014f);
        staticShader.setFloat("pointLight[7].quadratic", 0.000007f);


        // Veladora 9 (Abajo Izquierda)
        staticShader.setVec3("pointLight[8].position", glm::vec3(-1000.0f, 25.0f, 1000.0f));
        staticShader.setVec3("pointLight[8].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[8].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[8].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[8].constant", 1.0f);
        staticShader.setFloat("pointLight[8].linear", 0.0014f);
        staticShader.setFloat("pointLight[8].quadratic", 0.000007f);


        // Veladora 10 (Abajo Centro)
        staticShader.setVec3("pointLight[9].position", glm::vec3(0.0f, 25.0f, 1200.0f));
        staticShader.setVec3("pointLight[9].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[9].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[9].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[9].constant", 1.0f);
        staticShader.setFloat("pointLight[9].linear", 0.0014f);
        staticShader.setFloat("pointLight[9].quadratic", 0.000007f);


        // Veladora 11 (Abajo Derecha)
        staticShader.setVec3("pointLight[10].position", glm::vec3(1000.0f, 25.0f, 1000.0f));
        staticShader.setVec3("pointLight[10].ambient", glm::vec3(0.4f, 0.15f, 0.0f));
        staticShader.setVec3("pointLight[10].diffuse", glm::vec3(1.0f, 0.6f, 0.1f));
        staticShader.setVec3("pointLight[10].specular", glm::vec3(0.8f, 0.4f, 0.1f));
        staticShader.setFloat("pointLight[10].constant", 1.0f);
        staticShader.setFloat("pointLight[10].linear", 0.0014f);
        staticShader.setFloat("pointLight[10].quadratic", 0.000007f);


        // Lámpara Exterior 1 (Izquierda, entrada en Z = 1700)
        staticShader.setVec3("pointLight[11].position", glm::vec3(-155.0f, 180.0f, 1700.0f));
        staticShader.setVec3("pointLight[11].ambient", glm::vec3(0.35f, 0.20f, 0.05f));
        staticShader.setVec3("pointLight[11].diffuse", glm::vec3(1.0f, 0.72f, 0.25f));
        staticShader.setVec3("pointLight[11].specular", glm::vec3(0.9f, 0.65f, 0.25f));
        staticShader.setFloat("pointLight[11].constant", 1.0f);
        staticShader.setFloat("pointLight[11].linear", 0.0014f);
        staticShader.setFloat("pointLight[11].quadratic", 0.000007f);


        // Lámpara Exterior 2 (Derecha, entrada en Z = 1700)
        staticShader.setVec3("pointLight[12].position", glm::vec3(155.0f, 180.0f, 1700.0f));
        staticShader.setVec3("pointLight[12].ambient", glm::vec3(0.35f, 0.20f, 0.05f));
        staticShader.setVec3("pointLight[12].diffuse", glm::vec3(1.0f, 0.72f, 0.25f));
        staticShader.setVec3("pointLight[12].specular", glm::vec3(0.9f, 0.65f, 0.25f));
        staticShader.setFloat("pointLight[12].constant", 1.0f);
        staticShader.setFloat("pointLight[12].linear", 0.0014f);
        staticShader.setFloat("pointLight[12].quadratic", 0.000007f);

        // SpotLight 
        staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position));
        staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front));
        staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f));
        staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
        staticShader.setVec3("spotLight[0].specular", glm::vec3(0.2f, 0.2f, 0.2f));
        staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.0f)));
        staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(20.0f)));
        staticShader.setFloat("spotLight[0].constant", 1.0f);
        staticShader.setFloat("spotLight[0].linear", 0.009f);
        staticShader.setFloat("spotLight[0].quadratic", 0.003f);


        staticShader.setFloat("material_shininess", 32.0f);


        // Transformaciones de Vista y Proyección
        projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        viewOp = camera.GetViewMatrix();
        staticShader.setMat4("projection", projectionOp);
        staticShader.setMat4("view", viewOp);


        // ------------------------------------------------------------------
        // Dibujo del Escenario
        // ------------------------------------------------------------------
        // Piso de tierra 
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(1.6f));
        staticShader.setMat4("model", modelOp);
        Piso.Draw(staticShader);


        // Piso de pasto 
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(2.1f));
        staticShader.setMat4("model", modelOp);
        Pasto.Draw(staticShader);


        // Árbol central del panteón
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
        // modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        modelOp = glm::scale(modelOp, glm::vec3(200.0f));
        staticShader.setMat4("model", modelOp);
        ArbolCentral.Draw(staticShader);


        // Ofrenda
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 50.0f, 200.0f));
        modelOp = glm::scale(modelOp, glm::vec3(15.0f));
        staticShader.setMat4("model", modelOp);
        Ofrenda.Draw(staticShader);


        // ------------------------------------------------------------------
        // Esqueleto articulado 1 (Lado izquierdo de la ofrenda)
        // ------------------------------------------------------------------
        glm::mat4 modelTempEsq2 = glm::mat4(1.0f);
        float escalaEsq2 = 90.0f;

        // Esqueleto 1 - Torso
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-230.0f, (1.05f * escalaEsq2) + esq2_saltoY, 320.0f));
        modelOp = glm::rotate(modelOp, glm::radians(30.0f + esq2_giroTorso), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTempEsq2 = modelOp;
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinTorso.Draw(staticShader);


        // Esqueleto 1 - Cabeza
        modelOp = glm::translate(modelTempEsq2, glm::vec3(0.0f, 0.42f * escalaEsq2, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(esq2_rotCabeza), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinCabeza.Draw(staticShader);


        // Esqueleto 1 - Brazo Derecho 
        modelOp = glm::translate(modelTempEsq2, glm::vec3(-0.165f * escalaEsq2, 0.275f * escalaEsq2, 0.02f * escalaEsq2));
        modelOp = glm::rotate(modelOp, glm::radians(-115.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(18.0f + sin(tiempo * 4.5f) * 18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoDer.Draw(staticShader);


        // Esqueleto 1 - Brazo Izquierdo
        modelOp = glm::translate(modelTempEsq2, glm::vec3(0.175f * escalaEsq2, 0.34f * escalaEsq2, -0.02f * escalaEsq2));
        modelOp = glm::rotate(modelOp, glm::radians(esq2_rotBrazoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(-18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoIzq.Draw(staticShader);


        // Esqueleto 1 - Pierna Derecha
        modelOp = glm::translate(modelTempEsq2, glm::vec3(-0.105f * escalaEsq2, -0.145f * escalaEsq2, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(esq2_rotPiernaDer), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaDer.Draw(staticShader);


        // Esqueleto 1 - Pierna Izquierda
        modelOp = glm::translate(modelTempEsq2, glm::vec3(0.105f * escalaEsq2, -0.145f * escalaEsq2, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(esq2_rotPiernaIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq2));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaIzq.Draw(staticShader);


        // Fantasma
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movFantasma_x, 15.0f + alturaFantasma, movFantasma_z));
        modelOp = glm::rotate(modelOp, glm::radians(orientaFantasma), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(120.0f));
        staticShader.setMat4("model", modelOp);
        Fantasma.Draw(staticShader);


        // Bancas interiores (Pared frontal / Entrada)
        // 3 bancas del lado izquierdo y 3 del lado derecho
        glm::vec3 posBancas[6] = {
            // Lado izquierdo (de adentro viendo hacia afuera, X negativo)
            glm::vec3(-400.0f, 0.0f, 1500.0f),
            glm::vec3(-700.0f, 0.0f, 1500.0f),
            glm::vec3(-1000.0f, 0.0f, 1500.0f),

            // Lado derecho (X positivo)
            glm::vec3(400.0f, 0.0f, 1500.0f),
            glm::vec3(700.0f, 0.0f, 1500.0f),
            glm::vec3(1000.0f, 0.0f, 1500.0f)
        };


        for (int i = 0; i < 6; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), posBancas[i]);
            modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(85.0f));
            staticShader.setMat4("model", modelOp);
            Banco.Draw(staticShader);
        }

        
        // Mujer sentada en la primera banca izquierda
        animShader.use();
        animShader.setMat4("projection", projectionOp);
        animShader.setMat4("view", viewOp);
        animShader.setVec3("viewPos", camera.Position);
        animShader.setVec3("light.direction", glm::vec3(0.0f, -1.0f, -0.5f));
        animShader.setVec3("light.ambient", glm::vec3(0.45f, 0.45f, 0.50f));
        animShader.setVec3("light.diffuse", glm::vec3(0.85f, 0.85f, 0.90f));
        animShader.setVec3("light.specular", glm::vec3(0.3f, 0.3f, 0.3f));
        animShader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
        animShader.setFloat("material.shininess", 16.0f);
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 0.0f, 1500.0f));
        modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(2.4f));
        animShader.setMat4("model", modelOp);
        Mujer1.Draw(animShader);


        staticShader.use();

    
        // ------------------------------------------------------------------
        // Esqueleto articulado 2 (caminata de derecha a izquierda)
        // ------------------------------------------------------------------
        glm::mat4 modelTemp = glm::mat4(1.0f);
        float escalaBailarin = 95.0f;

        // Esqueleto 2 - Torso 
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + posX, (1.05f * escalaBailarin) + posY, 850.0f + posZ));
        modelOp = glm::rotate(modelOp, glm::radians(giroTorso), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTemp = modelOp;
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinTorso.Draw(staticShader);

        // Esqueleto 2 - Cabeza
        modelOp = glm::translate(modelTemp, glm::vec3(0.0f, 0.42f * escalaBailarin, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(rotCabeza), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinCabeza.Draw(staticShader);

        // Esqueleto 2 - Brazo Derecho
        modelOp = glm::translate(modelTemp, glm::vec3(-0.175f * escalaBailarin, 0.34f * escalaBailarin, -0.02f * escalaBailarin));
        modelOp = glm::rotate(modelOp, glm::radians(rotBrazoDer), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoDer.Draw(staticShader);

        // Esqueleto 2 - Brazo Izquierdo
        modelOp = glm::translate(modelTemp, glm::vec3(0.175f * escalaBailarin, 0.34f * escalaBailarin, -0.02f * escalaBailarin));
        modelOp = glm::rotate(modelOp, glm::radians(rotBrazoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(-18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoIzq.Draw(staticShader);

        // Esqueleto 2 - Pierna Derecha
        modelOp = glm::translate(modelTemp, glm::vec3(-0.105f * escalaBailarin, -0.145f * escalaBailarin, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(rotPiernaDer), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaDer.Draw(staticShader);

        // Esqueleto 2 - Pierna Izquierda
        modelOp = glm::translate(modelTemp, glm::vec3(0.105f * escalaBailarin, -0.145f * escalaBailarin, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(rotPiernaIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaBailarin));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaIzq.Draw(staticShader);


        // ------------------------------------------------------------------
        // Abuela articulada (junto a la ofrenda de lado derecho)
        // ------------------------------------------------------------------
        glm::mat4 modelTempAbuela = glm::mat4(1.0f);
        float escalaAbuela = 80.0f;


        // Abuela - Torso
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(240.0f + abuelaPosX, (1.015f * escalaAbuela) + abuelaPosY, 340.0f + abuelaPosZ));
        modelOp = glm::rotate(modelOp, glm::radians(-30.0f + abuelaGiroTorso), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTempAbuela = modelOp;
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaTorso.Draw(staticShader);


        // Abuela - Cabeza
        modelOp = glm::translate(modelTempAbuela, glm::vec3(0.0f, 0.355f * escalaAbuela, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(abuelaRotCabeza), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaCabeza.Draw(staticShader);


        // Abuela - Brazo Derecho (+X)
        modelOp = glm::translate(modelTempAbuela, glm::vec3(0.135f * escalaAbuela, 0.225f * escalaAbuela, -0.02f * escalaAbuela));
        modelOp = glm::rotate(modelOp, glm::radians(abuelaRotBrazoDer), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaBrazoDer.Draw(staticShader);


        // Abuela - Brazo Izquierdo (-X)
        modelOp = glm::translate(modelTempAbuela, glm::vec3(-0.145f * escalaAbuela, 0.235f * escalaAbuela, -0.02f * escalaAbuela));
        modelOp = glm::rotate(modelOp, glm::radians(abuelaRotBrazoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaBrazoIzq.Draw(staticShader);


        // Abuela - Pierna Derecha
        modelOp = glm::translate(modelTempAbuela, glm::vec3(0.038f * escalaAbuela, -0.085f * escalaAbuela, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(abuelaRotPiernaDer), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaPiernaDer.Draw(staticShader);


        // Abuela - Pierna Izquierda
        modelOp = glm::translate(modelTempAbuela, glm::vec3(-0.038f * escalaAbuela, -0.085f * escalaAbuela, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(abuelaRotPiernaIzq), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaAbuela));
        staticShader.setMat4("model", modelOp);
        AbuelaPiernaIzq.Draw(staticShader);


        // ------------------------------------------------------------------
        // Esqueleto articulado 3 (reverencias frente a una tumba)
        // ------------------------------------------------------------------
        glm::mat4 modelBaseEsq3 = glm::mat4(1.0f);
        glm::mat4 modelTorsoEsq3 = glm::mat4(1.0f);
        float escalaEsq3 = 90.0f;


        // Viendo de frente hacia la tumba 20
        modelBaseEsq3 = glm::translate(glm::mat4(1.0f), glm::vec3(-750.0f, 1.05f * escalaEsq3, 730.0f));
        modelBaseEsq3 = glm::rotate(modelBaseEsq3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        // Esqueleto 3 - Pierna Derecha 
        modelOp = glm::translate(modelBaseEsq3, glm::vec3(-0.105f * escalaEsq3, -0.145f * escalaEsq3, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaDer.Draw(staticShader);


        // Esqueleto 3 - Pierna Izquierda 
        modelOp = glm::translate(modelBaseEsq3, glm::vec3(0.105f * escalaEsq3, -0.145f * escalaEsq3, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinPiernaIzq.Draw(staticShader);


        // Esqueleto 3 - Torso 
        modelTorsoEsq3 = glm::rotate(modelBaseEsq3, glm::radians(esq3_inclinacion), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelTorsoEsq3, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinTorso.Draw(staticShader);


        // Esqueleto 3 - Cabeza 
        modelOp = glm::translate(modelTorsoEsq3, glm::vec3(0.0f, 0.42f * escalaEsq3, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(esq3_rotCabezaX), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinCabeza.Draw(staticShader);


        // Esqueleto 3 - Brazo Derecho
        modelOp = glm::translate(modelTorsoEsq3, glm::vec3(-0.175f * escalaEsq3, 0.34f * escalaEsq3, -0.02f * escalaEsq3));
        modelOp = glm::rotate(modelOp, glm::radians(esq3_rotBrazos), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoDer.Draw(staticShader);


        // Esqueleto 3 - Brazo Izquierdo
        modelOp = glm::translate(modelTorsoEsq3, glm::vec3(0.175f * escalaEsq3, 0.34f * escalaEsq3, -0.02f * escalaEsq3));
        modelOp = glm::rotate(modelOp, glm::radians(esq3_rotBrazos), glm::vec3(1.0f, 0.0f, 0.0f));
        modelOp = glm::rotate(modelOp, glm::radians(-18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelOp = glm::scale(modelOp, glm::vec3(escalaEsq3));
        staticShader.setMat4("model", modelOp);
        BailarinBrazoIzq.Draw(staticShader);


        // Muro trasero 
        for (int i = 0; i < 33; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1600.0f + (i * 100.0f), 0.0f, -1600.0f));
            modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(3.0f));
            staticShader.setMat4("model", modelOp);
            Muro.Draw(staticShader);
        }


        // Muro lateral izquierdo 
        for (int i = 0; i < 31; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1600.0f, 0.0f, -1500.0f + (i * 100.0f)));
            modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(3.0f));
            staticShader.setMat4("model", modelOp);
            Muro.Draw(staticShader);
        }


        // Muro lateral derecho 
        for (int i = 0; i < 31; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1600.0f, 0.0f, -1500.0f + (i * 100.0f)));
            modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(3.0f));
            staticShader.setMat4("model", modelOp);
            Muro.Draw(staticShader);
        }


        // Lado izquierdo 
        for (int i = 0; i < 15; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1600.0f + (i * 100.0f), 0.0f, 1600.0f));
            modelOp = glm::scale(modelOp, glm::vec3(3.0f));
            staticShader.setMat4("model", modelOp);
            Muro.Draw(staticShader);
        }


        // Lado derecho 
        for (int i = 0; i < 15; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f + (i * 100.0f), 0.0f, 1600.0f));
            modelOp = glm::scale(modelOp, glm::vec3(3.0f));
            staticShader.setMat4("model", modelOp);
            Muro.Draw(staticShader);
        }


        // Entrada en forma de arco
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1600.0f));
        modelOp = glm::scale(modelOp, glm::vec3(12.0f));
        staticShader.setMat4("model", modelOp);
        Entrada.Draw(staticShader);


        // Lampara 1 (Izquierda)
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 0.0f, 1700.0f));
        modelOp = glm::scale(modelOp, glm::vec3(90.0f)); 
        staticShader.setMat4("model", modelOp);
        Lampara.Draw(staticShader);


        // Lampara 2 (Derecha )
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, 0.0f, 1700.0f));
        modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(90.0f)); 
        staticShader.setMat4("model", modelOp);
        Lampara.Draw(staticShader);


        // Calavera de azucar 1 (Lado izquierdo, al frente de la ofrenda)
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-155.0f, 0.0f, 415.0f));
        modelOp = glm::rotate(modelOp, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(140.0f));
        staticShader.setMat4("model", modelOp);
        Calavera_azucar.Draw(staticShader);


        // Calavera de azucar 2 (Lado derecho, al frente de la ofrenda)
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(155.0f, 0.0f, 415.0f));
        modelOp = glm::rotate(modelOp, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(140.0f));
        staticShader.setMat4("model", modelOp);
        Calavera_azucar.Draw(staticShader);


        glm::vec3 posPilaHuesos[12] = {
            glm::vec3(-1350.0f, 0.0f, -1150.0f), // Zona superior izquierda
            glm::vec3(1150.0f, 0.0f, -1250.0f), // Zona superior derecha 
            glm::vec3(-500.0f, 0.0f, -1050.0f), // Pasillo central superior
            glm::vec3(500.0f, 0.0f, -1050.0f), // Pasillo central superior 
            glm::vec3(-1300.0f, 0.0f,  -200.0f), // Lateral izquierdo 
            glm::vec3(1300.0f, 0.0f,  -200.0f), // Lateral derecho
            glm::vec3(-850.0f, 0.0f,   200.0f), // Zona central-izquierda 
            glm::vec3(850.0f, 0.0f,   200.0f), // Zona central-derecha 
            glm::vec3(-1250.0f, 0.0f,   800.0f), // Lateral izquierdo 
            glm::vec3(1250.0f, 0.0f,   800.0f), // Lateral derecho 
            glm::vec3(-550.0f, 0.0f,   950.0f), // Cerca de la entrada 
            glm::vec3(550.0f, 0.0f,   950.0f)  // Cerca de la entrada
        };

        float rotPilaHuesos[12] = {
            35.0f, 120.0f, 210.0f, 75.0f, 15.0f, 240.0f, 180.0f, 90.0f, 305.0f, 160.0f, 45.0f, 290.0f
        };

        float escalaPilaHuesos[12] = {
            110.0f, 115.0f, 112.0f, 110.0f, 115.0f, 111.0f, 114.0f, 110.0f, 115.0f, 112.0f, 110.0f, 113.0f
        };

        for (int i = 0; i < 12; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), posPilaHuesos[i]);
            modelOp = glm::rotate(modelOp, glm::radians(rotPilaHuesos[i]), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(escalaPilaHuesos[i]));
            staticShader.setMat4("model", modelOp);
            Pila_Huesos.Draw(staticShader);
        }

        
        // Árboles traseros
        for (int i = 0; i < 9; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1600.0f + (i * 400.0f), 0.0f, -1900.0f));
            modelOp = glm::rotate(modelOp, glm::radians(i * 37.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(0.6f));
            staticShader.setMat4("model", modelOp);
            Arbol.Draw(staticShader);
        }


        // Árboles frontales
        for (int i = 0; i < 9; i++) {
            if (i == 4) continue; 
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1600.0f + (i * 400.0f), 0.0f, 1900.0f));
            modelOp = glm::rotate(modelOp, glm::radians(i * 61.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(0.6f));
            staticShader.setMat4("model", modelOp);
            Arbol.Draw(staticShader);
        }


        // Árboles izquierdos
        for (int i = 0; i < 9; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1900.0f, 0.0f, -1600.0f + (i * 400.0f)));
            modelOp = glm::rotate(modelOp, glm::radians(i * 43.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(0.6f));
            staticShader.setMat4("model", modelOp);
            Arbol.Draw(staticShader);
        }


        // Árboles derechos
        for (int i = 0; i < 9; i++) {
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1900.0f, 0.0f, -1600.0f + (i * 400.0f)));
            modelOp = glm::rotate(modelOp, glm::radians(i * 19.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(0.6f));
            staticShader.setMat4("model", modelOp);
            Arbol.Draw(staticShader);
        }


        // Postes
        for (int i = -4; i <= 4; i++) {
            if (i == 0) continue; 
            float posZ = i * 350.0f;
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, posZ));
            modelOp = glm::scale(modelOp, glm::vec3(15.0f));
            staticShader.setMat4("model", modelOp);
            Postes.Draw(staticShader);
        }


        // Papeles picados
        for (int i = -4; i <= 4; i++) {
            if (i == 0) continue;
            float posZ = i * 350.0f;
            modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 700.0f, posZ));
            modelOp = glm::rotate(modelOp, anguloViento, glm::vec3(1.0f, 0.0f, 0.0f));
            modelOp = glm::scale(modelOp, glm::vec3(15.0f));
            staticShader.setMat4("model", modelOp);
            Papel_picado.Draw(staticShader);
        }

        
        // Tumba 1 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1200.0f, 0.0f, -1350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 2 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-700.0f, 0.0f, -1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 3 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 0.0f, -1400.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 4 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 0.0f, -1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 5 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(900.0f, 0.0f, -1350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 6 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1400.0f, 0.0f, -850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 7 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-850.0f, 0.0f, -850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 8 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 0.0f, -850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 9 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1150.0f, 0.0f, -900.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 10 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1100.0f, 0.0f, -400.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 11 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-550.0f, 0.0f, -450.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 12 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -500.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 13 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(800.0f, 0.0f, -400.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 14 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1350.0f, 0.0f, 100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 15 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-750.0f, 0.0f, 50.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 16 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.0f, 100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 17 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1300.0f, 0.0f, 50.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 18 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1000.0f, 0.0f, 600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 19 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-450.0f, 0.0f, 550.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 20 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(550.0f, 0.0f, 600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 21 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1050.0f, 0.0f, 550.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 22 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1300.0f, 0.0f, 1100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 23 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-700.0f, 0.0f, 1050.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 24 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(750.0f, 0.0f, 1100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 25 con diseño Tumba
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1250.0f, 0.0f, 1050.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(3.5f));
        staticShader.setMat4("model", modelOp);
        Tumba.Draw(staticShader);


        // Tumba 1 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-950.0f, 0.0f, -1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 2 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-450.0f, 0.0f, -1250.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 3 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, -1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 4 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(650.0f, 0.0f, -1250.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 5 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1250.0f, 0.0f, -1350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 6 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1150.0f, 0.0f, -800.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 7 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-550.0f, 0.0f, -850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 8 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(350.0f, 0.0f, -800.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 9 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(900.0f, 0.0f, -850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 10 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1450.0f, 0.0f, -800.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 11 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1350.0f, 0.0f, -350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 12 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-850.0f, 0.0f, -400.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 13 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(550.0f, 0.0f, -350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 14 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1100.0f, 0.0f, -400.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 15 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1050.0f, 0.0f, 150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 16 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-450.0f, 0.0f, 100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 17 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(750.0f, 0.0f, 150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 18 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1050.0f, 0.0f, 100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 19 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1350.0f, 0.0f, 650.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 20 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-750.0f, 0.0f, 600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 21 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(850.0f, 0.0f, 650.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 22 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1350.0f, 0.0f, 600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 23 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1050.0f, 0.0f, 1150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 24 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-450.0f, 0.0f, 1100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 25 con diseño Tumba2
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 0.0f, 1150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(8.0f));
        staticShader.setMat4("model", modelOp);
        Tumba2.Draw(staticShader);


        // Tumba 1 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1450.0f, 0.0f, -1250.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 2 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1100.0f, 0.0f, -1050.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 3 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, 0.0f, -1000.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 4 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, -1050.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 5 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1050.0f, 0.0f, -1100.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 6 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1350.0f, 0.0f, -600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 7 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-750.0f, 0.0f, -650.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 8 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, -600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 9 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(750.0f, 0.0f, -650.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 10 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1300.0f, 0.0f, -600.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 11 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1150.0f, 0.0f, -150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 12 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-650.0f, 0.0f, -200.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 13 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.0f, -150.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 14 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1250.0f, 0.0f, -200.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 15 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1250.0f, 0.0f, 350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 16 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-850.0f, 0.0f, 300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 17 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(950.0f, 0.0f, 300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 18 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1450.0f, 0.0f, 350.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 19 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1150.0f, 0.0f, 850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 20 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f, 0.0f, 800.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 21 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f, 0.0f, 850.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 22 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1150.0f, 0.0f, 800.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 23 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);


        // Tumba 24 con diseño Tumba3
        modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(850.0f, 0.0f, 1300.0f));
        modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOp = glm::scale(modelOp, glm::vec3(5.5f));
        staticShader.setMat4("model", modelOp);
        Tumba3.Draw(staticShader);
        

        // Skybox
        skyboxShader.use();
        glm::mat4 skyboxView = glm::rotate(viewOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        skybox.Draw(skyboxShader, skyboxView, projectionOp, camera);

        
        // Limitador de framerate a 60 FPS
        deltaTime = SDL_GetTicks() - lastFrame;
        if (deltaTime < LOOP_TIME) {
            SDL_Delay((int)(LOOP_TIME - deltaTime));
            deltaTime = LOOP_TIME;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Para limpiar la memoria de audio antes de cerrar la ventana
    ma_sound_uninit(&musicaFondo);
    ma_engine_uninit(&engine);

    glfwTerminate();
    return 0;
}


// ------------------------------------------------------------------
// Control de teclado y cámara
// ------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Desplazamiento libre de la cámara sintética
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, (float)deltaTime * MovementSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, (float)deltaTime * MovementSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, (float)deltaTime * MovementSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, (float)deltaTime * MovementSpeed);


    // Posiciones rápidas de cámara (teclas 1, 2 y 3)
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        
        // 1) Fachada exterior
        camera.Position = glm::vec3(0.0f, 130.0f, 2180.0f);
        camera.Yaw = -90.0f; 
        camera.Pitch = -5.0f; 
        camera.Front = glm::normalize(glm::vec3(0.0f, sin(glm::radians(-5.0f)), -cos(glm::radians(-5.0f))));
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        // 2) Enfrente del esqueleto articulado (Z = 850)
        camera.Position = glm::vec3(0.0f, 95.0f, 1130.0f);
        camera.Yaw = -90.0f;
        camera.Pitch = -2.0f;
        camera.Front = glm::normalize(glm::vec3(0.0f, sin(glm::radians(-2.0f)), -cos(glm::radians(-2.0f))));
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        // 3) Enfrente de la ofrenda central encuadrando también ambas calaveras
        camera.Position = glm::vec3(0.0f, 85.0f, 650.0f);
        camera.Yaw = -90.0f;
        camera.Pitch = -4.0f;
        camera.Front = glm::normalize(glm::vec3(0.0f, sin(glm::radians(-4.0f)), -cos(glm::radians(-4.0f))));
    }

    // Iniciar / Detener baile por KeyFrames con tecla P
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        if (!play && FrameIndex > 1) {
            playIndex = 0;
            i_curr_steps = 0;
            resetElements();
            interpolation();
            play = true;
        }
        else {
            play = false;
        }
    }

    // Iniciar / Detener animación KeyFrames de la Abuela con tecla O
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        if (!playAbuela && FrameIndexAbuela > 1) {
            playIndexAbuela = 0;
            i_curr_steps_abuela = 0;
            resetElementsAbuela();
            interpolationAbuela();
            playAbuela = true;
        }
        else {
            playAbuela = false;
        }
    }
    
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
