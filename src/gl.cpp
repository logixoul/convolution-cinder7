#include "StdAfx.h"
#include "gl.h"
#include "util.h"
#include "getOpt.h"
using namespace std;

static void printShaderInfoLog(GLuint obj, string path)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	int ok; glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);

	if (infologLength > 0 && ok != GL_TRUE)
	{
	    infoLog = (char *)malloc(infologLength);
	    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		cout << "Problem compiling " << path << ": " << infoLog << endl;
	    free(infoLog);
	}
}

static void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	int ok; glGetProgramiv(obj, GL_LINK_STATUS, &ok);

	if (infologLength > 0 && ok != GL_TRUE)
	{
	    infoLog = (char *)malloc(infologLength);
	    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		cout << "Problem when linking: " << infoLog << endl;
	    free(infoLog);
	}
}

void Shader::SendConfigUniforms()
{
	for(auto i = getOpts.begin(); i != getOpts.end(); i++)
	{
		(*i)(*this);
	}
}
string Shader::shaderStr(string const& in)
{
	regex rx("#(.)\\s*\\(\\s*\"(.*?)\"\\s*,\\s*\"(.*?)\"\\s*,\\s*(.*?)\\s*\\)");
	string addedUniforms="";
	const std::sregex_iterator end;
	for (std::sregex_iterator iter(in.begin(), in.end(), rx); iter != end; ++iter)
	{
		string type = string((*iter)[1]);
		string name = string((*iter)[2]);
		string opts = string((*iter)[3]);
		string defaultValue = string((*iter)[4]);
		if(type=="b")
		{
			getOpts.push_back([name,opts,defaultValue](Shader& self){
				bool b = GetOpt::getOpt<bool>(name, opts, defaultValue == "true" ? true : defaultValue == "false" ? false : (throw 0));
				glUniform1i(glGetUniformLocation(self.id, name.c_str()), b);
			});
			addedUniforms += "uniform bool " + name + ";\n";
		} else if(type=="f")
		{
			getOpts.push_back([name,opts,defaultValue](Shader& self){
				float f = GetOpt::getOpt<float>(name, opts, Parse<float>(defaultValue));
				glUniform1f(glGetUniformLocation(self.id, name.c_str()), f);
			});
			addedUniforms += "uniform float " + name + ";\n";
		}
	}

	string result = "#version 130\n" + addedUniforms + regex_replace(in, rx, string("$2"));
	return result;
}

int Shader::loadComponent(string const& path, GLenum type)
{
	int id = glCreateShader(type);
	vector<unsigned char> inBuffer; loadFile(inBuffer, path);
	string strBuffer = shaderStr(string((char*)&inBuffer[0], (char*)&inBuffer[0]+inBuffer.size()));
	char* sources[] = { &strBuffer[0] };
	int lengths[] = { strBuffer.size() };
	glShaderSource(id, 1, (const char**)sources, lengths);
	glCompileShader(id);
	printShaderInfoLog(id, path);
	return id;
}

Shader::Shader(string const& vertName, string const& fragName)
{
	id = glCreateProgram();
	
	vertID = loadComponent(vertName + "_vs", GL_VERTEX_SHADER);
	fragID = loadComponent(fragName + "_fs", GL_FRAGMENT_SHADER);

	glAttachShader(id, vertID);
	glAttachShader(id, fragID);
	glLinkProgram(id);
	printProgramInfoLog(id);
}
