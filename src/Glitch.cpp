#include "Glitch.h"
#include "ofMain.h"

namespace itg
{
	Glitch::Glitch(const ofVec2f& aspect, bool arb) : RenderPass(aspect, arb, "Invert")
	{
		string vertShaderSrc = STRINGIFY(
			in vec2 texcoord;
		    in vec4 position;
		    out vec2 vTexCoord;
		    void main() {
			    gl_Position = position;
			    vTexCoord = texcoord;
		    }
		);

		string fragShaderSrc = STRINGIFY(
            // ref: https://github.com/cacheflowe/haxademic/blob/master/data/haxademic/shaders/filters/glitch-shake.glsl
			uniform sampler2D tex;

            in vec2 vTexCoord;
            out vec4 fragColor;

			uniform float time = 0.;
			uniform float glitchSpeed = 0.16;
			uniform float amp = 0.3;
			uniform float crossfade = 1.0;
			uniform float subdivide1 = 64.;
			uniform float subdivide2 = 64.;
			uniform vec2 resolution;

			void main() {
				vec2 uv = vTexCoord;
				vec2 block = floor((uv * resolution) / vec2(16));
				vec2 uv_noise = block / subdivide1;
				uv_noise += floor(vec2(time) * vec2(1234.0, 3543.0)) / subdivide2;
				uv_noise /= resolution;

				float block_thresh = pow(fract(time * 1236.0453), 2.0) * 0.2;
				float line_thresh = pow(fract(time * 2236.0453), 3.0) * 0.7;

				vec2 uv_r = uv;
				vec2 uv_b = uv;
				vec2 uv_g = uv;
				// glitch some blocks and lines
				if (texture(tex, uv).r > block_thresh || texture(tex, vec2(uv_noise.y, 0.0)).g < line_thresh) {
					vec2 dist = (fract(uv_noise * resolution) - 0.5) * 0.3;
					uv_r += dist * 0.1;
					uv_g += dist * 0.2;
					uv_b += dist * 0.125;
				}
				fragColor.r = texture(tex, uv_r).r;
				fragColor.b = texture(tex, uv_b).b;
				fragColor.g = texture(tex, uv_g).g;
				fragColor.a = 1.0;

				// loose luma for some blocks
				if (texture(tex, uv_noise).g < block_thresh)
					fragColor.rgb = fragColor.ggg;

				// discolor block lines
				if (texture(tex, vec2(uv_noise.y, 0.0)).b * 3.5 < line_thresh)
					fragColor.rgb = vec3(0.0, dot(fragColor.rgb, vec3(1.0)), 0.0);

				// interleave lines in some blocks
				if (texture(tex, uv_noise).g * 1.5 < block_thresh ||
					texture(tex, vec2(uv_noise.y, 0.0)).g * 2.5 < line_thresh) {
					float line = fract((uv.y * resolution.y) / 3.0);
					vec3 mask = vec3(3.0, 0.0, 0.0);
					if (line > 0.333)
						mask = vec3(0.0, 3.0, 0.0);
					if (line > 0.666)
						mask = vec3(0.0, 0.0, 3.0);
					
					fragColor.xyz *= mask;
				}
			}
		);

        ostringstream oss;
        oss << "#version 150" << endl;
        oss << vertShaderSrc << endl;
        shader.setupShaderFromSource(GL_VERTEX_SHADER, oss.str());
        
        oss.str("");
        oss << "#version 150" << endl;
        oss << fragShaderSrc << endl;
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());
        
		shader.bindDefaults();
		shader.linkProgram();
	}

	void Glitch::render(ofFbo & readFbo, ofFbo & writeFbo)
	{
		writeFbo.begin();
		shader.begin();
		shader.setUniformTexture("tex", readFbo.getTexture(), 0);
		shader.setUniform1f("time", ofGetElapsedTimeMillis());
		shader.setUniform2f("resolution", readFbo.getWidth(), readFbo.getHeight());

		//texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);

		shader.end();
		writeFbo.end();
	}
}
