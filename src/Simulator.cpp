#include "core/Application.h"
#include <iostream>
#include <cstdlib>
#include<math.h>


using namespace std;

struct Particule
{
    vec4 position;
    vec4 velocity;
    // vec4 size;
    Particule(vec3 pos, vec3 vel)
    {
        position = vec4(pos[0], pos[1], pos[2], 1.0f);
        velocity = vec4(vel[0], vel[1], vel[2], 1.0f);
        // size = vec4(siz[0], siz[1], siz[2], 1.0f);
    }
};

class Simulator: public Application
{
private:
    Program* program = NULL;
    Program* compute = NULL;
    Renderer* renderer = NULL;
    Renderer* axeRenderer = NULL;
    Computer* computer = NULL;
    Buffer* particules = NULL;
    Buffer* velocities = NULL;
    int numberParticles= 50000;
public:
    Simulator(int argc, char* argv[]);
    void update(int time);
    void render();
    void setup();
    void teardown();
};

Simulator::Simulator(int argc, char* argv[]) : Application(argc, argv) {}

void Simulator::update(int elapsedTime)
{
    computer->compute(numberParticles/4, 1, 1);
}

void Simulator::render()
{
     renderer->render(PRIMITIVE_POINTS, numberParticles);
     axeRenderer->indexedRender(PRIMITIVE_LINES, 24);
}
void Simulator::setup()
{
    int x,y,z;
    int reduce=3;
    x=10;
    y=8;
    z=12;
    float nbrLevels=numberParticles/z;
    // void srand(int seed);
    vector<Particule> vectParticule;
    for(int a = 0; a < numberParticles; a = a + 1){
      vectParticule.push_back(
        Particule(
          vec3(x/2,y,(int) a/nbrLevels),
          vec3(rand()*a%12+10,-rand()*a%12,rand()*a%12)
          // vec3(5,5,5)
          // vec3(0,0,0)
        )
      );
      // cout << (float)((rand() % 12)/reduce) << endl;
    }
    // empty vector of ints
    setClearColor(0.95f, 0.95f, 0.95f, 1.0f);

    particules = new Buffer(vectParticule.data(), vectParticule.size()*sizeof(Particule));

    vec3 axe[] = {

        vec3(0, 0, 0),
        vec3(x, 0, 0),
        //2
        vec3(0, y, 0),
        vec3(0, 0, z),
        //4
        vec3(x, y, 0),
        vec3(x, 0, z),
        //6
        vec3(0, y, z),
        vec3(x, y, z)
    };
    Buffer* axeBuffer = new Buffer(axe, 8*sizeof(vec3));
    // permet d'obtenir les axes du repère en fonction des vec3
    // axe xyz, face avant, face droite, face arrière, face gauche
    GUInt axeIndex[] = {0,1,0,2,0,3,1,4,4,2, 2,6,6,3, 3,5,5,7,7,6, 7,4,5,1};
    Buffer* axeIndexBuffer = new Buffer(axeIndex, 24*sizeof(GUInt));

    program = new Program();
    program->addShader(Shader::fromFile("shaders/perspective.vert"));
    program->addShader(Shader::fromFile("shaders/black.frag"));
    program->link();

    renderer = program->createRenderer();
    axeRenderer = program->createRenderer();

    renderer->setVertexData("vertex", particules, TYPE_FLOAT, 0, 3, sizeof(Particule));
    //renderer->setVec("vertex", vec2(100,100));
    axeRenderer->setVertexData("vertex", axeBuffer, TYPE_FLOAT, 0, 3, sizeof(vec3));
    axeRenderer->index(axeIndexBuffer);

    mat4 projection = perspective(90.0f, 640.0/480.0, 0.1, 100);

    // camera angle
    // double w=pow(2,0.5);
    double w = 1/2;
    // Camera coordonates, where the camera looks
    // mat4 view = lookat(vec3(-10, 4, -5), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 view = lookat(vec3(15, 13, -10), vec3(0, 3, 0), vec3(0, 1, 0));
    // mat4 view = lookat(vec3(0, 0, -10), vec3(0, 0, 0), vec3(0, 1, 0));
    // mat4 view = lookat(vec3(25, 0, 0), vec3(0, 0, 0), vec3(0, 1, 0));
    // mat4 view = lookat(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));
    // mat4 view = lookat(vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 1, 0));

    renderer->setMatrix("projectionMatrix", projection);
    renderer->setMatrix("modelViewMatrix", view);

    axeRenderer->setMatrix("projectionMatrix", projection);
    axeRenderer->setMatrix("modelViewMatrix", view);

    compute = new Program();
    compute->addShader(Shader::fromFile("shaders/gravity.comp"));
    compute->link();

    computer = compute->createComputer();
    computer->setData(1, particules);
    //computer->setData(2, velocities);
}

void Simulator::teardown()
{

}

int main(int argc, char** argv)
{
    try
    {
        Simulator app = Simulator(argc, argv);
	    return app.run();
    }
	catch(Exception e)
    {
        cout << e.getMessage() << endl;
        return 1;
    }
}
