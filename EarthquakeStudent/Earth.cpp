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

	Array<Vector3> cpuVerts;
	Array<Vector3> cpuNorms;
	Array<int> cpuIndices;
	Array<Vector2> cpuTexCoords;

	for(int i=0;i<STACKS;i++){
		for (int j = 0; j < SLICES; ++j){
			double latitude = (180.0 / STACKS) * i;
			double longitude = (360.0 / SLICES) * j;
			Vector3 tempVector = getPosition(latitude, longitude);
			cpuVerts.append(tempVector);
			cpuNorms.append(tempVector.unit());
			cpuTexCoords.append(Vector2(longitude / 360.0, latitude / 180.0));
			/* code */
		}
	}

	for (int i = 0; i < STACKS - 1; ++i){
		for (int j = 1; j < SLICES; ++j){
			int jMod = j % SLICES;
			cpuIndices.append(SLICES * i + jMod, (SLICES * (i + 1)) + jMod - 1, (SLICES * i) + jMod - 1);
			cpuIndices.append(SLICES * i + jMod, (SLICES * (i + 1)) + jMod, (SLICES * (i + 1)) + jMod - 1);
		}
	}


	vbuffer = VertexBuffer::create((sizeof(Vector3) + sizeof(Vector3)) * cpuVerts.size() +
		sizeof(int)*cpuIndices.size() + (sizeof(Vector2) * cpuTexCoords.size()) );
	debugAssertGLOk();

	gpuVerts = AttributeArray(cpuVerts, vbuffer);
	debugAssertGLOk();
	gpuNorms = AttributeArray(cpuNorms, vbuffer);
	debugAssertGLOk();
	gpuIndices = IndexStream(cpuIndices, vbuffer);
	debugAssertGLOk();
	gpuTexCoords = AttributeArray(cpuTexCoords, vbuffer);
	debugAssertGLOk();
	
}

Vector3 Earth::getPosition(double latitude, double longitude) {
    // TODO: Given a latitude and longitude as input, return the corresponding 3D x,y,z position
    // on your Earth geometry
    // Should somehow test this to see it returns (0,1,0) for lat,lon = 0, 0
    double y = cos(toRadians(latitude));
//    if (latitude > 90){
//    	y *= -1;
//    }

    return Vector3(sin(toRadians(latitude)) * sin(toRadians(longitude)), y, sin(toRadians(latitude)) * cos(toRadians(longitude)));
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

	args.setAttributeArray("vertex", gpuVerts);
	args.setAttributeArray("normal", gpuNorms);

	args.setIndexStream(gpuIndices);
	args.setPrimitiveType(PrimitiveType::TRIANGLES);

	args.setAttributeArray("texCoord0", gpuTexCoords);

	// rd->setRenderMode(RenderDevice::RENDER_WIREFRAME);
	// rd->setCullFace(CullFace::NONE);
	
	rd->apply(shader, args);
}

void Earth::draw(RenderDevice *rd) {
  rd->pushState();
  rd->setCullFace(CullFace::NONE);

  configureShaderArgs(rd);

  rd->popState();
}


