#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <fstream>
#include<string>
#include<sstream>

static void GL_Clear_Error()
{
	while(glGetError()!=GL_NO_ERROR);
}

static void GL_Check_Error()
{
	while(GLenum error = glGetError())
	{
		std::cout<<"[opengl error]  ( "<<error<<" )"<<std::endl;
	}
}

struct shader_program_source
{
	std::string vertex_source;
	std::string fragment_source;
};


static shader_program_source parse_shader(const std::string &file_path)
{
	enum class ShaderType
	{
		NONE=-1,VERTEX=0,FRAGMENT=1
	};

	std::ifstream stream(file_path);
	std::string line;
	std::stringstream ss[2];
	ShaderType shader_type=ShaderType::NONE;

	while (getline(stream,line))
	{
		if(line.find("#shader")!=std::string::npos)
		{
			if(line.find("vertex")!=std::string::npos)
			// set vertex mode
			shader_type=ShaderType::VERTEX;
			else if(line.find("fragment")!=std::string::npos)
			// set fragment mode
			shader_type=ShaderType::FRAGMENT;

		}
		else{
			ss[(int)shader_type]<<line<<"\n";
		}
	}
	
	return {ss[0].str(),ss[1].str()};
}

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
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "first window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    //指定上下文
    glfwMakeContextCurrent(window);
	//设置频率
	glfwSwapInterval(1);

    // glad init
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 数据 索引 
	float positions[] = {-0.5f, -0.5f, 
	                      0.5f, -0.5f, 
						  0.5f,  0.5f,
	                     -0.5f,  0.5f};
	unsigned int indices[]={
		0,1,2,
		2,3,0
	};

	//  GLFW_OPENGL_CORE_PROFILE模式需要显式的加上VAO 顶点数组
	// 当未绑定VBO的时候，报1282的error，无法启用顶点属性/设置顶点布局
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建顶点缓冲区
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4*2, positions, GL_STATIC_DRAW);

    // 顶点属性
    // 启用顶点属性 指定顶点的布局
    glEnableVertexAttribArray(0);
    // 一个顶点有多少属性就调用几次，现在只有位置一个属性，只调用一次
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//创建索引缓冲区
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6, indices, GL_STATIC_DRAW);

    //创建、绑定shader
    //过程：DrawCall->vertex shader->fragment shader
    // shader
    // 顶点 vertex shader   决定输入位置

    // 片段 fragment shader 决定输出颜色

	shader_program_source source=parse_shader("res/shader/basic.shader");
    unsigned int shader = create_shader(source.vertex_source, source.fragment_source);

    // 创建shader
    // unsigned int shader = create_shader(vertexshader, fragmentshader);
    //绑定shader
    glUseProgram(shader);

	//读取颜色变量的位置，并设置该变量数据 静态颜色
	int color_location=glGetUniformLocation(shader,"u_color");
	glUniform4f(color_location,0.2,0.3,0.5,1.0);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	//动态颜色
	float r=0.1,increment=0.05;

    // loop
    while (!glfwWindowShouldClose(window)) {
        // 清除buffer 并设置背景颜色
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.25, 0.75, 0.25, 1.0);

        // 进行绘制
        //无索引缓冲区 this is DrawCall  绘制的就是绑定的那个buffer/类似图层概念
        // glDrawArrays(GL_TRIANGLES, 0, 3*2);

		GL_Clear_Error();

		//步骤：绑定shader，绑定顶点数组，绑定索引缓冲区，drawcall        
		glUseProgram(shader);
		//动态颜色
	    glUniform4f(color_location,r,0.3,0.5,1.0);
        glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
		positions[0]+=0.05;
		
		if(r<0.0)
		    increment=0.05;
		if(r>1.0)
		    increment=-0.05;
        r+= increment;

        //有索引缓冲区
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

		GL_Check_Error();

        // 交换buffer，进行显示
        glfwSwapBuffers(window);
        // 分发事件
        glfwPollEvents();
    }

	glDeleteProgram(shader);

    //终止窗口
    glfwTerminate();
    return 0;
}