#ifndef SHADER_HEADER_H
#define SHADER_HEADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "vendor/glm/glm.hpp"

struct shader_program_source {
    std::string vertex_source;
    std::string fragment_source;
};

class shader {
private:
    
    std::string m_filepath;
    std::unordered_map<std::string, int> m_uniform_location_cache;

    int GetUniformsLocation(const std::string& name);
    shader_program_source parse_shader(const std::string& file_path);
    unsigned int compile_shader(unsigned int type, const std::string& source);
    unsigned int create_shader(const std::string& vertex_shader,
                               const std::string& fragment_shader);

public:
	unsigned int m_RendererID;
	
    shader(const std::string& filepath);
    ~shader();

    void Bind();
    void Unbind(); 

    // set uniforms
	void SetUniform1iv(const std::string& name, int count, int* value);
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, glm::vec2& value);
    void SetUniform3f(const std::string& name, glm::vec3& value);
	void SetUniform3f(const std::string& name, float v1,float v2,float v3);
    void SetUniform4f(const std::string& name, glm::vec4& value);
	void SetUniform4f(const std::string& name, float v1,float v2,float v3,float v4);

    void SetUniformMat3(const std::string& name, const glm::mat3& mat4);
    void SetUniformMat4(const std::string& name, const glm::mat4& mat4);
};

#endif