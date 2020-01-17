#include "Rotate.h"
#include "ofMain.h"

namespace itg
{
	Rotate::Rotate(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "Rotate")
	{
		string vertShaderSrc = STRINGIFY(
			#version 150\n
			in vec2 texcoord;
		    in vec4 position;
		    out vec2 vTexCoord;
		    void main() {
			    gl_Position = position;
			    vTexCoord = texcoord;
		    }
		);

		string fragShaderSrc = STRINGIFY(
			#version 150\n
			uniform sampler2D tex;

            in vec2 vTexCoord;
            out vec4 fragColor;

            const float PI = 3.14159265358979323846;

            uniform vec2 resolution;
            uniform float time;

            float remap(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
                return (value - inputMin) * ((outputMax - outputMin) / (inputMax - inputMin)) + outputMin;
            }

            void main() {
                vec2 uv = vTexCoord;
                float w = (0.5 - (uv.x)) * (resolution.x / resolution.y);
                float h = 0.5 - uv.y;
                float distFromCenter = sqrt(w * w + h * h) + time * 0.5;
                float angle = remap(atan(w, h), -PI, PI, 0.0, 1.0);

                vec4 color = texture(tex, vec2(angle, distFromCenter));
                fragColor = color;
            }
		);

		shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
		shader.bindDefaults();
		shader.linkProgram();
	}

	void Rotate::render(ofFbo & readFbo, ofFbo & writeFbo)
	{
		writeFbo.begin();
		shader.begin();
		shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        shader.setUniform2f("resolution", readFbo.getWidth(), readFbo.getHeight());
		shader.setUniform1f("time", ofGetElapsedTimef());
		//texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);

		shader.end();
		writeFbo.end();
	}
}