#include "Earth.h"
#include "config.h"

using namespace std;

Earth::Earth(shared_ptr<Camera> camera, shared_ptr<Light> light) {
	defaultCamera = camera;
	this->light = light;

  // Note: TEXTURE_PATH is set in config.h
  tex = Texture::fromFile(TEXTURE_PATH);
  setupGeometry();
  setupShader();
}

void Earth::setupGeometry() {
  // TODO: Write your own code here to tesselate and create texture coordinates for the Earth 
  // and then save the data to VertexBuffers, AttributeArrays, and an IndexStream.  The tesselation
  // should be based on the STACKS and SLICES parameters.
  const int STACKS = 20;
  const int SLICES = 40;


}

Vector3 Earth::getPosition(double latitude, double longitude) {
  // TODO: Given a latitude and longitude as input, return the corresponding 3D x,y,z position 
  // on your Earth geometry

}

void Earth::setupShader() {
	G3D::String vertexShaderPath = DATA_DIR+"gouraud-shader.vrt";
	G3D::String fragmentShaderPath = DATA_DIR+"gouraud-shader.pix";

	debugAssert(FileSystem::exists(vertexShaderPath)); // If this asserts false, then your DATA_DIR is not set correctly in config.h
	debugAssert(FileSystem::exists(fragmentShaderPath));
	shader = Shader::fromFiles(vertexShaderPath, fragmentShaderPath);
}

void Earth::configureShaderArgs(RenderDevice* rd) {
	Args args;
	args.enableG3DArgs(true);
	args.setUniform("wsLight", light->position().xyz().direction());
	args.setUniform("lightColor", light->color);
	args.setUniform("wsEyePosition", defaultCamera->frame().translation);
	args.setUniform("textureSampler", tex, Sampler::video());
	
	//TODO: Set the attribute arrays and primitive types to draw the geometry
	//      that you created in setupGeometry().
	
	rd->apply(shader, args);
}

void Earth::draw(RenderDevice *rd) {
  rd->pushState();
  rd->setCullFace(CullFace::NONE);

  configureShaderArgs(rd);

  rd->popState();
}


