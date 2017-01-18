#pragma once
#include "StdAfx.h"
using namespace std;

struct Shader
{
	int id, fragID, vertID;
	Shader(string const& vertName, string const& fragName);
	Shader() {}
	void SendConfigUniforms();

private:
	int loadComponent(string const& path, GLenum type);
	string shaderStr(string const& in);
	vector<std::function<void(Shader&)>> getOpts;
	//GetOpt::getOpt<bool>(#name, opts, defaultValue)
};