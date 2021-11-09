// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include "MeshObject.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <chrono>
#include <string>
#include <iostream>

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject CBO;
VertexBufferObject NBO;

// Contains the vertex positions
// The default 6 vertices are used to show axis
std::vector<glm::vec3> V(6);
std::vector<glm::vec3> C(6);
std::vector<glm::vec3> N_v(6);
std::vector<glm::vec3> N_f(6);

// Constants
const glm::mat4 UnitMatrix(
    glm::vec4(1, 0, 0, 0),
    glm::vec4(0, 1, 0, 0),
    glm::vec4(0, 0, 1, 0),
    glm::vec4(0, 0, 0, 1)
);

// Object List
std::vector<MeshObject> ObjectList;
int OBJECT_SELECTED = -1;

// View Matrix constructors
glm::vec3 CamaraPosition(0,0,1);
glm::vec3 CamaraUp(0,1,0);

// MVP Matrices and Perspective-mode-flag
glm::mat4 View(UnitMatrix);
glm::mat4 Perspective(UnitMatrix);
bool IF_PERSPECTIVE;

bool IF_TRACKBALL;
double R, THETA, PHI;

// Transfromation modes
enum mode {
    TRANSLATION_MODE = 0,
    ROTATION_MODE = 1,
};
mode Operation_mode = TRANSLATION_MODE;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


glm::vec3 cursor_pos_in_window(GLFWwindow* window){
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
    glm::vec4 p_canonical_1((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);
    glm::vec4 p_canonical_2((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,1,1);
    glm::vec4 p_world_1 = glm::inverse(View) * glm::inverse(Perspective) * p_canonical_1;
    glm::vec4 p_world_2 = glm::inverse(View) * glm::inverse(Perspective) * p_canonical_2;
    // std::cout << p_world.x << p_world.y << std::endl;
    
    return glm::vec3(p_world_1 - p_world_2);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        glReadPixels(xpos, height - ypos - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &OBJECT_SELECTED);
        std::cout << "selected:" << OBJECT_SELECTED << std::endl; 
    }

    // Upload the change to the GPU
    VBO.update(V);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS){
        if(Operation_mode == ROTATION_MODE){
            switch(key)
            {
                case GLFW_KEY_W:
                    ObjectList[OBJECT_SELECTED].RotateVector.x += 5;
                    break;
                case GLFW_KEY_S:
                    ObjectList[OBJECT_SELECTED].RotateVector.x -= 5;
                    break;
                case GLFW_KEY_A:
                    ObjectList[OBJECT_SELECTED].RotateVector.y += 5;
                    break;
                case GLFW_KEY_D:
                    ObjectList[OBJECT_SELECTED].RotateVector.y -= 5;
                    break;
                case GLFW_KEY_F:
                    ObjectList[OBJECT_SELECTED].RotateVector.z += 5;
                    break;
                case GLFW_KEY_G:
                    ObjectList[OBJECT_SELECTED].RotateVector.z -= 5;
                    break;
            }
        }

        if(Operation_mode == TRANSLATION_MODE){
            switch(key)
            {
                case GLFW_KEY_W:
                    ObjectList[OBJECT_SELECTED].TranslateVector.y += 0.05;
                    break;
                case GLFW_KEY_S:
                    ObjectList[OBJECT_SELECTED].TranslateVector.y -= 0.05;
                    break;
                case GLFW_KEY_A:
                    ObjectList[OBJECT_SELECTED].TranslateVector.x -= 0.05;
                    break;
                case GLFW_KEY_D:
                    ObjectList[OBJECT_SELECTED].TranslateVector.x += 0.05;
                    break;
                case GLFW_KEY_F:
                    ObjectList[OBJECT_SELECTED].TranslateVector.z += 0.05;
                    break;
                case GLFW_KEY_G:
                    ObjectList[OBJECT_SELECTED].TranslateVector.z -= 0.05;
                    break;
            }
        }

        // Change Camara
        if(IF_TRACKBALL){
            std::cout << R << THETA << PHI << std::endl;
            switch(key){
                case GLFW_KEY_UP:
                    THETA -= 0.02;
                    break;
                case GLFW_KEY_DOWN:
                    THETA += 0.02;
                    break;
                case GLFW_KEY_LEFT:
                    PHI += 0.02;
                    break;
                case GLFW_KEY_RIGHT:
                    PHI -= 0.02;
                    break;
                case GLFW_KEY_EQUAL:
                    R -= 0.05;
                    break;
                case GLFW_KEY_MINUS:
                    R += 0.05;
                    break;
            }
            CamaraPosition.x = R * sin(THETA) * cos(PHI);
            CamaraPosition.y = R * cos(THETA);
            CamaraPosition.z = R * sin(THETA) * sin(PHI);
            glm::vec3 look = glm::normalize(CamaraPosition);
            glm::vec3 worldUp(0.0f,0.1f,0.0f);
            glm::vec3 right = glm::cross(look,worldUp);
            CamaraUp = -glm::cross(look,right);
        }
        else{
            switch(key){
                case GLFW_KEY_UP:
                    CamaraPosition.y += 0.05;
                    break;
                case GLFW_KEY_DOWN:
                    CamaraPosition.y -= 0.05;
                    break;
                case GLFW_KEY_LEFT:
                    CamaraPosition.x -= 0.05;
                    break;
                case GLFW_KEY_RIGHT:
                    CamaraPosition.x += 0.05;
                    break;
                case GLFW_KEY_EQUAL:
                    CamaraPosition.z -= 0.05;
                    break;
                case GLFW_KEY_MINUS:
                    CamaraPosition.z += 0.05;
                    break;
            }
        }
        
        switch(key)
        {
            case  GLFW_KEY_1:
                ObjectList.push_back(MeshObject("/home/kurisute/Desktop/CG/assignments/assignment-3/data/cube.off",V.size()));
                std::cout << "cube object loaded!" << std::endl;
                V.insert(V.end(),ObjectList[ObjectList.size()-1].V.begin(),ObjectList[ObjectList.size()-1].V.end());
                C.insert(C.end(),ObjectList[ObjectList.size()-1].C.begin(),ObjectList[ObjectList.size()-1].C.end());
                N_f.insert(N_f.end(),ObjectList[ObjectList.size()-1].N_f.begin(),ObjectList[ObjectList.size()-1].N_f.end());
                N_v.insert(N_v.end(),ObjectList[ObjectList.size()-1].N_v.begin(),ObjectList[ObjectList.size()-1].N_v.end());
                std::cout << V.size() << std::endl;
                OBJECT_SELECTED = ObjectList.size()-1;
                break;
            case GLFW_KEY_2:
                ObjectList.push_back(MeshObject("/home/kurisute/Desktop/CG/assignments/assignment-3/data/bumpy_cube.off",V.size()));
                V.insert(V.end(),ObjectList[ObjectList.size()-1].V.begin(),ObjectList[ObjectList.size()-1].V.end());
                C.insert(C.end(),ObjectList[ObjectList.size()-1].C.begin(),ObjectList[ObjectList.size()-1].C.end());
                N_f.insert(N_f.end(),ObjectList[ObjectList.size()-1].N_f.begin(),ObjectList[ObjectList.size()-1].N_f.end());
                N_v.insert(N_v.end(),ObjectList[ObjectList.size()-1].N_v.begin(),ObjectList[ObjectList.size()-1].N_v.end());
                OBJECT_SELECTED = ObjectList.size()-1;
                break;
            case  GLFW_KEY_3:
                ObjectList.push_back(MeshObject("/home/kurisute/Desktop/CG/assignments/assignment-3/data/bunny.off",V.size()));
                V.insert(V.end(),ObjectList[ObjectList.size()-1].V.begin(),ObjectList[ObjectList.size()-1].V.end());
                C.insert(C.end(),ObjectList[ObjectList.size()-1].C.begin(),ObjectList[ObjectList.size()-1].C.end());
                N_f.insert(N_f.end(),ObjectList[ObjectList.size()-1].N_f.begin(),ObjectList[ObjectList.size()-1].N_f.end());
                N_v.insert(N_v.end(),ObjectList[ObjectList.size()-1].N_v.begin(),ObjectList[ObjectList.size()-1].N_v.end());
                OBJECT_SELECTED = ObjectList.size()-1;
                break;

            // Change Transformation Modes
            case GLFW_KEY_R:
                Operation_mode = ROTATION_MODE;
                break;
            case GLFW_KEY_T:
                Operation_mode = TRANSLATION_MODE;
                break;
            
            // Scale
            case GLFW_KEY_Q:
                ObjectList[OBJECT_SELECTED].ScaleVector += glm::vec3(0.05,0.05,0.05);
                break;
            case GLFW_KEY_E:
                ObjectList[OBJECT_SELECTED].ScaleVector -= glm::vec3(0.05,0.05,0.05);
                break;

            // Change Persepctive Mode
            case GLFW_KEY_P:
                IF_PERSPECTIVE = true;
                break;
            case GLFW_KEY_O:
                IF_PERSPECTIVE = false;
                break;
            
            // Change Trackball Mode
            case GLFW_KEY_N:
                IF_TRACKBALL = false;
                CamaraUp = glm::vec3(0,1,0);
                break;
            case GLFW_KEY_M:
                IF_TRACKBALL = true;
                R = sqrt(pow(CamaraPosition.x,2)+pow(CamaraPosition.y,2)+pow(CamaraPosition.z,2));
                THETA = acos(CamaraPosition.y/R);
                PHI = atan(CamaraPosition.z/CamaraPosition.x);
                break;
            
            // Change Rendering Mode to Selected Object
            case GLFW_KEY_Z:
                ObjectList[OBJECT_SELECTED].Rmode = WIREFRAME;
                break;
            case GLFW_KEY_X:
                ObjectList[OBJECT_SELECTED].Rmode = FLAT;
                break;
            case GLFW_KEY_C:
                ObjectList[OBJECT_SELECTED].Rmode = PHONG;
                break;
            
            default:
                break;
        }
    }
    // Upload the change to the GPU
    VBO.update(V);
}


int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    // Add Axis to illustrate the result better
    VBO.init();
    V[0] = glm::vec3(1e+6,0,0);
    V[1] = glm::vec3(-1e+6,0,0);
    V[2] = glm::vec3(0,1e+6,0);
    V[3] = glm::vec3(0,-1e+6,0);
    V[4] = glm::vec3(0,0,1e+6);
    V[5] = glm::vec3(0,0,-1e+6);
    VBO.update(V);

    CBO.init();
    C[0] = glm::vec3(1,0,0);
    C[1] = glm::vec3(1,0,0);
    C[2] = glm::vec3(0,1,0);
    C[3] = glm::vec3(0,1,0);
    C[4] = glm::vec3(0,0,1);
    C[5] = glm::vec3(0,0,1);
    CBO.update(C);

    NBO.init();
    N_f[0] = glm::vec3(1,0,0);
    N_f[1] = glm::vec3(1,0,0);
    N_f[2] = glm::vec3(0,1,0);
    N_f[3] = glm::vec3(0,1,0);
    N_f[4] = glm::vec3(0,0,1);
    N_f[5] = glm::vec3(0,0,1);
    NBO.update(N_f);

    IF_PERSPECTIVE = true;
    IF_TRACKBALL = false;

    //Add Lightsource
    ObjectList.push_back(MeshObject("/home/kurisute/Desktop/CG/assignments/assignment-3/data/lightcube.off",V.size()));
    V.insert(V.end(),ObjectList[ObjectList.size()-1].V.begin(),ObjectList[ObjectList.size()-1].V.end());
    C.insert(C.end(),ObjectList[ObjectList.size()-1].C.begin(),ObjectList[ObjectList.size()-1].C.end());
    N_f.insert(N_f.end(),ObjectList[ObjectList.size()-1].N_f.begin(),ObjectList[ObjectList.size()-1].N_f.end());
    N_v.insert(N_v.end(),ObjectList[ObjectList.size()-1].N_v.begin(),ObjectList[ObjectList.size()-1].N_v.end());
    OBJECT_SELECTED = ObjectList.size()-1;
    ObjectList[OBJECT_SELECTED].UnitScale = glm::vec3(1,1,1);

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec3 position;"
                    "in vec3 color;"
                    "in vec3 normal;"
                    "out vec3 f_position;"
                    "out vec3 f_color;"
                    "out vec3 f_normal;"
                    "uniform mat4 model;"
                    "uniform mat4 view;"
                    "uniform mat4 perspective;"
                    "void main()"
                    "{"
                    "    gl_Position = perspective * view * model * vec4(position, 1.0);"
                    "    f_color = color;"
                    "    f_normal = mat3(transpose(inverse(model))) * normal;"
                    "    f_position = vec3(model * vec4(position, 1.0));"
                    "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "in vec3 f_color;"
                    "in vec3 f_normal;"
                    "in vec3 f_position;"
                    "out vec4 outColor;"
                    "uniform vec3 lightPos;"
                    "uniform vec3 lightcolor;"
                    "uniform vec3 viewPos;"
                    "uniform bool if_uni_color;"
                    "uniform vec3 uni_color;"
                    "void main()"
                    "{"
                    "    float ambientStrength = 0.1;"
                    "    float specularStrength = 0.5;"
                    "    vec3 norm = normalize(f_normal);"
                    "    vec3 ambient = ambientStrength * lightcolor;"
                    "    vec3 lightdir = normalize(lightPos - f_position);"
                    "    float diff = max(dot(norm, lightdir), 0.0);"
                    "    vec3 diffuse = diff * lightcolor;"
                    "    vec3 viewdir = normalize(viewPos - f_position);"
                    "    vec3 reflectdir = reflect(-lightdir, norm);"
                    "    float spec = pow(max(dot(viewdir,reflectdir), 0.0), 30);"
                    "    vec3 specular = specularStrength * spec * lightcolor;"
                    "    vec3 result = (ambient + diffuse + specular) * f_color;"
                    "    if(if_uni_color)"
                    "       outColor = vec4(uni_color, 1.0);"
                    "    else"
                    "       outColor = vec4(result, 1.0);"
                    "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",CBO);
    program.bindVertexAttribArray("normal",NBO);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClearStencil(-1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        
        VBO.update(V);
        CBO.update(C);
        NBO.update(N_f);
        glm::vec3 light_pos = glm::vec3(ObjectList[0].get_model_matrix() * glm::vec4(ObjectList[0].BaryCenter,1.0));
        glUniform3f(program.uniform("lightPos"),light_pos.x,light_pos.y,light_pos.z);
        glUniform3f(program.uniform("lightcolor"),1.0f,1.0f,1.0f);

        // View Matrix
        View = glm::lookAt(CamaraPosition,glm::vec3(0,0,0),CamaraUp);
        glUniformMatrix4fv(program.uniform("view"),1,GL_FALSE,glm::value_ptr(View));
        glUniform3f(program.uniform("viewPos"),CamaraPosition.x,CamaraPosition.y,CamaraPosition.z);

        // Perspective Matrix
        int width,height;
        glfwGetWindowSize(window, &width, &height);
        float ratio = 1.0f*width/height;
        if(IF_PERSPECTIVE)
            Perspective = glm::perspective(glm::radians(70.0f),ratio,0.1f,100.0f);
        else
            Perspective = glm::ortho(-1.0f*ratio,1.0f*ratio,-1.0f,1.0f,0.1f,100.0f);
        glUniformMatrix4fv(program.uniform("perspective"),1,GL_FALSE,glm::value_ptr(Perspective));

        // Axis Display
        glUniformMatrix4fv(program.uniform("model"),1,GL_FALSE,glm::value_ptr(UnitMatrix));
        glUniform1i(program.uniform("if_uni_color"),true);
        glUniform3f(program.uniform("uni_color"),1.0f,0.0f,0.0f);
        glDrawArrays(GL_LINES,0,2);
        glUniform3f(program.uniform("uni_color"),0.0f,1.0f,0.0f);
        glDrawArrays(GL_LINES,2,2);
        glUniform3f(program.uniform("uni_color"),0.0f,0.0f,1.0f);
        glDrawArrays(GL_LINES,4,2);
        glUniform1i(program.uniform("if_uni_color"),false);

        // Lightsource Display
        glUniformMatrix4fv(program.uniform("model"),1,GL_FALSE,glm::value_ptr(ObjectList[0].get_model_matrix()));
        glUniform1i(program.uniform("if_uni_color"),true);
        if(OBJECT_SELECTED == 0)
            glUniform3f(program.uniform("uni_color"),1.0f,1.0f,0.0f);
        else
            glUniform3f(program.uniform("uni_color"),1.0f,1.0f,1.0f);
        for(int j = ObjectList[0].VBO_Pos;j < ObjectList[0].VBO_Pos + ObjectList[0].V.size();j+=3){
            glStencilFunc(GL_ALWAYS, 0, -1);
            glDrawArrays(GL_TRIANGLES, j, 3);
        }
        glUniform1i(program.uniform("if_uni_color"),false);
        
        // Object Display
        for(int i = 1;i < ObjectList.size();i++){
            glUniformMatrix4fv(program.uniform("model"),1,GL_FALSE,glm::value_ptr(ObjectList[i].get_model_matrix()));
            if(ObjectList[i].Rmode == WIREFRAME){
                if(OBJECT_SELECTED == i){
                    glUniform1i(program.uniform("if_uni_color"),true);
                    glUniform3f(program.uniform("uni_color"),1.0f,1.0f,0.0f);
                }
                for(int j = ObjectList[i].VBO_Pos;j < ObjectList[i].VBO_Pos + ObjectList[i].V.size();j+=3){
                    glStencilFunc(GL_ALWAYS, i, -1);
                    glDrawArrays(GL_LINE_LOOP, j, 3);
                }
                glUniform1i(program.uniform("if_uni_color"),false);
            }

            else if(ObjectList[i].Rmode == FLAT){
                NBO.update(N_f);
                // draw fragments
                if(OBJECT_SELECTED == i){
                    glUniform1i(program.uniform("if_uni_color"),true);
                    glUniform3f(program.uniform("uni_color"),1.0f,1.0f,0.0f);
                }
                for(int j = ObjectList[i].VBO_Pos;j < ObjectList[i].VBO_Pos + ObjectList[i].V.size();j+=3){
                    glStencilFunc(GL_ALWAYS, i, -1);
                    glDrawArrays(GL_TRIANGLES, j, 3);
                }
                glUniform1i(program.uniform("if_uni_color"),false);
                // pass a uniform color to draw wireframe
                glUniform1i(program.uniform("if_uni_color"),true);
                glUniform3f(program.uniform("uni_color"),0.0f,0.0f,0.0f);
                for(int j = ObjectList[i].VBO_Pos;j < ObjectList[i].VBO_Pos + ObjectList[i].V.size();j+=3){
                    glStencilFunc(GL_ALWAYS, i, -1);
                    glDrawArrays(GL_LINE_LOOP, j, 3);
                }
                glUniform1i(program.uniform("if_uni_color"),false);
            }

            else if(ObjectList[i].Rmode == PHONG){
                NBO.update(N_v);
                if(OBJECT_SELECTED == i){
                    glUniform1i(program.uniform("if_uni_color"),true);
                    glUniform3f(program.uniform("uni_color"),1.0f,1.0f,0.0f);
                }
                for(int j = ObjectList[i].VBO_Pos;j < ObjectList[i].VBO_Pos + ObjectList[i].V.size();j+=3){
                    glStencilFunc(GL_ALWAYS, i, -1);
                    glDrawArrays(GL_TRIANGLES, j, 3);
                }
                glUniform1i(program.uniform("if_uni_color"),false);
            }
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();
    CBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
