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
  const int STACKS = 20;
  const int SLICES = 40;

  int x1, x2, y1, y2, z1, z2;

	Array<Vector3> cpuVerts;
	Array<Vector3> cpuNorms;
	Array<int> cpuIndices;
	Array<Vector2> cpuTexCoords;

	for(int i = 0; i <= STACKS; i++){	
		for (int j = 0; j <= SLICES; j++){
			double latitude = (180.0 / STACKS) * i;
			double longitude = (360.0 / SLICES) * j;
			Vector3 tempVector = getPosition(latitude, longitude);
			cpuVerts.append(tempVector);
			cpuNorms.append(tempVector.unit());
			cpuTexCoords.append(Vector2(longitude / 360.0, latitude / 180.0));
		}
	}

	for (int i = 0; i <= STACKS; ++i){
		for (int j = 0; j < SLICES; ++j){
				x1 = ((SLICES + 1) * i) + j;
				y1 = (SLICES + 1) * (i + 1) + j;
				z1 = x1 + 1;

				x2 = z1;
				y2 = y1;
				z2 = y1 + 1;
				cpuIndices.append( x1, y1, z1 );
				cpuIndices.append( x2, y2, z2 );		
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
    return Vector3(sin(toRadians(latitude)) * sin(toRadians(longitude)), 
    			   cos(toRadians(latitude)), sin(toRadians(latitude)) * cos(toRadians(longitude)));
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
	
	rd->apply(shader, args);
}

void Earth::draw(RenderDevice *rd) {
  rd->pushState();
  rd->setCullFace(CullFace::NONE);

  configureShaderArgs(rd);

  rd->popState();
}


