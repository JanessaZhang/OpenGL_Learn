#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 编译shader
static unsigned int compile_shader(unsigned int type,
                                   const std::string& source) {
    // 根据类型创建shader
    unsigned int id = glCreateShader(type);
    // 获得shader源码
    const char* src = source.c_str();
    // 绑定源码到shader上
    glShaderSource(id, 1, &src, nullptr);
    // 编译shader
    glCompileShader(id);

    // TODO：error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "!"
                  << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// 创建shader
// shader的源码传进去，OPenGl编译源码，然后链接
static unsigned int create_shader(const std::string& vertex_shader,
                                  const std::string& fragment_shader) {
    // 创建缓冲区program
    unsigned int program = glCreateProgram();
    // 编译 获得两个编译好的shader
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
    // 链接 把shader 绑定到缓冲区program  然后链接program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main() {
    // glfw init
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建window
    GLFWwindow* window = glfwCreateWindow(640, 480, "first window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    //指定上下文
    glfwMakeContextCurrent(window);

    // glad init
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 创建buffer，并绑定，传入数据
    float positions[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW);
    
	// 我的PC上需要加上VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 顶点属性
    // 启用顶点属性
    glEnableVertexAttribArray(0);
    // 一个顶点有多少属性就调用几次，现在只有位置一个属性，只调用一次
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //创建、绑定shader
    //过程：DrawCall->vertex shader->fragment shader
    // shader
    // 顶点 vertex shader   决定输入位置

    // 片段 fragment shader 决定输出颜色

    std::string vertexshader = "#version 330 core\n"
                               "\n"
                               "layout(location=0) in vec4 position;\n"
                               "\n"
                               "void main()\n"
                               "\n"
                               "{\n"
                               "    gl_Position=position;\n"
                               "}\n";

    std::string fragmentshader = "#version 330 core\n"
                                 "\n"
                                 "layout(location=0) out vec4 color;\n"
                                 "\n"
                                 "void main()\n"
                                 "\n"
                                 "{\n"
                                 "    color=vec4(1.0,0.0,0.0,1.0);\n"
                                 "}\n";

    // 创建shader
    unsigned int shader = create_shader(vertexshader, fragmentshader);
    //绑定shader
    glUseProgram(shader);

    // loop
    while (!glfwWindowShouldClose(window)) {
        // 清除buffer 并设置背景颜色
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.25, 0.75, 0.25, 1.0);

        // 进行绘制
        //无索引缓冲区 this is DrawCall  绘制的就是绑定的那个buffer/类似图层概念
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //有索引缓冲区
        // glDrawElements(GL_TRIANGLES,3,int/short,NULL);

        // 交换buffer，进行显示
        glfwSwapBuffers(window);
        // 分发事件
        glfwPollEvents();
    }

    //终止窗口
    glfwTerminate();
    return 0;
}