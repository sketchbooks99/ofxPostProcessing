/*
 *  EdgePass.cpp
 *
 *  Copyright (c) 2012, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "EdgePass.h"

namespace itg
{
    EdgePass::EdgePass(const ofVec2f& aspect, bool arb) :
        RenderPass(aspect, arb, "edge"), hue(0.5f), saturation(0.f)
    {
        string vertShaderSrc = STRINGIFY(
                                         #version 410 core\n
                                         in vec2 texcoord;
                                         in vec4 position;
                                         uniform mat4 modelViewProjectionMatrix;
                                         out vec2 vTexCoord;
                                         void main() {
                                            
                                             gl_Position = position;
                                             vTexCoord = texcoord;
                                         }
        );

        string fragShaderSrc = STRINGIFY(
			#version 410 core\n
            uniform sampler2D tex;
            uniform vec2 aspect;
            uniform float hue;
            uniform float saturation;

            in vec2 vTexCoord;
            out vec4 fragColor;
                                         
            vec2 texel = vec2(1.0 / aspect.x, 1.0 / aspect.y);

            // hard coded matrix values
            mat3 G[9] = mat3[]( mat3( 0.3535533845424652, 0, -0.3535533845424652, 0.5, 0, -0.5, 0.3535533845424652, 0, -0.3535533845424652 ),
                                mat3( 0.3535533845424652, 0.5, 0.3535533845424652, 0, 0, 0, -0.3535533845424652, -0.5, -0.3535533845424652 ),
                                mat3( 0, 0.3535533845424652, -0.5, -0.3535533845424652, 0, 0.3535533845424652, 0.5, -0.3535533845424652, 0 ),
                                mat3( 0.5, -0.3535533845424652, 0, -0.3535533845424652, 0, 0.3535533845424652, 0, 0.3535533845424652, -0.5 ),
                                mat3( 0, -0.5, 0, 0.5, 0, 0.5, 0, -0.5, 0 ),
                                mat3( -0.5, 0, 0.5, 0, 0, 0, 0.5, 0, -0.5 ),
                                mat3( 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.6666666865348816, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204 ),
                                mat3( -0.3333333432674408, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, 0.6666666865348816, 0.1666666716337204, -0.3333333432674408, 0.1666666716337204, -0.3333333432674408 ),
                                mat3( 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408, 0.3333333432674408 ));
            
            vec3 hsv(float h,float s,float v) { return mix(vec3(1.),clamp((abs(fract(h+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),s)*v; }
                                         
            void main(void)
            {
                mat3 I;
                float cnv[9];
                vec3 samp;
            
                /* fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value */
                for (int i=0; i<3; i++)
                {
                    for (int j=0; j<3; j++)
                    {
                        samp = texture(tex, vTexCoord + texel * vec2(i-1.0,j-1.0)).rgb;
                        I[i][j] = length(samp); 
                    }
                }

                /* calculate the convolution values for all the masks */
                for (int i=0; i<9; i++)
                {
                    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
                    cnv[i] = dp3 * dp3; 
                }

                float M = (cnv[0] + cnv[1]) + (cnv[2] + cnv[3]);
                float S = (cnv[4] + cnv[5]) + (cnv[6] + cnv[7]) + (cnv[8] + M); 

                fragColor = vec4(hsv(hue, saturation, sqrt(M/S)), 1.0);
            }
        );
        
        /*ostringstream oss;
        oss << "#version 410 core\n" << endl;
		oss << fragShaderSrc;*/
        // if (arb)
        // {
        //     oss << "#define SAMPLER_TYPE sampler2DRect" << endl;
             //oss << "#extension GL_ARB_texture_rectangle : enable" << endl;
        //     oss << fragShaderSrc;
        // }
        // else
        // {
        //     oss << "#define SAMPLER_TYPE sampler2D" << endl;
        //     oss << fragShaderSrc;
        // }
        
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
		shader.bindDefaults();
        shader.linkProgram();
#ifdef _ITG_TWEAKABLE
        addParameter("hue", this->hue, "min=0 max=1");
        addParameter("saturation", this->saturation, "min=0 max=1");
#endif
    }
    
    void EdgePass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        shader.begin();
        shader.setUniformTexture("tex", readFbo.getTexture(), 0);
        if (arb) shader.setUniform2f("aspect", 1.f, 1.f);
        else shader.setUniform2f("aspect", aspect.x, aspect.y);
        shader.setUniform1f("hue", hue);
        shader.setUniform1f("saturation", saturation);
        
        /*if (arb) texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight(), readFbo.getWidth(), readFbo.getHeight());
        else texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());*/
		quad.draw(OF_MESH_FILL);
        
        shader.end();
        
        writeFbo.end();
    }
}
